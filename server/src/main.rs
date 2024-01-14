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
//static SERVER_IP: &str = "localhost:1000";
static SERVER_IP: &str = "192.168.0.186:1000";
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

use std::net::SocketAddr;

fn deploy_changes(
    clients: &mut HashMap<SocketAddr, Arc<TcpStream>>,
    state: &State,
    src: SocketAddr,
) -> Result<(), ()> {
    for (addr, stream) in clients {
        if src != *addr {
            let state_string = serde_json::to_string(state).unwrap();
            write!(stream.as_ref(), "{}", state_string).map_err(|e| {
                eprintln!("[ERROR]: failed to send response: {e}");
            })?;
        }
    }

    return Ok(());
}

fn server_handler(receiver: Receiver<Message>) -> Result<(), ()> {
    let mut clients = HashMap::new();
    let mut state = State {
        gens: vec![Generator {
            loc: "Swamp Tp".to_owned(),
            element: 10,
            filled: "30.12.2023".to_owned(),
            next_filling: "31.12.2023".to_owned(),
        }],
        accs: vec![Account {
            name: "test".to_owned(),
            password: "123".to_owned(),
        }],
    };
    loop {
        let msg = receiver.recv().expect("server has ung up");
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
                deploy_changes(&mut clients, &state, sender)?;
            }
            Message::Disconnected(addr) => {
                clients.remove(&addr);
            }
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
