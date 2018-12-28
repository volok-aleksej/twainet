#ifndef AUTO_COMPLETE_HELPER_H
#define AUTO_COMPLETE_HELPER_H

#include <string>
#include <vector>
#include <string.h>

class AutoCompleteHelper
{
public:
    std::vector<std::string> autoCompleteHelper(const std::string& word, const std::vector<std::string>& words)
    {
        std::vector<std::string> usewords;
        for(auto word_ : words) {
            if(word.size() <= word_.size() && memcmp(word.data(), word_.data(), word.size()) == 0) {
                usewords.push_back(word_);
            }
        }

        return usewords;
    }
};

#endif/*AUTO_COMPLETE_HELPER_H*/
