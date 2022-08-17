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

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate); 

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;
private:
    struct QueryResult {
        std::vector<Document> results;
        bool EmptyResult;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int num_of_empty_res;
    const SearchServer& search_server_;
}; 

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        QueryResult qr;
        if(requests_.size()==min_in_day_)
        {
            requests_.pop_front();
            if(requests_.front().EmptyResult==true)
            {
                num_of_empty_res--;
            }
        }
        std::vector<Document> res = search_server_.FindTopDocuments(raw_query, document_predicate);
        if(res.empty())
        {
            qr.results=res;
            qr.EmptyResult=true;
            num_of_empty_res++;
        }
        else
        {
            qr.results=res;
            qr.EmptyResult=false;
        }
        requests_.push_back(qr);
        return res;
    }
