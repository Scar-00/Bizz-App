#include "state.h"
#include "accounts.h"
#include "generators.h"
#include "imprints.h"
#include "tcp.h"
#include "util/util.h"

State State::Create(FFIState state) {
    State self;
    for(size_t i = 0; i < state.accounts_len; i++) {
        self.accounts.push_back(Account{ state.accounts[i].name, state.accounts[i].password });
    }
    for(size_t i = 0; i < state.generators_len; i++) {
        self.generators.push_back(Generator{ state.generators[i].loc, state.generators[i].ele, TimeFromPtr(state.generators[i].last), TimeFromPtr(state.generators[i].next) });
    }

    for(size_t i = 0; i < state.imprints_len; i++) {
        FFIImprint *imprint = &state.imprints[i];
        std::vector<Tame> tames;
        for(size_t j = 0; j < imprint->tames_len; j++) {
            tames.push_back(Tame{ imprint->tames[j].name, imprint->tames[j].loc, imprint->tames[j].needs_imprint, imprint->tames[j].amount, imprint->tames[j].watch_food });
        }
        self.imprints.push_back(Imprint{ imprint->name, std::move(tames) });
    }

    return self;
}

void State::Sync(TcpConnection tcp_conn) {
    this->changed = false;
    tcp_conn.SyncState(*this);
}

void State::SetUpdated() {
    this->changed = true;
}
