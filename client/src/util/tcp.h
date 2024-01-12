#pragma once

#include <tcp.h>
#include <optional>
struct State;

struct TcpConnection {
    TcpStream stream = nullptr;
    void Connect(const char *addr);
    State QueryState();
    void SyncState(State &state);
    std::optional<State> AwaitServerMessage();
};
