#include "Input.h"
#include <iostream>

Input::Input() {}

char Input::getKey() {
    char c;
    std::cin >> c;
    return c;
}
