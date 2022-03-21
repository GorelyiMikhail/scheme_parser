#pragma once

#include <sstream>

#include "parser.h"

class Interpreter {
public:
    Interpreter() = default;

    std::string Run(const std::string);
};
