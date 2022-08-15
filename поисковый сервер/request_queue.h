#pragma once

#include "read_input_functions.h"
#include "document.h"
#include "search_server.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <deque>

using namespace std;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate);

    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);

    vector<Document> AddFindRequest(const string& raw_query);

    int GetNoResultRequests() const;
private:
    struct QueryResult {
        vector<Document> results;
        bool EmptyResult;
    };
    deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int num_of_empty_res;
    const SearchServer& search_server_;
}; 
