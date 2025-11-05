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

        int readUTF8(const char **text);

        class TextDocument
        {
        public:
            /*
                TODO: Use more efficient data structures later
            */
            int version;
            std::string languageId;
            std::string uri;
            std::optional<std::string> path;
            // take the easy path
            // Easy line access
            // Are these copied on insert-delete??? I hope this moves D: Idk
            std::vector<std::unique_ptr<std::string>> text;

            void setContent(std::string new_content);
            void patch(std::string new_content, Range range);

            TextDocument();
            TextDocument(std::string &_uri, std::string &_lang, std::string &_text);
        };
    }
}

#endif