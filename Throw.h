#pragma once

#include "StringFormat.h"

#ifdef IN_IDE_PARSER
void THROW( exception, const char *format, ... );
#endif

#define THROW( exception,  ... ) throw exception( StringUtils::printf(__VA_ARGS__) )


