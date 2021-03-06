// search_server_s4_t2_solution.cpp
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <deque>

using namespace std;
//using namespace std::string_literals;

#include "read_input_functions.h"
#include "document.h"
#include "paginator.h"
#include "string_processing.h"
#include "search_server.h"
#include "request_queue.h"

#include "test_example_functions.h"
//#include "remove_duplicates.h"

#include "process_queries.h"

using namespace std;


int main() {
    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }

    const vector<string> queries = {
        "nasty rat -not"s,
        "not very funny nasty pet"s,
        "curly hair"s
    };
    for (const Document& document : ProcessQueriesJoined(search_server, queries)) {
        cout << "Document "s << document.id << " matched with relevance "s << document.relevance << endl;
    }

    return 0;
}

/*
Document 1 matched with relevance 0.183492
Document 5 matched with relevance 0.183492
Document 4 matched with relevance 0.167358
Document 3 matched with relevance 0.743945
Document 1 matched with relevance 0.311199
Document 2 matched with relevance 0.183492
Document 5 matched with relevance 0.127706
Document 4 matched with relevance 0.0557859
Document 2 matched with relevance 0.458145
Document 5 matched with relevance 0.458145
*/

// #include <algorithm>
// #include <cmath>
// #include <iostream>
// #include <map>
// #include <set>
// #include <stdexcept>
// #include <string>
// #include <utility>
// #include <vector>
// #include <deque>

// #include <functional>
// #include <numeric>
// #include <execution>

// //using namespace std::string_literals;

// #include "read_input_functions.h"
// #include "document.h"
// #include "paginator.h"
// #include "string_processing.h"
// #include "search_server.h"
// #include "request_queue.h"
// #include "remove_duplicates.h"
// #include "process_queries.h"

// #include "test_example_functions.h"

// using namespace std;

// int T() {

//     SearchServer search_server("and with"s);

//     int id = 0;
//     for (
//         const string& text : {
//             "funny pet and nasty rat"s,
//             "funny pet with curly hair"s,
//             "funny pet and not very nasty rat"s,
//             "pet with rat and rat and rat"s,
//             "nasty rat with curly hair"s,
//         }
//     ) {
//         search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
//     }

//     const vector<string> queries = {
//         "nasty rat -not"s,
//         "not very funny nasty pet"s,
//         "curly hair"s
//     };
//     for (const Document& document : ProcessQueriesJoined(search_server, queries)) {
//         cout << "Document "s << document.id << " matched with relevance "s << document.relevance << endl;
//     }

//     return 0;
// }

// /*
// Document 1 matched with relevance 0.183492
// Document 5 matched with relevance 0.183492
// Document 4 matched with relevance 0.167358
// Document 3 matched with relevance 0.743945
// Document 1 matched with relevance 0.311199
// Document 2 matched with relevance 0.183492
// Document 5 matched with relevance 0.127706
// Document 4 matched with relevance 0.0557859
// Document 2 matched with relevance 0.458145
// Document 5 matched with relevance 0.458145
// */

// int Test() {
//     SearchServer ss("and with"s);

//     int id = 0;
//     for (
//         const string& text : {
//             "funny pet and nasty rat"s,
//             "funny pet with curly hair"s,
//             "funny pet and not very nasty rat"s,
//             "pet with rat and rat and rat"s,
//             "nasty rat with curly hair"s,
//         }
//     ) {
//         ss.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
//     }

//     const string query = "curly and funny -not"s;

//     {
//         const auto [words, status] = ss.MatchDocument(query, 1);
//         cout << words.size() << " words for document 1"s << endl;
//         // 1 words for document 1
//     }

//     {
//         const auto [words, status] = ss.MatchDocument(execution::seq, query, 2);
//         cout << words.size() << " words for document 2"s << endl;
//         // 2 words for document 2
//     }

//     {
//         const auto [words, status] = ss.MatchDocument(execution::par, query, 3);
//         cout << words.size() << " words for document 3"s << endl;
//         // 0 words for document 3
//     }

//     return 0;
// }

// // /*
// // 1 words for document 1
// // 2 words for document 2
// // 0 words for document 3
// // */

// #include "search_server.h"

// #include <execution>
// #include <iostream>
// #include <random>
// #include <string>
// #include <vector>

// #include "log_duration.h"

// using namespace std;

// string GenerateWord(mt19937& generator, int max_length) {
//     const int length = uniform_int_distribution(1, max_length)(generator);
//     string word;
//     word.reserve(length);
//     for (int i = 0; i < length; ++i) {
//         word.push_back(uniform_int_distribution('a', 'z')(generator));
//     }
//     return word;
// }

// vector<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
//     vector<string> words;
//     words.reserve(word_count);
//     for (int i = 0; i < word_count; ++i) {
//         words.push_back(GenerateWord(generator, max_length));
//     }
//     sort(words.begin(), words.end());
//     words.erase(unique(words.begin(), words.end()), words.end());
//     return words;
// }

// string GenerateQuery(mt19937& generator, const vector<string>& dictionary, int word_count, double minus_prob = 0) {
//     string query;
//     for (int i = 0; i < word_count; ++i) {
//         if (!query.empty()) {
//             query.push_back(' ');
//         }
//         if (uniform_real_distribution<>(0, 1)(generator) < minus_prob) {
//             query.push_back('-');
//         }
//         query += dictionary[uniform_int_distribution<int>(0, dictionary.size() - 1)(generator)];
//     }
//     return query;
// }

// vector<string> GenerateQueries(mt19937& generator, const vector<string>& dictionary, int query_count, int max_word_count) {
//     vector<string> queries;
//     queries.reserve(query_count);
//     for (int i = 0; i < query_count; ++i) {
//         queries.push_back(GenerateQuery(generator, dictionary, max_word_count));
//     }
//     return queries;
// }

// template <typename ExecutionPolicy>
// void Test(string_view mark, SearchServer search_server, const string& query, ExecutionPolicy&& policy) {
//     //LOG_DURATION(mark);
//     auto ld = LogDuration(mark);
//     const int document_count = search_server.GetDocumentCount();
//     int word_count = 0;
//     for (int id = 0; id < document_count; ++id) {
//         const auto [words, status] = search_server.MatchDocument(policy, query, id);
//         word_count += words.size();
//     }
//     //cout << ld.GetTimeNow();
//     cout << word_count << endl;
// }

// #define TEST(policy) Test(#policy, search_server, query, execution::policy)

// int main() {

//     {
//         T();
//     }

//     {
//         Test();
//     }

    
    
//     mt19937 generator;

//     const auto dictionary = GenerateDictionary(generator, 1000, 10);
//     //const auto documents = GenerateQueries(generator, dictionary, 10'000, 70);
//     const auto documents = GenerateQueries(generator, dictionary, 5'000, 70);

//     const string query = GenerateQuery(generator, dictionary, 500, 0.1);

//     SearchServer search_server(dictionary[0]);
//     for (size_t i = 0; i < documents.size(); ++i) {
//         search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, {1, 2, 3});
//     }

//     TEST(seq);
//     TEST(par);
// }