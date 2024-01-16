use chrono::Utc;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::net::TcpStream;
use std::sync::Arc;

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
