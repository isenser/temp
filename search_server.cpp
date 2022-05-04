//#pragma once
#include "search_server.h"

std::set<int>::const_iterator SearchServer::begin() const{
    return document_ids_.begin();

}

std::set<int>::const_iterator SearchServer::end() const{
    return document_ids_.end();

}

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw invalid_argument("AddDocument: Invalid document_id"s);
    }

    added_documents_.push_back(document);

    const vector<string_view> words_in_doc = SplitIntoWordsNoStop(document);

    const double inv_word_count = 1.0 / words_in_doc.size();
    for (const string_view& word : words_in_doc) {
        //map<string, map<int, double>> word_to_document_freqs_;
        word_to_document_freqs_[word][document_id] += inv_word_count;

        //map<int, map<string, double>  document_to_word_freqs_
        document_to_word_freqs_[document_id][word] += inv_word_count;
    }

    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    document_ids_.insert(document_id);
}

void SearchServer::RemoveDocument(int document_id) {
    if ((document_id > 0) || (documents_.count(document_id) != 0)) {
        
        vector<string_view> vec_words;
        for (auto& [key, value] : word_to_document_freqs_) {
            value.erase(document_id);
            if (value.empty()) {
                vec_words.push_back(key);
            }
        }

        for (auto& word : vec_words) {
            word_to_document_freqs_.erase(word);
        }

        document_to_word_freqs_.erase(document_id);
        documents_.erase(document_id);
    }
}//void SearchServer::RemoveDocument(int document_id) {

void SearchServer::RemoveDocument(const std::execution::sequenced_policy&, int document_id) {
    RemoveDocument(document_id);
}

//parallel
void SearchServer::RemoveDocument(const std::execution::parallel_policy&, int document_id){
    if ((document_id > 0) || (documents_.count(document_id) != 0)) {
        
        vector<const string_view*> vec_words;

        //map<int, map<string, double>>  document_to_word_freqs_;

        for(const auto& [word, freqs] : document_to_word_freqs_.at(document_id)) {
            //vec_words.push_back(*(&word));
            vec_words.push_back((&word));
            //cout << *&word << endl;
        }

        std::for_each(
            std::execution::par,
            vec_words.begin(), vec_words.end(),
            [&](const auto &word_id_freqs) {
              word_to_document_freqs_.at(*word_id_freqs).erase(document_id); 
            }
        );

        document_to_word_freqs_.erase(document_id);
        documents_.erase(document_id);
        document_ids_.erase(document_id);
    } else 
    return;
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
    });
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

const map<string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static map<string_view, double> result;
    result.clear(); //обнуление глобальное переменной
    result = document_to_word_freqs_.at(document_id);
    return result;
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const string_view& raw_query, int document_id) const {
	const auto query = ParseQuery(raw_query);

	vector<string_view> matched_words;
	for (const string_view& word : query.plus_words) {
	    if (word_to_document_freqs_.count(word) == 0) {
	        continue;
	    }
	    if (word_to_document_freqs_.at(word).count(document_id)) {
	        matched_words.push_back(word);
	    }
	}
	for (const string_view& word : query.minus_words) {
	    if (word_to_document_freqs_.count(word) == 0) {
	        continue;
	    }
	    if (word_to_document_freqs_.at(word).count(document_id)) {
	        matched_words.clear();
	        break;
	    }
	}
	return {matched_words, documents_.at(document_id).status};
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(
    const std::execution::sequenced_policy&,\
    const string_view& raw_query, int document_id) const {
    return MatchDocument(raw_query,document_id);
}

//PARALLEL
tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(
    const std::execution::parallel_policy&,\
    const string_view& raw_query, int document_id) const {
    
    const Query_Vect& query = ParseQuery(std::execution::par, raw_query);

    std::vector<std::string_view> matched_words;
    if(std::any_of(query.minus_words.begin(), query.minus_words.end(), [this, document_id](const auto &val)
    {
        return word_to_document_freqs_.find(val) != word_to_document_freqs_.end() and word_to_document_freqs_.at(val).count(document_id) ;
    }))
    {
      return {matched_words, documents_.at(document_id).status};
    }
   matched_words.reserve(query.plus_words.size());
   std::copy_if(query.plus_words.begin(),query.plus_words.end(),std::back_inserter(matched_words),[this, document_id](auto &val)
   {
       return word_to_document_freqs_.find(val) != word_to_document_freqs_.end() and word_to_document_freqs_.at(val).count(document_id);
   });
 
   sort(matched_words.begin(),matched_words.end());
   auto f = std::unique(matched_words.begin(),matched_words.end());
   matched_words.erase(f, matched_words.end());
 
   return {matched_words,documents_.at(document_id).status };

}

//---------- PRIVATE --------------
bool SearchServer::IsStopWord(const string_view& word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const string_view& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

// vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
//     vector<string> words;
//     for (const string& word : SplitIntoWords(text)) {
//         if (!IsValidWord(word)) {
//             throw invalid_argument("Word "s + word + " is invalid"s);
//         }
//         if (!IsStopWord(word)) {
//             words.push_back(word);
//         }
//     }
//     return words;
// }

vector<string_view> SearchServer::SplitIntoWordsNoStop(const string_view& text) const {
    vector<string_view> words;
    for (const string_view& word : SplitIntoWords_SV(text)) {
        if (!IsValidWord(word)) {
            //throw invalid_argument("Word "sv + word + " is invalid"sv);
            throw invalid_argument("Word is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(const string_view& text) const {
    if (text.empty()) {
        throw invalid_argument("Query word is empty"s);
    }
    string_view word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
        //throw invalid_argument("Query word "s + text + " is invalid");
        throw invalid_argument("Query word is invalid");
    }

    return {word, is_minus, IsStopWord(word)};
}

SearchServer::Query SearchServer::ParseQuery(const string_view& text) const {
    SearchServer::Query result;
    for (const string_view& word : SplitIntoWords_SV(text)) {
        const auto query_word = SearchServer::ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.insert(query_word.data);
            } else {
                result.plus_words.insert(query_word.data);
            }
        }
    }
    return result;
}

// --- PARALLEL ---
SearchServer::Query_Vect SearchServer::ParseQuery(const std::execution::parallel_policy&, const std::string_view& text) const
{
    SearchServer::Query_Vect result;
    for (const string_view& word : SplitIntoWords_SV(text)) {
        const auto query_word = SearchServer::ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            } else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
    return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}