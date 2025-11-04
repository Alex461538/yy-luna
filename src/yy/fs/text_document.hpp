#if !defined(__hpp_yy_fs_text_document__)
#define __hpp_yy_fs_text_document__

//  --- Global includes ---
#include <string>
#include <utility>
#include <optional>
#include <memory>
#include <vector>
//  --- Project includes ---
//  --- Local includes ---
#include "../out.hpp"

namespace YY
{
    namespace Fs
    {
        /*
        This should denote final chars?
        */
        struct Position
        {
            int line;
            int character;
        };

        struct Range
        {
            Position start;
            Position end;
        };

        int readUTF8(const char **text)
        {
            char c1 = **text;
            char c2 = *(*text + 1);
            char c3 = *(*text + 2);
            char c4 = *(*text + 3);

            if (c1 >= 0b1111'0000)
            {
                *text += 4;
                return (((uint32_t)c1 & 0b0000'0111) << 21) &&
                       (((uint32_t)c1 & 0b0111'1111) << 14) &&
                       (((uint32_t)c1 & 0b0111'1111) << 7) &&
                       ((uint32_t)c4 & 0b0111'1111);
            }
            else if (c1 >= 0b1110'0000)
            {
                *text += 3;
                return (((uint32_t)c1 & 0b0000'1111) << 14) &&
                       (((uint32_t)c1 & 0b0111'1111) << 7) &&
                       ((uint32_t)c3 & 0b0111'1111);
            }
            else if (c1 >= 0b1100'0000)
            {
                *text += 2;
                return (((uint32_t)c1 & 0b0001'1111) << 7) &&
                       ((uint32_t)c2 & 0b0111'1111);
            }
            else
            {
                *text += 1;
                return c1 & 0b0111'1111;
            }
        }

        class TextDocument
        {
        public:
            /*
                TODO: Use more efficient data structures later
            */
            int version = 0;
            std::string languageId;
            std::string uri;
            std::optional<std::string> path;
            // take the easy path
            // Easy line access
            // Are these copied on insert-delete??? I hope this moves D: Idk
            std::vector<std::unique_ptr<std::string>> text;

            void setContent(std::string new_content)
            {
                text.clear();

                const char *curr = new_content.data();
                const char *end = new_content.data() + new_content.size();

                const char *lineStart = curr;

                while (curr < end)
                {
                    int32_t letter = readUTF8(&curr);

                    if (letter == '\n' || curr == end)
                    {
                        text.push_back(std::make_unique<std::string>(std::string(lineStart, (size_t)(curr - lineStart))));

                        lineStart = curr;
                    }
                }
            }

            /*
            What if I don't want to support utf-16?
            */
            void patch(std::string new_content, Range range)
            {
                std::vector<std::string> lines;

                const char *curr = new_content.data();
                const char *end = new_content.data() + new_content.size();

                const char *lineStart = curr;

                while (curr < end)
                {
                    int32_t letter = readUTF8(&curr);

                    if (letter == '\n' || curr == end)
                    {
                        lines.push_back(std::string(lineStart, (size_t)(curr - lineStart)));

                        lineStart = curr;
                    }
                }

                /* text.replace(
                    start,
                    length,
                    new_content); */

                // YY::Debug::log("[Line]: %s", text.back()->c_str());
            }

            TextDocument() {}

            TextDocument(std::string &_uri, std::string &_lang, std::string &_text)
            {
                uri = _uri;
                languageId = _lang;
                setContent(_text);
            }
        };

        void from_json(const json &j, TextDocument &p)
        {
            if (
                j.contains("uri") &&
                j.contains("languageId") &&
                j.contains("version") &&
                j.contains("text") &&
                j["uri"].is_string() &&
                j["languageId"].is_string() &&
                j["version"].is_number_integer() &&
                j["text"].is_string())
            {
                p.uri = j["uri"].get<std::string>();
                p.languageId = j["languageId"].get<std::string>();
                p.version = j["version"].get<int>();
                p.setContent(j["text"]);
            }
        }
    }
}

#endif