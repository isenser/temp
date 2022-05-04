#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <list>

#include "document.h"
#include "string_processing.h"

#include <functional>
#include <numeric>
#include <execution>

/*
Подсказка для начинающих её выполнять: 
1) у вас должно быть два контейнера 
для стрингов стоп-слов и слов документов, 
например 
const std::set<std::string, std::less<>> stop_words_;
 и 
 std::set<std::string, std::less<>> words_;; 
 2) у вас должна быть новая функция SplitIntoWords, 
подходящая для вьюх; 
3) а далее везде в SearchServer меняем 
стринги на вьюхи. 
Всё очень просто (казалось бы)))) (отредактировано) 
*/

const int MAX_RESULT_DOCUMENT_COUNT = 5;

class SearchServer {
public:
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  // Extract non-empty stop words
    {
        if (!std::all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            throw invalid_argument("Some of stop words are invalid"s);
        }
    }

    explicit SearchServer(const string& stop_words_text)
        : SearchServer(SplitIntoWords(stop_words_text)) 
    {    
       
    }

    // explicit SearchServer(const string_view& stop_words_text)
    //     : SearchServer(SplitIntoWords_SV(stop_words_text)) 
    // {   
       
    // }


    std::set<int>::const_iterator begin() const;
    std::set<int>::const_iterator end()   const;

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings);

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const;
    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const;
    vector<Document> FindTopDocuments(const string& raw_query) const;

    int GetDocumentCount() const;

    void RemoveDocument(int document_id);

    void RemoveDocument(const std::execution::sequenced_policy&, int document_id);

    void RemoveDocument(const std::execution::parallel_policy&, int document_id);

    const map<string_view, double>& GetWordFrequencies(int document_id) const;

    tuple<vector<string_view>, DocumentStatus> MatchDocument(
        const string_view& raw_query, int document_id) const; 

    tuple<vector<string_view>, DocumentStatus> MatchDocument(
        const std::execution::sequenced_policy&,\
        const string_view& raw_query, int document_id) const; 

    tuple<vector<string_view>, DocumentStatus> MatchDocument(
        const std::execution::parallel_policy&,\
        const string_view& raw_query, int document_id) const; 

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    

    const set<string, std::less<>> stop_words_;
    //const set<string_view> stop_words_;
    vector<string> added_documents_;

    map<string_view, map<int, double>> word_to_document_freqs_;

    map<int, map<string_view, double>>  document_to_word_freqs_;

    map<int, DocumentData> documents_;
    
    set<int> document_ids_; 

    struct QueryWord {
        string_view data;
        bool is_minus;
        bool is_stop;
    };

    struct Query {
        set<string_view> plus_words;
        set<string_view> minus_words;
    };

    struct Query_Vect {
        vector<string_view> plus_words;
        vector<string_view> minus_words;
    };

    bool IsStopWord(const string_view& word) const;

    static bool IsValidWord(const string_view& word);

    vector<string_view> SplitIntoWordsNoStop(const string_view& text) const;

    static int ComputeAverageRating(const vector<int>& ratings);

    QueryWord ParseQueryWord(const string_view& text) const;
    
    Query ParseQuery(const string_view& text) const;
    Query_Vect ParseQuery(const std::execution::parallel_policy&, const std::string_view& text) const;
    // Existence required
    double ComputeWordInverseDocumentFreq(const string& word) const;

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;
};


//---------------------------------------------------------
template <typename DocumentPredicate>
vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
    const auto query = ParseQuery(raw_query);

    auto matched_documents = FindAllDocuments(query, document_predicate);

    sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
        if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
            return lhs.rating > rhs.rating;
        } else {
            return lhs.relevance > rhs.relevance;
        }
    });
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    return matched_documents;
}

template <typename DocumentPredicate>
vector<Document> SearchServer::FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
    map<int, double> document_to_relevance;
    for (const string_view& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        //const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        const double inverse_document_freq = ComputeWordInverseDocumentFreq({word.data(), word.size()});
        for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }

    for (const string_view& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
    }
    return matched_documents;
}
