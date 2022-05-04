
#include "remove_duplicates.h"

// template <typename Type> 
// void PrintIter(string str, const set<Type>& list) {
//     if (list.empty()) {
//         cout << str << ": Empty" << endl;
//     } else {
//         cout << str << "_"<< list.size() << ": ";
//         for (auto& v:list) {
//             cout << v << " ";
//         }; cout << endl;
//     }
// }

// void RemoveDuplicates(SearchServer& search_server) {
//     vector <int> remote_id;
//     set <set<string>> words;

//     for (const int document_id : search_server) {
//         map<string, double> word_freq;
//         set<string> set_words;
//         word_freq = search_server.GetWordFrequencies(document_id);
//         for (const auto& [word, freq] : word_freq) {
//             set_words.insert(word);
//         }

//         if (words.count(set_words) > 0) {
//             remote_id.push_back(document_id);
//             cout << "Found duplicate document id "s << document_id << endl;     
//         } else {
//             words.insert(set_words);
//         }
//     }

//     for (auto& id:remote_id) {
//         search_server.RemoveDocument(id);
//     }
// }