#pragma once

#include <tcp.h>
#include <optional>
struct State;

struct TcpConnection {
    TcpStream stream = nullptr;
    void Connect(const char *addr);
    void Close();
    State QueryState();
    void SyncState(State &state);
    std::optional<State> AwaitServerMessage();
};
