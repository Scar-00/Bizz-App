#pragma once
#include "accounts.h"
#include "generators.h"
#include "imprints.h"
#include "util/tcp.h"
#include "util/util.h"

#include <vector>

struct State {
    std::vector<Account> accounts;
    std::vector<Generator> generators;
    std::vector<Imprint> imprints;
    bool changed = false;
    static State Create(FFIState state);
    void Sync(TcpConnection tcp_conn);
    void SetUpdated();
};
