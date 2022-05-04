#pragma once

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

#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) 
        : search_server_(search_server){};

    void QueryUpdate(vector<Document> find_result);
    
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate);

    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);

    vector<Document> AddFindRequest(const string& raw_query);

    int GetNoResultRequests() const;

private:
    deque<bool> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
};



template <typename DocumentPredicate>
vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
    vector<Document> find_result = search_server_.FindTopDocuments(raw_query, document_predicate);
    RequestQueue::QueryUpdate(find_result);
    return find_result;
}

