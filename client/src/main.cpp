#include <stdio.h>
#include "context.h"
#include "editor/ui.h"
#include <string_view>
#include <tcp.h>
#include "util/tcp.h"
#include "state.h"

#include <thread>

int main(int argc, char **argv) {
    Window window = Window({ 256, 144 });
    Context::Create(window);
    bool logged_in = false;

    State state;

    TcpConnection server_conn;

    std::thread t{[](Window &window, bool &logged_in, TcpConnection &server_conn, State &state) {
        while(!window.ShouldClose()) {
            if(logged_in) {
                auto res = server_conn.AwaitServerMessage();
                if(res.has_value()) {
                    state = res.value();
                }
                else {
                    break;
                }
            }
            if(window.ShouldClose()) {
                break;
            }
        }
    }, std::ref(window), std::ref(logged_in), std::ref(server_conn), std::ref(state)};

    while(!window.ShouldClose()) {
        window.RenderBegin();

        if(logged_in) {
            Editor::BeginDocking();

            //static bool open = true;
            //ImGui::ShowDemoWindow(&open);

            Editor::RenderBreedingWindow(state);
            Editor::RenderGeneratorsWindow(state);
            Editor::RenderAccountsWindow(state);
            if(state.changed) {
                state.Sync(server_conn);
            }
        }else {
            Editor::RenderLoginWindow(window, logged_in, server_conn, state);
        }


        window.RenderEnd();
    }
    t.detach();
    Context::Destroy();
    return 0;
}
