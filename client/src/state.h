#pragma once
#include "accounts.h"
#include "generators.h"
#include "util/tcp.h"
#include "util/util.h"

#include <vector>

struct State {
    std::vector<Account> accounts;
    std::vector<Generator> generators;
    bool changed = false;
    static State Create(FFIState state);
    void Sync(TcpConnection tcp_conn);
    void AddGenerator(Generator gen);
    void RemoveGenerator(size_t index);
    void AddAccount(Account acc);
    void RemoveAccount(size_t index);
};
