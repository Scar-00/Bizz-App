#pragma once

#include <stddef.h>

typedef void * TcpStream;

struct FFIAccount {
    const char *name;
    const char *password;
};

struct FFIGenerator {
    const char *loc;
    size_t ele;
    const char *last;
    const char *next;
};

struct FFITame {
    const char *name;
    const char *loc;
    size_t needs_imprint;
    size_t amount;
    bool watch_food;
};

struct FFIImprint {
    const char *name;
    FFITame *tames;
    size_t tames_len;
};

struct FFIState {
    FFIAccount *accounts;
    size_t accounts_len;
    FFIGenerator *generators;
    size_t generators_len;
    FFIImprint *imprints;
    size_t imprints_len;
};

struct FFIServerStatus {
  enum class Tag {
    None,
    Data,
  };

  struct Data_Body {
    FFIState _0;
  };

  Tag tag;
  union {
    Data_Body data;
  };
};

extern "C" {
    TcpStream ffi_connect_to_server(const char *addr);
    void ffi_close_stream(TcpStream stream);
    FFIState ffi_state_query_form_server(TcpStream stream);
    void ffi_state_sync_with_server(TcpStream stream, FFIState state);
    FFIServerStatus ffi_server_await_message(TcpStream stream);
}
