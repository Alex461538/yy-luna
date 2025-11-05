#include "lsp-message.hpp"

namespace YY
{
    namespace LSP
    {
        void from_json(const json &j, URI &p)
        {
            if (j.is_string())
            {
                p.text = j.get<std::string>();
            }
        }

        void to_json(json &j, const URI &p)
        {
            j = p.text;
        }

        void from_json(const json &j, WorkspaceFolder &p)
        {
            if (
                j.is_object() &&
                j.contains("name") &&
                j.contains("uri") &&
                j["name"].is_string() &&
                j["uri"].is_string())
            {
                p.name = j["name"].get<std::string>();
                p.uri = j["uri"].get<URI>();
            }
        }

        void to_json(json &j, const WorkspaceFolder &p)
        {
            j = json{
                {"name", p.name},
                {"uri", p.uri}};
        }

        void from_json(const json &j, TextDocumentItem &p)
        {
            if (
                j.is_object() &&
                j.contains("uri") &&
                j.contains("languageId") &&
                j.contains("text") &&
                j["uri"].is_string() &&
                j["languageId"].is_string() &&
                j["text"].is_string())
            {
                p.uri = j["uri"].get<URI>();
                p.languageId = j["languageId"].get<std::string>();
                p.text = j["text"].get<std::string>();
            }
        }

        // Inheritance is broken bro...
        namespace Params
        {
            void from_json(const json &j, DidOpenTextDocumentParams &p)
            {
                if (j.contains("textDocument") && j["textDocument"].is_object())
                {
                    p.textDocument = j["textDocument"].get<TextDocumentItem>();
                }
            }

            void from_json(const json &j, DidCloseTextDocumentParams &p)
            {
                if (j.contains("textDocument") && j["textDocument"].is_string())
                {
                    p.textDocument = j["textDocument"].get<URI>();
                }
            }

            void from_json(const json &j, InitializeParams &p)
            {
                if (j.is_object())
                {
                    if (j.contains("clientInfo") && j["clientInfo"].is_object())
                    {
                        auto &c = j["clientInfo"];

                        p.clientInfo = InitializeParams::ClientInfo();
                        if (c.contains("name") && c["name"].is_string())
                        {
                            p.clientInfo->name = c["name"].get<std::string>();
                        }
                        if (c.contains("version") && c["version"].is_string())
                        {
                            p.clientInfo->version = c["version"].get<std::string>();
                        }
                    }
                    if (j.contains("processId") && j["processId"].is_number_integer())
                    {
                        p.processId = j["processId"].get<int>();
                    }
                    if (j.contains("locale") && j["locale"].is_string())
                    {
                        p.locale = j["locale"].get<std::string>();
                    }
                    if (j.contains("rootPath") && j["rootPath"].is_string())
                    {
                        p.rootPath = j["rootPath"].get<std::string>();
                    }
                    if (j.contains("rootUri") && j["rootUri"].is_string())
                    {
                        p.rootUri = j["rootUri"].get<std::string>();
                    }
                    if (j.contains("trace") && j["trace"].is_string())
                    {
                        p.trace = j["trace"].get<std::string>();
                    }
                    if (j.contains("workspaceFolders") && j["workspaceFolders"].is_array())
                    {
                        p.workspaceFolders = {};
                        for (auto &wsf : j["workspaceFolders"])
                        {
                            p.workspaceFolders->push_back(wsf.get<WorkspaceFolder>());
                        }
                    }
                    p.initializationOptions = j["initializationOptions"];
                    p.capabilities = j["capabilities"];
                }
            }
        }

        Message::operator std::string() const
        {
            json p = {
                {"params", params},
                {"jsonrpc", jsonrpc},
                {"method", method}};

            if (id)
            {
                p["id"] = *id;
            }

            return p.dump();
        }
    }
}