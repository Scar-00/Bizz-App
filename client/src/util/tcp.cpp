#include "tcp.h"
#include "../editor/ui.h"
#include "../state.h"
#include "util.h"
#include <algorithm>
#include <ctime>

void TcpConnection::Connect(const char *addr) {
    stream = ffi_connect_to_server(addr);
}

void TcpConnection::Close() {
    ffi_close_stream(this->stream);
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

    struct StringGenerator { const char *loc; size_t ele; std::string filled; std::string refill; };

    std::vector<StringGenerator> string_gens;
    string_gens.reserve(state.generators.size());
    for(const auto &gen : state.generators) {
        string_gens.push_back(StringGenerator{ gen.location.c_str(), gen.element, PrintTime(gen.date_filled), PrintTime(gen.date_needs_refill) });
    }

    std::vector<FFIGenerator> ffi_gens;
    ffi_accounts.reserve(state.generators.size());
    for(const auto &gen : string_gens) {
        ffi_gens.push_back(FFIGenerator{ gen.loc, gen.ele, gen.filled.c_str(), gen.refill.c_str() });
    }

    std::vector<FFIImprint> ffi_imprints;
    std::vector<std::vector<FFITame>> ffi_tames;
    for(const auto &imprint : state.imprints) {
        std::vector<FFITame> local_ffi_tames;
        std::vector<char *> time_strings;
        for(const auto &tame : imprint.tames) {
            auto time_string = PrintTime(tame.needs_imprint, "%H.%M");
            auto string_alloc = (char *)malloc(time_strings.size() * sizeof(char));
            strncpy(string_alloc, time_string.c_str(), time_string.size());
            time_strings.push_back(string_alloc);
        }
        for(size_t i = 0; i < imprint.tames.size(); i++) {
            const auto &tame = imprint.tames[i];
            local_ffi_tames.push_back(FFITame{ tame.name.c_str(), tame.loc.c_str(), /*time_strings[i].c_str()*/ "4.00", tame.amount, tame.watch_food });
        }
        ffi_tames.push_back(std::move(local_ffi_tames));
    }
    for(size_t i = 0; i < state.imprints.size(); i++) {
        const auto &imprint = state.imprints[i];
        ffi_imprints.push_back(FFIImprint{ imprint.acc, &ffi_tames[i][0], ffi_tames[i].size() });
    }

    FFIState ffi_state = { &ffi_accounts[0], ffi_accounts.size(), &ffi_gens[0], ffi_gens.size(), &ffi_imprints[0], ffi_imprints.size() };
    ffi_state_sync_with_server(stream, ffi_state);
}

std::optional<State> TcpConnection::AwaitServerMessage() {
    auto res = ffi_server_await_message(this->stream);
    if(res.tag == FFIServerStatus::Tag::None) {
        return std::optional<State>();
    }
    return std::optional(State::Create(res.data._0));
}
