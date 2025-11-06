#ifndef __hpp_yy_lsp_message__
#define __hpp_yy_lsp_message__

//  --- Global includes ---
#include <optional>
#include <string>
//  --- Project includes ---
#include <json.hpp>
//  --- Local includes ---
#include "../out.hpp"

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

        struct TextDocumentItem {
            URI	uri;
            std::string languageId;
            std::string text;
            int version;
        };

        void from_json(const json &j, URI &p);
        void from_json(const json &j, WorkspaceFolder &p);
        void from_json(const json &j, TextDocumentItem &p);
        
        void to_json(json &j, const URI &p);
        void to_json(json &j, const WorkspaceFolder &p);

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

            struct DidOpenTextDocumentParams {
                TextDocumentItem textDocument;
            };

            void from_json(const json &j, DidOpenTextDocumentParams &p);
            void from_json(const json &j, DidCloseTextDocumentParams &p);
            void from_json(const json &j, InitializeParams &p);
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

            operator std::string() const;
        };
    }
}

#endif