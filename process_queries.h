#pragma once

// #include <iostream>
// #include <random>
//#include <string>
// #include <string_view>

// #include "log_duration.h"

#include "read_input_functions.h"
#include "document.h"
#include "paginator.h"
#include "string_processing.h"
#include "search_server.h"
#include "request_queue.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <execution>



/*
Она принимает N запросов и возвращает вектор длины N,
i-й элемент которого — результат вызова FindTopDocuments для i-го запроса.
*/
std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);

std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);
