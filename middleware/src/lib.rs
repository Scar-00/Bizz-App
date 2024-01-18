use chrono::Utc;
use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct State {
    pub gens: Vec<Generator>,
    pub accs: Vec<Account>,
    pub imprints: Vec<Imprint>,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Generator {
    pub loc: String,
    pub element: usize,
    pub filled: chrono::DateTime<Utc>,
    pub next_filling: chrono::DateTime<Utc>,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Account {
    pub name: String,
    pub password: String,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Tame {
    pub name: String,
    pub loc: String,
    pub needs_imprint: chrono::DateTime<Utc>,
    pub amount: usize,
    pub needs_feeding: bool,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Imprint {
    pub name: String,
    pub tames: Vec<Tame>,
}

#[derive(Debug, Serialize, Deserialize)]
pub enum Item {
    Get,
    Update(State),
}
