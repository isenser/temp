
#include "string_processing.h"

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

//@input str = stop_words_text
//@ouput vector<string_view>
vector<string_view> SplitIntoWords_SV(string_view str) {
    vector<string_view> result;

    if (str.empty()) return result;

    while (str.size() > 0) {
        //Убираем пробелы вначале
        if (str.find(' ') == 0) {
            int first_space = std::min(str.find_first_not_of(" "), str.size());
            str.remove_prefix(first_space);
        }
        
        //Если нет больше пробелов
        const long unsigned int first_space = str.find(' ');
        if (first_space == str.npos) {
            if (str.empty()) return result;
            result.push_back(str);
            return result;
        } else {
            result.push_back(str.substr(0, first_space));
            str.remove_prefix(first_space);
        }
    }
    return result;
}

// set<string> MakeUniqueNonEmptyStrings(const vector<string>& strings) {
//     set<string> non_empty_strings;
//     for (const string& str : strings) {
//         if (!str.empty()) {
//             non_empty_strings.insert(str);
//         }
//     }
//     return non_empty_strings;
// }

// set<string_view> MakeUniqueNonEmptyStrings(const string_view& strings) {
//     set<string_view> non_empty_strings;
//     for (const string_view& str : strings) {
//         if (!str.empty()) {
//             non_empty_strings.insert(str);
//         }
//     }
//     return non_empty_strings;
// }