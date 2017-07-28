#include "StringFormat.h"

#include "stb_sprintf.h"

namespace StringUtils
{
    std::string vprintf(const char* format, std::va_list args)
    {
        struct UserData {
            std::string result;
            char buffer[STB_SPRINTF_MIN];
        } data;

        STBSP_SPRINTFCB *callback = []( char *buf, void *user, int len) -> char* {
            UserData *data = (UserData*)user;
            data->result.append(buf, len);

            return data->buffer;
        };

        stbsp_vsprintfcb( callback, &data, data.buffer, format, args );

        return std::move(data.result);
    }
}
