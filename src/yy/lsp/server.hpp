#ifndef __hpp_yy_lsp_server__
#define __hpp_yy_lsp_server__

//  --- Global includes ---
#include <iostream>
#include <thread>
#include <cstring>
#include <format>
#include <fstream>
#include <map>
//  --- Project includes ---
#include <json.hpp>
//  --- Local includes ---
#include "rpc.hpp"

#include "lsp-message.hpp"
#include "../scramble/scramble.hpp"

using json = nlohmann::json;

namespace YY
{
    namespace LSP
    {
        class Server
        {
            // State holder for any workspaces information
            YY::Scramble scramble;
            // Is the server running?
            bool running;
            // Is the server no shut and able to receive messages?
            bool no_shut;
        public:
            Server();

            bool is_running();
            YY::LSP::Message peek();
            void run();
            void process(YY::LSP::Message &message);
        };
    }
}

#endif