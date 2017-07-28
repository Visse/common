#pragma once

#include <sstream>

namespace Common
{
    inline std::istringstream GetLine( std::istream &stream ) {
        std::string line;
        std::getline( stream, line );
        return std::istringstream( std::move(line) );
    }
}