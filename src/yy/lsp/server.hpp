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

#include "../fs/text_document.hpp"

using json = nlohmann::json;

namespace YY
{
    namespace LSP
    {
        class Server
        {
            bool running;
            bool no_shut;

            std::map<std::string, YY::Fs::TextDocument> openedDocuments;

        public:
            Server()
            {
                running = false;
                no_shut = false;
            }

            bool is_running()
            {
                return running;
            }

            void run()
            {
                YY::Debug::log("🐥 Protect chocobos worldwide!\n");
                running = true;
            }

            YY::LSP::Message peek()
            {
                return YY::RPC::get();
            }

            void process(YY::LSP::Message &message)
            {
                if (!running)
                {
                    return;
                }

                json result = {};

                YY::Debug::log("[in]: %s\n\n", std::string(message).c_str());

                if (no_shut)
                {
                    if (message.method == "shutdown")
                    {
                        result = json::parse("null");
                        no_shut = false;
                    }
                    else if (message.method == "textDocument/didOpen")
                    {
                        auto file = message.params.get<YY::Fs::TextDocument>();
                        openedDocuments[file.uri] = file;
                    }
                    else if (message.method == "textDocument/didChange")
                    {

                    }
                    else if (message.method == "textDocument/didClose")
                    {
                        auto params = message.params.get<YY::LSP::Params::DidCloseTextDocumentParams>();
                    }
                    else if (message.method == "textDocument/completion")
                    {
                        result = {
                            {"isIncomplete", false},
                            {"items", {{{"label", "fuck off"}}, {{"label", "dog flesh inc."}}, {{"label", "jelly"}}, {{"label", "tesorito's favorities"}}, {{"label", "giving bombs to carity"}}}}};
                    }
                }
                else
                {
                    if (message.method == "initialize")
                    {
                        auto params = message.params.get<YY::LSP::Params::InitializeParams>();

                        YY::Debug::log("%s\n\n", message.params["workspaceFolders"].dump(4).c_str());

                        result = {
                            {"capabilities", {{"completionProvider", json::object()}, {"textDocumentSync", {{"openClose", true}, {"change", 1}}}}},
                            {"serverInfo", {{"name", "yy-lsp"}, {"version", "1.0.0"}}}};
                        no_shut = true;

                        if (params.clientInfo && params.clientInfo->version)
                        {
                            YY::Debug::log("Connected from %s %s\n\n", params.clientInfo->name.c_str(), params.clientInfo->version->c_str());
                        }
                    }
                    else if (message.method == "exit")
                    {
                        running = false;
                    }
                }

                if (auto &id = message.id)
                {
                    YY::RPC::respond(*id, result);
                }
            }
        };
    }
}

#endif