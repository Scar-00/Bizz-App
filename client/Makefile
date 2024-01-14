CC = clang++

CFLAGS  = -std=c++20
CFLAGS += -I../lib/glm/
CFLAGS += -I../lib/imgui/
CFLAGS += -Itcp/
CFLAGS += -I../lib/glfw/include
CFLAGS += -I../lib/glad/include
CFLAGS += -Wno-deprecated-declarations

LDFLAGS  = ../lib/glfw/glfw3dll.lib
LDFLAGS += ../lib/imgui/obj/imgui.o
LDFLAGS += ../lib/imgui/obj/imgui_demo.o
LDFLAGS += ../lib/imgui/obj/imgui_draw.o
LDFLAGS += ../lib/imgui/obj/imgui_impl_glfw.o
LDFLAGS += ../lib/imgui/obj/imgui_impl_opengl3.o
LDFLAGS += ../lib/imgui/obj/imgui_stdlib.o
LDFLAGS += ../lib/imgui/obj/imgui_tables.o
LDFLAGS += ../lib/imgui/obj/imgui_widgets.o
LDFLAGS += tcp/target/debug/tcp.lib
LDFLAGS += ../lib/glad/src/glad.o
LDFLAGS += -lComdlg32
LDFLAGS += -lshell32
LDFLAGS += -luserenv
LDFLAGS += -lws2_32
LDFLAGS += -lbcrypt
LDFLAGS += -ladvapi32
LDFLAGS += -lcfgmgr32
LDFLAGS += -lcomctl32
LDFLAGS += -ld2d1
LDFLAGS += -ldwrite
LDFLAGS += -ldxgi
LDFLAGS += -lgdi32
LDFLAGS += -lkernel32
LDFLAGS += -lmsimg32
LDFLAGS += -lole32
LDFLAGS += -lopengl32
LDFLAGS += -lshlwapi
LDFLAGS += -luser32
LDFLAGS += -lwindowscodecs
LDFLAGS += -lwinspool
LDFLAGS += -luserenv
LDFLAGS += -lws2_32
LDFLAGS += -lbcrypt
LDFLAGS += -lntdll

SRC = $(wildcard src/**/**/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
DATA = $(wildcard src/**/**/*.o.json) $(wildcard src/**/*.o.json) $(wildcard src/*.o.json)

BIN = node.exe

build: $(OBJ)
		$(CC) -o $(BIN) $^ $(LDFLAGS)

run: build
	$(BIN)

%.o: %.cpp
		$(CC) -o $@ -c $< -MJ $@.json $(CFLAGS)

data: build
		sed -e '1 s/./[ '\\\n'{/' -e '$$ s/,$$/'\\\n'] /' $(DATA) > compile_commands.json
