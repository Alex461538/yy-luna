#if !defined(__hpp_yy_rpc__)
#define __hpp_yy_rpc__

//  --- Global includes ---
#include <iostream>
#include <optional>
#include <cstring>
#include <fstream>
#include <map>
#include <thread>
#include <format>
//  --- Project includes ---
#include <json.hpp>
//  --- Local includes ---
#include "../constants.hpp"
#include "../out.hpp"
#include "lsp-message.hpp"

using json = nlohmann::json;

namespace YY
{
    namespace RPC
    {
        extern std::string msg_buf;
        extern YY::Constants::TransportKind transport;

        void set_transport(YY::Constants::TransportKind out);
        const char *inside_of(const char *ptr, const char *data_start, const char *data_end, int displacement);
        const char *consume_trailing(const char *data);
        int64_t bytes_to_int(const char *data);
        std::string encode(json &data);
        std::optional<YY::LSP::Message> decode(std::string &str_buf);
        YY::LSP::Message get();

        void write(json &data);
        void respond(int request_id, json &result);
    }
}

#endif