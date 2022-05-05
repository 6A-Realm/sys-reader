#ifndef util_hpp
#define util_hpp

#include <switch.h>
#include "json.hpp"

using nlohmann::json;

#define INNER_HEAP_SIZE 0xA7000
#define R_ASSERT(res_expr)            \
    ({                                \
        const Result rc = (res_expr); \
        if (R_FAILED(rc))             \
        {                             \
            fatalThrow(rc);           \
        }                             \
    })

#endif

void WriteToLog(std::string message);
void handle_message(const std::string & message);
void buttonPressed(std::string button, json& wsMessage);