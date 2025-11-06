#include "server.hpp"

namespace YY
{
    namespace LSP
    {
        Server::Server()
        {
            running = false;
            no_shut = false;
        }

        bool Server::is_running()
        {
            return running;
        }

        void Server::run()
        {
            YY::Debug::log("🐥 Protect chocobos worldwide!\n\n");
            running = true;
        }

        YY::LSP::Message Server::peek()
        {
            return YY::RPC::get();
        }

        void Server::process(YY::LSP::Message &message)
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
                    auto params = message.params.get<YY::LSP::Params::DidOpenTextDocumentParams>();
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

                    if (auto &workspaceFolders = params.workspaceFolders)
                    {
                        for (auto &folder : *workspaceFolders)
                        {
                            scramble.addPackageFolder(folder);
                        }
                    }

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
    }
}