#if !defined(__hpp_yy_cli__)
#define __hpp_yy_cli__

//  --- Global includes ---
#include <map>
#include <variant>
//  --- Project includes ---
#include <argh.h>
#include <json.hpp>
//  --- Local includes ---
#include "constants.hpp"
#include "lsp/server.hpp"

namespace YY
{
    namespace CLI
    {
        struct Command
        {
            enum class Action
            {
                IDLE,
                SHOW_VERSION,
                BUILD,
                RPC_CONNECT
            } action = Action::IDLE;

            struct RPC_ConnectOptions
            {
                YY::Constants::TransportKind transportKind = YY::Constants::TransportKind::STDIO;
            };

            struct BuildOptions
            {
                YY::Constants::OsAPI osapi = YY::Constants::OsAPI::NONE;
                YY::Constants::ISA isa = YY::Constants::ISA::NONE;
                YY::Constants::Packaging packaging = YY::Constants::Packaging::NONE;

                std::filesystem::path source;
            };

            std::variant<BuildOptions, RPC_ConnectOptions, int> parameters = 0;

            void set_action(std::string name)
            {
                std::map<std::string, Command::Action> mappings = {
                    {"build", Command::Action::BUILD},
                    {"version", Command::Action::SHOW_VERSION},
                    {"rpc-connect", Command::Action::RPC_CONNECT},

                    {"b", Command::Action::BUILD},
                    {"v", Command::Action::SHOW_VERSION},
                    {"rpcc", Command::Action::RPC_CONNECT},
                };

                auto value = mappings.find(name);
                if (value != mappings.end())
                {
                    action = value->second;
                }
                else
                {
                    action = Command::Action::IDLE;
                }

                switch (action)
                {
                case Command::Action::BUILD:
                {
                    parameters = BuildOptions();
                }
                break;
                case Command::Action::RPC_CONNECT:
                {
                    parameters = RPC_ConnectOptions();
                }
                break;
                default:
                    break;
                }
            }

            void set_rpc_transport_kind(std::string name)
            {
                auto &params = std::get<RPC_ConnectOptions>(parameters);

                std::map<std::string, YY::Constants::TransportKind> mappings = {
                    {"stdio", YY::Constants::TransportKind::STDIO},
                };

                auto value = mappings.find(name);
                if (value != mappings.end())
                {
                    params.transportKind = value->second;
                }
                else
                {
                    params.transportKind = YY::Constants::TransportKind::STDIO;
                }
            }

            void set_build_isa(std::string name)
            {
                auto &params = std::get<BuildOptions>(parameters);

                std::map<std::string, YY::Constants::ISA> mappings = {
                    {"x86-32", YY::Constants::ISA::X86_32},
                    {"x86-64", YY::Constants::ISA::X86_64},
                };

                auto value = mappings.find(name);
                if (value != mappings.end())
                {
                    params.isa = value->second;
                }
                else
                {
                    params.isa = YY::Constants::ISA::NONE;
                }
            }

            void set_build_pkg(std::string name)
            {
                auto &params = std::get<BuildOptions>(parameters);

                std::map<std::string, YY::Constants::Packaging> mappings = {
                    {"o", YY::Constants::Packaging::OBJ},
                    {"exe", YY::Constants::Packaging::EXE},
                    {"elf", YY::Constants::Packaging::ELF}};

                auto value = mappings.find(name);
                if (value != mappings.end())
                {
                    params.packaging = value->second;
                }
                else
                {
                    params.packaging = YY::Constants::Packaging::NONE;
                }
            }

            void set_build_osapi(std::string name)
            {
                auto &params = std::get<BuildOptions>(parameters);

                std::map<std::string, YY::Constants::OsAPI> mappings = {
                    {"linux", YY::Constants::OsAPI::LINUX},
                    {"windows", YY::Constants::OsAPI::WINDOWS}};

                auto value = mappings.find(name);
                if (value != mappings.end())
                {
                    params.osapi = value->second;
                }
                else
                {
                    params.osapi = YY::Constants::OsAPI::NONE;
                }
            }

            void parse(const char **args, int arg_count)
            {
                argh::parser cmdl;

                cmdl.add_params(
                    {"--transport",
                     "--isa", "--osapi", "--pkg"});

                cmdl.parse(arg_count, args);

                set_action(cmdl[1]);

                switch (action)
                {
                case Command::Action::BUILD:
                {
                    set_build_isa(cmdl("--isa", "").str());
                    set_build_pkg(cmdl("--pkg", "").str());
                    set_build_osapi(cmdl("--osapi", "").str());
                }
                break;
                case Command::Action::RPC_CONNECT:
                {
                    set_rpc_transport_kind(cmdl("--transport", "").str());
                }
                break;
                default:
                {
                }
                break;
                }
            }

            void execute()
            {
                switch (action)
                {
                case Command::Action::SHOW_VERSION:
                {
                    YY::Out::log("%sVersion %s | On %s %s\n", YY::Constants::CLI_BANNER, YY::Constants::VERSION, YY::Constants::OSAPI_NAME, YY::Constants::ISA_NAME);
                }
                break;
                case Command::Action::BUILD:
                {
                    auto &params = std::get<BuildOptions>(parameters);

                    /* 
                    I'm thinking about this later, but
                    it will be good to just recycle the server without the RPC complexity, just direct function execution.
                    */
                }
                break;
                case Command::Action::RPC_CONNECT:
                {
                    // Since this point, we need to keep stdio clean from any logging
                    auto &params = std::get<RPC_ConnectOptions>(parameters);
                    // Redirect logs to logfile
                    YY::Out::set_out(YY::Constants::TransportKind::LOG_FILE);
                    // Map RPC to the specified place (stdio)
                    YY::RPC::set_transport(params.transportKind);

                    YY::LSP::Server server;
                    // Awake server
                    server.run();
                    // Consume messages until done
                    while (server.is_running())
                    {
                        YY::RPC::Message msg = server.peek();
                        server.process(msg);
                    }
                    // Debug for catching stuck servers
                    YY::Debug::log("[RPC]: server excited normally...\n");
                }
                break;
                default:
                {
                }
                break;
                }
            }
        };
    }
}

#endif