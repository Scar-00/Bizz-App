use serde::{Deserialize, Serialize};
use std::io::{Read, Write};
use std::net::TcpStream;

type CStr = *const std::ffi::c_char;

#[derive(Debug, Serialize, Deserialize)]
struct State {
    gens: Vec<Generator>,
    accs: Vec<Account>,
}

#[derive(Debug, Serialize, Deserialize)]
struct Generator {
    loc: String,
    element: usize,
    filled: String,
    next_filling: String,
}

#[derive(Debug, Serialize, Deserialize)]
struct Account {
    name: String,
    password: String,
}

#[derive(Debug, Serialize, Deserialize)]
enum ItemType {
    State,
    Generators,
    Accounts,
}

#[derive(Debug, Serialize, Deserialize)]
enum Item {
    Get,
    Update(State),
}

/*fn main() -> Result<(), ()> {
    let mut stream = TcpStream::connect("192.168.0.186:1000").unwrap();
    let _ = stream.write(b"{ \"token\": \"DEV\" }");
    let mut buf: [u8; 256] = [0; 256];
    let n = stream.read(&mut buf).map_err(|e| {
        eprintln!("[ERROR]: failed to read from stream: {e}");
    })?;

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf).map_err(|e| {
        eprintln!("[ERROR]: {e}");
    })?;
    println!("{buf}");

    let message = Item::Get;
    /*let message = Item::Update(ItemProps::Generators(vec![Generator {
        loc: String::from("swamp"),
        element: 10,
        filled: "2020".to_string(),
        next_filling: "2021".to_owned(),
    }]));*/

    let s = serde_json::to_string(&message).unwrap();
    println!("req: {s}");
    let _ = write!(stream, "{}", s);
    let mut buf: [u8; 256] = [0; 256];
    let n = stream.read(&mut buf).map_err(|e| {
        eprintln!("[ERROR]: failed to read from stream: {e}");
    })?;

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf).map_err(|e| {
        eprintln!("[ERROR]: {e}");
    })?;
    println!("res: {buf}");
    let state: Vec<Account> = serde_json::from_str(buf).unwrap();

    println!("state:\n{:#?}", state);

    loop {}
}*/

macro_rules! to_cstr {
    ($s: expr) => {
        {
            let str = std::ffi::CString::new($s).unwrap().into_raw();
            str
        }
    };
}

macro_rules! from_cstr {
    ($s: expr) => {
        {
            let str = std::ffi::CStr::from_ptr($s).to_str().unwrap().to_string();
            str
        }
    };
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

impl From<FFIState> for State {
    fn from(value: FFIState) -> Self {
        unsafe {
            let ffi_gens = Vec::from_raw_parts(value.generators as *mut FFIGenrator, value.generators_len, value.generators_len);
            let ffi_accs = Vec::from_raw_parts(value.accounts as *mut FFIAccount, value.accounts_len, value.accounts_len);
            let accs: Vec<_> = ffi_accs.iter().map(|i| Account{ name: from_cstr!(i.name), password: from_cstr!(i.password) }).collect();
            let gens: Vec<_> = ffi_gens.iter().map(|i| Generator{ loc: from_cstr!(i.loc), element: i.element, filled: from_cstr!(i.last), next_filling: from_cstr!(i.next) }).collect();
            let s = Self{
                gens,
                accs,
            };
            ffi_gens.leak();
            ffi_accs.leak();
            return s
        }
    }
}

impl Into<FFIState> for State {
    fn into(self) -> FFIState {
        let accs: Vec<_> = self.accs.into_iter().map(|i| {
            FFIAccount{ name: to_cstr!(i.name), password: to_cstr!(i.password) }
        }).collect();
        let gens: Vec<_> = self.gens.into_iter().map(|i| {
            FFIGenrator{ loc: to_cstr!(i.loc), element: i.element, last: to_cstr!(i.filled), next: to_cstr!(i.next_filling) }
        }).collect();
        let state = FFIState {
            accounts:accs.as_ptr() as *mut (),
            accounts_len: accs.len(),
            generators: gens.as_ptr() as *mut (),
            generators_len: gens.len(),
        };
        gens.leak();
        accs.leak();
        return state;
    }
}

#[repr(C)]
pub struct FFIState {
    accounts: *mut (),
    accounts_len: usize,
    generators: *mut (),
    generators_len: usize,
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
    let stream = Box::new(TcpStream::connect(addr).unwrap());
    let _ = stream.as_ref().write(b"{ \"token\": \"DEV\" }");
    let mut buf: [u8; 256] = [0; 256];
    let n = stream.as_ref().read(&mut buf).map_err(|e| {
        eprintln!("[ERROR]: failed to read from stream: {e}");
        return std::ptr::null_mut::<TcpStream>();
    }).unwrap();

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf).map_err(|e| {
        eprintln!("[ERROR]: {e}");
        return std::ptr::null_mut::<TcpStream>();
    }).unwrap();
    println!("{buf}");
    return Box::<TcpStream>::leak(stream);
}

#[no_mangle]
pub extern "C" fn ffi_state_query_form_server(stream: *mut TcpStream) -> FFIState {
    let mut stream = unsafe{ stream.as_ref().unwrap() };
    let s = serde_json::to_string(&Item::Get).unwrap();
    println!("req: {s}");
    let _ = write!(stream, "{}", s);
    let mut buf: [u8; 256] = [0; 256];
    let n = stream.read(&mut buf).map_err(|e| {
        eprintln!("[ERROR]: failed to read from stream: {e}");
    }).unwrap();

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf).map_err(|e| {
        eprintln!("[ERROR]: {e}");
    }).unwrap();
    println!("res: {buf}");
    let state: State = serde_json::from_str(buf).unwrap();

    println!("state:\n{:#?}", state);
    return state.into();
}

#[no_mangle]
pub extern "C" fn ffi_state_sync_with_server(stream: *mut TcpStream, state: FFIState) {
    println!("Synced State");
    let mut stream = unsafe{ stream.as_ref().unwrap() };
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
    let mut stream = unsafe{ stream.as_ref().unwrap() };
    let mut buf: [u8; 256] = [0; 256];
    let n = match stream.read(&mut buf) {
        Ok(n) => n,
        Err(_) => return FFIServerStatus::None,
    };

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf).map_err(|e| {
        eprintln!("[ERROR]: {e}");
    }).unwrap();
    println!("res: {buf}");
    let state: State = serde_json::from_str(buf).unwrap();
    return FFIServerStatus::Data(state.into());
}
