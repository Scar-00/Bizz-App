#pragma once

#include <string>

struct Account {
    std::string name;
    std::string password;
    void ChangePassword(std::string &new_password);
    bool Display();
};
