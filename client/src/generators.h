#pragma once

#include <string>
#include <ctime>

struct Generator {
    std::string location;
    size_t element;
    tm date_filled;
    tm date_needs_refill;
    bool Display();
};
