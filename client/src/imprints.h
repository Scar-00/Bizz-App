#pragma once

static const char *imprint_accounts[] = {"Scar", "Janschke", "Koala", "Panda", "Rd", "Nova"};

#include <string>
#include <time.h>
#include <vector>

struct Tame {
    std::string name;
    std::string loc;
    tm needs_imprint;
    size_t amount;
    bool watch_food;
};

struct State;
struct Imprint {
    const char *acc;
    std::vector<Tame> tames;
    void Display(State &state);
};
