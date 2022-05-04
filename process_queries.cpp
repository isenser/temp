#include "process_queries.h"

/*
Она принимает N запросов и возвращает вектор длины N,
i-й элемент которого — результат вызова FindTopDocuments для i-го запроса.
*/

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {

	std::vector<std::vector<Document>> documents_lists(queries.size());

   // for (const std::string& query : queries) {
   //     documents_lists.push_back(search_server.FindTopDocuments(query));
   // } 

   transform(
      execution::par, 
      queries.begin(), queries.end(),  // входной диапазон
      documents_lists.begin(), //выходной итератор для записи результата
      [&] (string s) {return search_server.FindTopDocuments(s);}
   );

    return documents_lists;
}


std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {

    vector<Document> result;
    for (const auto& doc : ProcessQueries(search_server, queries)) {
        result.insert(result.end(), doc.begin(),doc.end());
     }
     return result;
}


