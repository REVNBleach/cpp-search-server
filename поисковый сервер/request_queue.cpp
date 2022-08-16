#include "request_queue.h"
#include "read_input_functions.h"
#include "document.h"
#include "search_server.h"
using namespace std;

    RequestQueue::RequestQueue(const SearchServer& search_server):search_server_(search_server) {
        num_of_empty_res=0;
    }

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
        QueryResult qr;
        if(requests_.size()==min_in_day_)
        {
            requests_.pop_front();
            if(requests_.front().EmptyResult==true)
            {
                num_of_empty_res--;
            }
        }
        vector<Document> res = search_server_.FindTopDocuments(raw_query, status);
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

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
        QueryResult qr;
        if(requests_.size()==min_in_day_)
        {
            requests_.pop_front();
            if(requests_.front().EmptyResult==true)
            {
                num_of_empty_res--;
            }
        }
        vector<Document> res = search_server_.FindTopDocuments(raw_query);
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

    int RequestQueue::GetNoResultRequests() const {
        return num_of_empty_res;
    }
