#include "rpc.hpp"

namespace YY
{
    namespace RPC
    {
        std::string msg_buf = "";
        YY::Constants::TransportKind transport = YY::Constants::TransportKind::STDIO;

        void set_transport(YY::Constants::TransportKind out)
        {
            transport = out;
        }

        const char *inside_of(const char *ptr, const char *data_start, const char *data_end, int displacement = 0)
        {
            if (ptr == nullptr || ptr + displacement < data_start || ptr + displacement >= data_end)
            {
                return nullptr;
            }
            return ptr + displacement;
        }

        const char *consume_trailing(const char *data)
        {
            if (data == nullptr)
            {
                return nullptr;
            }
            while (data[0] == ' ')
            {
                data++;
            }
            return data;
        }

        int64_t bytes_to_int(const char *data)
        {
            if (data == nullptr)
            {
                return 0;
            }

            return std::atol(data);
        }

        std::string encode(json &data)
        {
            std::string out = data.dump();

            return std::format("Content-Length: {}\r\n\r\n{}", std::to_string(out.length()), out);
        }

        std::optional<YY::LSP::Message> decode(std::string &str_buf)
        {
            size_t separator_pos = str_buf.find("\r\n\r\n");

            if (separator_pos == std::string::npos)
            {
                return std::nullopt;
            }

            std::string headers = str_buf.substr(0, separator_pos);
            std::string content = str_buf.substr(separator_pos + 4);

            int content_length = bytes_to_int(
                consume_trailing(
                    inside_of(std::strstr(headers.data(), "Content-Length:"), headers.data(), headers.data() + headers.length(), sizeof("Content-Length:"))));

            if (content_length == 0 || content.length() < content_length)
            {
                return std::nullopt;
            }

            // Consume message even if has an invalid body
            // -> At this point it's for sure a complete RPC message
            str_buf = str_buf.substr(separator_pos + 4 + content_length);

            /*
            IDEA: [struct-enum, function pre-pos]
                Guarantees can be defined here for more practical usage & type safety
                Also you shoud respect them, avoiding invalid states.
            */

            json payload;

            try
            {
                payload = json::parse(content);
            }
            catch (const std::exception &e)
            {
                YY::Debug::log("[RPC in ERR]: %s\n", e.what());

                return std::nullopt;
            }

            YY::LSP::Message msg;

            if (payload.contains("id") && payload["id"].is_number_integer())
            {
                msg.id = payload["id"];
            }

            if (payload.contains("method") && payload["method"].is_string())
            {
                msg.method = payload["method"];
            }

            if (payload.contains("jsonrpc") && payload["jsonrpc"].is_string())
            {
                msg.jsonrpc = payload["jsonrpc"];
            }

            if (payload.contains("params") && payload["params"].is_object())
            {
                msg.params = payload["params"];
            }

            return msg;
        }

        YY::LSP::Message get()
        {
            YY::LSP::Message message;

            if (transport == YY::Constants::TransportKind::STDIO)
            {
                bool found = false;

                char c;

                while (!found)
                {
                    if (std::cin.peek() == EOF)
                    {
                        std::cin.clear();
                    }
                    else
                    {
                        std::cin.get(c);
                        msg_buf += c;
                        if (auto msg = decode(msg_buf))
                        {
                            found = true;
                            message = *msg;
                        }
                        continue;
                    }

                    if (auto msg = decode(msg_buf))
                    {
                        found = true;
                        message = *msg;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            return message;
        }

        void write(json &data)
        {
            std::string blob = encode(data);

            switch (transport)
            {
            case YY::Constants::TransportKind::STDIO:
            {
                std::cout << blob;
            }
            break;
            default:
                break;
            }

            YY::Debug::log("[RPC out]: %s\n", data.dump().c_str());
        }

        void respond(int request_id, json &result)
        {
            json p = {
                {"id", request_id},
                {"result", result}
            };

            write(p);
        }
    }
}