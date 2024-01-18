use std::io::{Read, Write};
use std::net::TcpStream;
use middleware::{Item, State, Imprint, Tame, Account, Generator};

type CStr = *const std::ffi::c_char;

macro_rules! to_cstr {
    ($s: expr) => {{
        let str = std::ffi::CString::new($s).unwrap().into_raw();
        str
    }};
}

macro_rules! from_cstr {
    ($s: expr) => {{
        let str = std::ffi::CStr::from_ptr($s).to_str().unwrap().to_string();
        str
    }};
}

struct FFIAccount {
    name: CStr,
    password: CStr,
}

struct FFIGenrator {
    loc: CStr,
    element: usize,
    last: CStr,
    next: CStr,
}

struct FFITame {
    name: CStr,
    loc: CStr,
    needs_imprint: CStr,
    amount: usize,
    needs_feeding: bool,
}

struct FFIImprint {
    acc: CStr,
    imprints: *mut FFITame,
    imprints_len: usize,
}

impl From<FFIState> for State {
    fn from(value: FFIState) -> Self {
        unsafe {
            let ffi_gens = Vec::from_raw_parts(
                value.generators as *mut FFIGenrator,
                value.generators_len,
                value.generators_len,
            );
            let ffi_accs = Vec::from_raw_parts(
                value.accounts as *mut FFIAccount,
                value.accounts_len,
                value.accounts_len,
            );
            let ffi_imprints = Vec::from_raw_parts(
                value.imprints as *mut FFIImprint,
                value.imprints_len,
                value.imprints_len,
            );
            let accs: Vec<_> = ffi_accs
                .iter()
                .map(|i| Account {
                    name: from_cstr!(i.name),
                    password: from_cstr!(i.password),
                })
                .collect();
            let gens: Vec<_> = ffi_gens
                .iter()
                .map(|i| {
                    let filled = chrono::DateTime::parse_from_str(&from_cstr!(i.last), "%d.%m.%Y").unwrap();
                    let next_filling = chrono::DateTime::parse_from_str(&from_cstr!(i.next), "%d.%m.%Y").unwrap();
                    Generator {
                        loc: from_cstr!(i.loc),
                        element: i.element,
                        filled: filled.into(),
                        next_filling: next_filling.into(),
                    }
                })
                .collect();
            let imprints: Vec<_> = ffi_imprints
                .iter()
                .map(|i| {
                    let ffi_tames = Vec::from_raw_parts(i.imprints, i.imprints_len, i.imprints_len);
                    let imprint = Imprint {
                        name: from_cstr!(i.acc),
                        tames: ffi_tames
                            .iter()
                            .map(|t| {
                                let now = chrono::Utc::now().format("%d.%m.%Y").to_string();
                                let input = now + " -- +0000 -- " + &from_cstr!(t.needs_imprint);
                                println!("input: {input}");
                                let needs_imprint = chrono::DateTime::parse_from_str(&input, "%d.%m.%Y -- %z -- %H:%M").unwrap().into();
                                Tame {
                                    name: from_cstr!(t.name),
                                    loc: from_cstr!(t.loc),
                                    needs_imprint,
                                    amount: t.amount,
                                    needs_feeding: t.needs_feeding,
                                }
                            })
                            .collect(),
                    };
                    ffi_tames.leak();
                    return imprint;
                })
                .collect();
            let s = Self {
                gens,
                accs,
                imprints,
            };
            ffi_gens.leak();
            ffi_accs.leak();
            ffi_imprints.leak();
            return s;
        }
    }
}

impl Into<FFIState> for State {
    fn into(self) -> FFIState {
        let accs: Vec<_> = self
            .accs
            .into_iter()
            .map(|i| FFIAccount {
                name: to_cstr!(i.name),
                password: to_cstr!(i.password),
            })
            .collect();
        let gens: Vec<_> = self
            .gens
            .into_iter()
            .map(|i| FFIGenrator {
                loc: to_cstr!(i.loc),
                element: i.element,
                last: to_cstr!(i.filled.format("%d.%m.%Y").to_string()),
                next: to_cstr!(i.next_filling.format("%d.%m.%Y").to_string()),
            })
            .collect();
        let imprints: Vec<_> = self
            .imprints
            .into_iter()
            .map(|i| {
                let mut ffi_tames: Vec<FFITame> = i
                    .tames
                    .into_iter()
                    .map(|t| FFITame {
                        name: to_cstr!(t.name),
                        loc: to_cstr!(t.loc),
                        needs_imprint: to_cstr!(t.needs_imprint.format("%H:%M").to_string()),
                        amount: t.amount,
                        needs_feeding: t.needs_feeding,
                    })
                    .collect();
                let imprint = FFIImprint {
                    acc: to_cstr!(i.name),
                    imprints: ffi_tames.as_mut_ptr(),
                    imprints_len: ffi_tames.len(),
                };
                ffi_tames.leak();
                return imprint;
            })
            .collect();
        let state = FFIState {
            accounts: accs.as_ptr() as *mut (),
            accounts_len: accs.len(),
            generators: gens.as_ptr() as *mut (),
            generators_len: gens.len(),
            imprints: imprints.as_ptr() as *mut (),
            imprints_len: imprints.len(),
        };
        gens.leak();
        accs.leak();
        imprints.leak();
        return state;
    }
}

#[repr(C)]
pub struct FFIState {
    accounts: *mut (),
    accounts_len: usize,
    generators: *mut (),
    generators_len: usize,
    imprints: *mut (),
    imprints_len: usize,
}

#[repr(C)]
pub enum ResponseType {
    Failed,
    Ok,
}

#[repr(C)]
pub struct Response {
    ty: ResponseType,
    res: FFIState,
}

#[no_mangle]
pub extern "C" fn ffi_connect_to_server(addr: CStr) -> *mut TcpStream {
    let addr = unsafe { std::ffi::CStr::from_ptr(addr).to_str().unwrap() };
    let stream =
        Box::new(TcpStream::connect(addr).expect(&format!("Failed to connect to `{addr}`")));
    let _ = stream.as_ref().write(b"{ \"token\": \"DEV\" }");
    let mut buf: [u8; 256] = [0; 256];
    let n = stream
        .as_ref()
        .read(&mut buf)
        .map_err(|e| {
            eprintln!("[ERROR]: failed to read from stream: {e}");
            return std::ptr::null_mut::<TcpStream>();
        })
        .unwrap();

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf)
        .map_err(|e| {
            eprintln!("[ERROR]: {e}");
            return std::ptr::null_mut::<TcpStream>();
        })
        .unwrap();
    println!("{buf}");
    return Box::<TcpStream>::leak(stream);
}

#[no_mangle]
pub extern "C" fn ffi_close_stream(stream: *mut TcpStream) {
    let stream = unsafe { stream.as_ref().unwrap() };
    let _ = stream.shutdown(std::net::Shutdown::Both);
}

#[no_mangle]
pub extern "C" fn ffi_state_query_form_server(stream: *mut TcpStream) -> FFIState {
    let mut stream = unsafe { stream.as_ref().unwrap() };
    let s = serde_json::to_string(&Item::Get).unwrap();
    println!("req: {s}");
    let _ = write!(stream, "{}", s);
    let mut buf: [u8; 4096] = [0; 4096];
    let n = stream
        .read(&mut buf)
        .map_err(|e| {
            eprintln!("[ERROR]: failed to read from stream: {e}");
        })
        .unwrap();

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf)
        .map_err(|e| {
            eprintln!("[ERROR]: {e}");
        })
        .unwrap();
    println!("res: {buf}");
    let state: State = serde_json::from_str(buf).unwrap();

    println!("state:\n{:#?}", state);
    return state.into();
}

#[no_mangle]
pub extern "C" fn ffi_state_sync_with_server(stream: *mut TcpStream, state: FFIState) {
    println!("Synced State");
    let mut stream = unsafe { stream.as_ref().unwrap() };
    let state: State = state.into();
    let s = serde_json::to_string(&Item::Update(state)).unwrap();
    println!("req: {s}");
    let _ = write!(stream, "{}", s);
}

#[repr(C)]
pub enum FFIServerStatus {
    None,
    Data(FFIState),
}

#[no_mangle]
pub extern "C" fn ffi_server_await_message(stream: *mut TcpStream) -> FFIServerStatus {
    println!("Received server update message");
    let mut stream = unsafe { stream.as_ref().unwrap() };
    let mut buf: [u8; 4096] = [0; 4096];
    let n = match stream.read(&mut buf) {
        Ok(n) => n,
        Err(_) => return FFIServerStatus::None,
    };
    if n == 0 {
        return FFIServerStatus::None;
    }

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf)
        .map_err(|e| {
            eprintln!("[ERROR]: {e}");
        })
        .unwrap();
    println!("res: {buf}");
    let state: State = serde_json::from_str(buf).unwrap();
    return FFIServerStatus::Data(state.into());
}
