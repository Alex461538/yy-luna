#include "text_document.hpp"

namespace YY
{
    namespace Fs
    {
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

        void TextDocument::setContent(std::string new_content)
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
        void TextDocument::patch(std::string new_content, Range range)
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

        TextDocument::TextDocument() {}

        TextDocument::TextDocument(std::string &_uri, std::string &_lang, std::string &_text)
        {
            uri = _uri;
            languageId = _lang;
            setContent(_text);
        }
    }
}