#include "tcp.h"
#include "../editor/ui.h"
#include "../state.h"

void TcpConnection::Connect(const char *addr) {
    stream = ffi_connect_to_server("localhost:1000");
}

State TcpConnection::QueryState() {
    auto ffi_state = ffi_state_query_form_server(stream);
    return State::Create(ffi_state);
}

void TcpConnection::SyncState(State &state) {
    std::vector<FFIAccount> ffi_accounts;
    ffi_accounts.reserve(state.accounts.size());
    for(const auto &acc : state.accounts) {
        ffi_accounts.push_back(FFIAccount{ acc.name.c_str(), acc.password.c_str() });
    }

    std::vector<FFIGenerator> ffi_gens;
    ffi_accounts.reserve(state.generators.size());
    for(const auto &gen : state.generators) {
        ffi_gens.push_back(FFIGenerator{ gen.location.c_str(), gen.element, PrintTime(gen.date_filled).c_str(), PrintTime(gen.date_needs_refill).c_str() });
    }

    FFIState ffi_state = { &ffi_accounts[0], ffi_accounts.size(), &ffi_gens[0], ffi_gens.size() };
    ffi_state_sync_with_server(stream, ffi_state);
}

std::optional<State> TcpConnection::AwaitServerMessage() {
    auto res = ffi_server_await_message(this->stream);
    if(res.tag == FFIServerStatus::Tag::None) {
        return std::optional<State>();
    }
    return std::optional(State::Create(res.data._0));
}
