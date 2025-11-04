#ifndef __hpp_yy_lsp_message__
#define __hpp_yy_lsp_message__

//  --- Global includes ---
#include <optional>
#include <string>
//  --- Project includes ---
#include <json.hpp>
//  --- Local includes ---
#include "../fs/text_document.hpp"

using json = nlohmann::json;

namespace YY
{
    namespace LSP
    {
        struct URI
        {
            std::string text;
        };

        struct WorkspaceFolder
        {
            URI uri;
            std::string name;
        };

        void from_json(const json &j, URI &p)
        {
            if (j.is_string())
            {
                p.text = j.get<std::string>();
            }
        }

        void from_json(const json &j, WorkspaceFolder &p)
        {
            if (j.is_object())
            {
                if (j.contains("name") && j["name"].is_string())
                {
                    p.name = j["name"].get<std::string>();
                }
                if (j.contains("uri") && j["uri"].is_string())
                {
                    p.uri = j["version"].get<URI>();
                }
            }
        }

        // Inheritance is broken bro...
        namespace Params
        {
            struct InitializeParams
            {
                struct ClientInfo
                {
                    std::string name;
                    std::optional<std::string> version;
                };

                std::optional<ClientInfo> clientInfo;
                std::optional<int> processId;

                std::optional<std::string> locale;

                std::optional<std::string> rootPath;
                std::optional<std::string> rootUri;
                std::string trace;

                std::optional<json> initializationOptions;

                json capabilities;

                std::optional<std::vector<WorkspaceFolder>> workspaceFolders;
            };

            struct DidCloseTextDocumentParams
            {
                URI textDocument;
            };

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

            /* void to_json(json &j, const InitializeParams &p)
            {
                //j = json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
            }

            void from_json(const json &j, InitializeParams &p)
            {
                //j.at("name").get_to(p.name);
            } */
        }

        namespace Results
        {
            struct InitializeResult
            {
                struct ServerInfo
                {
                    std::string name;
                    std::optional<std::string> version;
                };

                std::optional<ServerInfo> serverInfo;
                json capabilities;
            };
        }

        struct Message
        {
            std::optional<int> id;
            std::string jsonrpc;
            std::string method;
            json params;

            operator std::string() const
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
        };
    }
}

#endif