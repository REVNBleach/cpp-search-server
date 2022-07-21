#include "search_server.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
    }
}

void TestCheckFindingRightDocs() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }    
}

void TestCheckMinusWords() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("-cat -in -the -city"s);
        ASSERT(found_docs.empty());
    }    
}

void TestCheckMatching() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat in the city"s);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
        set<string> expectation = {"cat"s, "in"s, "the"s, "city"s};
        const auto [words, status] = server.MatchDocument("cat in the city"s, document_id);
        set<string> s(words.begin(), words.end());
        ASSERT_EQUAL(s, expectation);
    }    
}

void TestCheckMatchingAndMinusWords() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("-cat in the city"s);
        ASSERT(found_docs.size() == 0);
    }    
}

void TestCheckRelevance() {
    const int doc_id = 1;
    const string content = "cat in the city"s;
    const int doc_id2 = 2;
    const string content2 = "cat in the town"s;
    const int doc_id3 = 3;
    const string content3 = "cat in a town"s;
    const int doc_id4 = 4;
    const string content4 = "cat get a town"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id4, content4, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat in the city"s);
        ASSERT(found_docs[0].id == 1);
        ASSERT(found_docs[1].id == 2);
        ASSERT(found_docs[2].id == 3);
        ASSERT(found_docs[3].id == 4);
    }    
}

void TestCheckRightRating() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat in the city"s);
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.rating == (1 + 2 + 3) / 3);
    }    
}

void TestCheckPredicate() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    const int doc_id2 = 41;
    const string content2 = "cat in the city"s;
    const vector<int> ratings2 = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("cat"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; });
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, 42);
    }    
}

void TestCheckSearchByStatus() {
    const int doc_id = 42;
    const int doc_id2 = 43;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id2, content, DocumentStatus::BANNED, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s, DocumentStatus::BANNED);
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, 43);
    }    
}

void TestCheckRightRelevance() {
    
        SearchServer server;
        server.SetStopWords("и в на"s);
        server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
        const auto found_docs = server.FindTopDocuments("кот"s);
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.relevance, log(1.0 / 1);
     
}

void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestCheckFindingRightDocs);
    RUN_TEST(TestCheckMinusWords);
    RUN_TEST(TestCheckMatching);
    RUN_TEST(TestCheckMatchingAndMinusWords);
    RUN_TEST(TestCheckRelevance);
    RUN_TEST(TestCheckRightRating);
    RUN_TEST(TestCheckPredicate);
    RUN_TEST(TestCheckSearchByStatus);    
    RUN_TEST(TestCheckRightRelevance);
}


int main() {
    TestSearchServer();
    cout << "Search server testing finished"s << endl;
}
