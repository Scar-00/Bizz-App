use chrono::Utc;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::Arc;
use std::sync::{
    mpsc,
    mpsc::{Receiver, Sender},
};
use std::thread;

static AUTH_TOKEN: &str = "DEV";
static SERVER_IP: &str = "localhost:1000";
//static SERVER_IP: &str = "192.168.0.186:1000";
//static SERVER_IP: &str = "[2a02:908:d81:b9c0:da7d]:4000";

#[derive(Debug)]
enum Message {
    Connected(Arc<TcpStream>),
    Disconnected(SocketAddr),
    Get(Arc<TcpStream>),
    Update(SocketAddr, State),
}

#[derive(Debug, Serialize, Deserialize)]
struct State {
    gens: Vec<Generator>,
    accs: Vec<Account>,
    imprints: Vec<Imprint>,
}

#[derive(Debug, Serialize, Deserialize)]
struct Generator {
    loc: String,
    element: usize,
    filled: chrono::DateTime<Utc>,
    next_filling: chrono::DateTime<Utc>,
}

#[derive(Debug, Serialize, Deserialize)]
struct Account {
    name: String,
    password: String,
}

#[derive(Debug, Serialize, Deserialize)]
struct Tame {
    name: String,
    loc: String,
    needs_imprint: chrono::DateTime<Utc>,
    amount: usize,
    needs_feeding: bool,
}

#[derive(Debug, Serialize, Deserialize)]
struct Imprint {
    name: String,
    tames: Vec<Tame>,
}

#[derive(Debug, Serialize, Deserialize)]
enum Item {
    Get,
    Update(State),
}

use std::net::SocketAddr;

fn deploy_changes(
    clients: &mut HashMap<SocketAddr, Arc<TcpStream>>,
    state: &State,
    src: Option<SocketAddr>,
) -> Result<(), ()> {
    for (addr, stream) in clients {
        if src != Some(*addr) {
            let state_string = serde_json::to_string(state).unwrap();
            write!(stream.as_ref(), "{}", state_string).map_err(|e| {
                eprintln!("[ERROR]: failed to send response: {e}");
            })?;
        }
    }

    return Ok(());
}

static IMPRINT_ACCOUNTS: &[&'static str] = &["Scar", "Janschke", "Koala", "Panda", "Rd", "Nova"];

fn server_handler(receiver: Receiver<Message>) -> Result<(), ()> {
    let mut clients = HashMap::new();
    let mut state = State {
        gens: vec![],
        accs: vec![],
        imprints: IMPRINT_ACCOUNTS
            .iter()
            .map(|a| Imprint {
                name: a.to_string(),
                tames: vec![Tame {
                    name: "test".into(),
                    loc: "test".into(),
                    needs_imprint: Utc::now(),
                    amount: 10,
                    needs_feeding: false,
                }],
            })
            .collect(),
    };
    //loop over all time related things once every minute and send an update to the

    let mut start = Utc::now();
    loop {
        if let Ok(msg) = receiver.try_recv() {
            match msg {
                Message::Connected(stream) => {
                    println!("Client connected");
                    clients.insert(stream.peer_addr().unwrap(), stream.clone());
                }
                Message::Get(sender) => {
                    let msg = serde_json::to_string(&state).unwrap();
                    sender.as_ref().write(msg.as_bytes()).map_err(|e| {
                        eprintln!("[ERROR]: failed to send response: {e}");
                    })?;
                }
                Message::Update(sender, s) => {
                    state = s;
                    println!("{:#?}", state);
                    deploy_changes(&mut clients, &state, Some(sender))?;
                }
                Message::Disconnected(addr) => {
                    clients.remove(&addr);
                }
            }
        }

        let diff = start.signed_duration_since(Utc::now());
        //println!("diff: {}", diff);
        //println!("??: {}", -chrono::Duration::seconds(10));
        if diff <= -chrono::Duration::seconds(10) {
            for gen in &mut state.gens {
                if gen.filled.signed_duration_since(Utc::now()) >= chrono::Duration::hours(18) {
                    gen.element -= 1;
                }
            }
            for imprint in &mut state.imprints {
                for tame in &mut imprint.tames {
                    tame.needs_imprint -= chrono::Duration::minutes(1);
                }
            }
            start = Utc::now();
            let _ = deploy_changes(&mut clients, &state, None);
            println!("{:#?}", state);
        }
    }
}

#[derive(Debug, Serialize, Deserialize)]
struct AuthMessage {
    token: String,
}

fn auth_client(mut stream: &TcpStream) -> Result<(), ()> {
    let mut buf: [u8; 256] = [0; 256];
    let n = stream.read(&mut buf).map_err(|e| {
        eprintln!("[ERROR]: failed to read from stream: {e}");
    })?;

    let buf = &buf[0..n];
    let buf = std::str::from_utf8(buf).map_err(|e| {
        eprintln!("[ERROR]: {e}");
    })?;
    let auth: AuthMessage = serde_json::from_str(buf).map_err(|e| {
        eprintln!("[ERROR]: unrecognised AUTH format: {e}");
    })?;

    if auth.token == AUTH_TOKEN {
        let _ = writeln!(stream, "Authorised client");
        return Ok(());
    }

    let _ = writeln!(stream, "Invalid token");
    return Err(());
}

fn client_handler(stream: Arc<TcpStream>, sender: Sender<Message>) -> Result<(), ()> {
    auth_client(&stream)?;

    sender
        .send(Message::Connected(stream.clone()))
        .map_err(|e| {
            eprintln!("[ERROR]: failed to send meesage: {e}");
        })?;
    let mut vec_buf = [0; 4096];
    loop {
        let n = stream.as_ref().read(&mut vec_buf).map_err(|e| {
            eprintln!("[ERROR]: failed to read from stream: {e}");
        })?;
        let buf = &vec_buf[0..n];
        let buf = std::str::from_utf8(buf).map_err(|e| {
            eprintln!("[ERROR]: {e}");
        })?;
        match serde_json::from_str::<Item>(&buf) {
            Ok(item) => {
                sender
                    .send(match item {
                        Item::Get => Message::Get(stream.clone()),
                        Item::Update(state) => Message::Update(stream.peer_addr().unwrap(), state),
                    })
                    .map_err(|e| {
                        eprintln!("[ERROR]: failed to send meesage: {e}");
                    })?;
            }
            Err(e) => {
                if n > 0 {
                    println!("{e}");
                }
            }
        }
        vec_buf = [0; 4096];
    }
}

fn main() {
    let listener = TcpListener::bind(SERVER_IP).expect("Failed to bind to address");

    let (sender, receiver) = mpsc::channel();
    thread::spawn(|| {
        let _ = server_handler(receiver);
    });

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                let sender = sender.clone();
                thread::spawn(|| {
                    let _ = client_handler(Arc::new(stream), sender);
                });
            }
            Err(e) => {
                eprintln!("Error: {}", e);
            }
        }
    }
}
