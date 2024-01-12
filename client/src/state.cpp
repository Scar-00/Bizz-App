#include "state.h"
#include "accounts.h"
#include "generators.h"

State State::Create(FFIState state) {
    State self;
    for(size_t i = 0; i < state.accounts_len; i++) {
        self.accounts.push_back(Account{ state.accounts[i].name, state.accounts[i].password });
    }
    for(size_t i = 0; i < state.generators_len; i++) {
        self.generators.push_back(Generator{ state.generators[i].loc, state.generators[i].ele, TimeFromPtr(state.generators[i].last), TimeFromPtr(state.generators[i].next) });
    }

    return self;
}

void State::Sync(TcpConnection tcp_conn) {
    this->changed = false;
    tcp_conn.SyncState(*this);
}

void State::AddGenerator(Generator gen) {
    this->changed = true;
    this->generators.push_back(gen);
}

void State::RemoveGenerator(size_t index) {
    this->changed = true;
    this->generators.erase(this->generators.cbegin() + index);
}

void State::AddAccount(Account acc) {
    this->changed = true;
    this->accounts.push_back(acc);
}

void State::RemoveAccount(size_t index) {
    this->changed = true;
    this->accounts.erase(this->accounts.cbegin() + index);
}
