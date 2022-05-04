#include "request_queue.h"

void RequestQueue::QueryUpdate(vector<Document> find_result) {
    if (find_result.size() == 0) {
        requests_.push_back(false);
    } else {
        requests_.push_back(true);
    }
    while (requests_.size() > min_in_day_) {
        requests_.pop_front();
    }
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    vector<Document> find_result = search_server_.FindTopDocuments(raw_query, status);
    QueryUpdate(find_result);
    return find_result;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    vector<Document> find_result = search_server_.FindTopDocuments(raw_query);
    QueryUpdate(find_result);
    return find_result;
}

int RequestQueue::GetNoResultRequests() const {
    if (!requests_.empty()) {
        return count(requests_.begin(),requests_.end(),false);
    }
    return 0;   
}