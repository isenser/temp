#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <set>

using namespace std;

vector<string> SplitIntoWords(const string& text);
//vector<string_view> SplitIntoWords(string_view str);
vector<string_view> SplitIntoWords_SV(string_view str);

//set<string> MakeUniqueNonEmptyStrings(const string& strings);
//set<string_view> MakeUniqueNonEmptyStrings(const string_view& strings);

template <typename StringContainer>
set<string, std::less<>> MakeUniqueNonEmptyStrings(const StringContainer& string_container) {
    set<string, std::less<>> non_empty_strings;
    for (const auto& str : string_container) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

