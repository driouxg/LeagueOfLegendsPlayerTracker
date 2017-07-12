#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <cpprest/json.h>
#include <sqlite3.h> 
#include <stdlib.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams


class HttpRestClient {

public:
	int leaguePoints;
	int summonerID;
	std::string sumName;
	utility::string_t url;

	inline HttpRestClient();
	inline json::value MakeRestRequest();
	inline int GetSummonerID();
	inline utility::string_t GetSummonerRank();
	inline void BuildUrl(utility::string_t firstPartOfUrl, utility::string_t name, utility::string_t key);
};



HttpRestClient::HttpRestClient() {

	// Constructor
	url = L"";
}

json::value HttpRestClient::MakeRestRequest() {

	try
	{
		// Create Client
		http_client client(url);

		// Make the request
		pplx::task<http_response> response = client.request(methods::GET);

		// Get the data in JSON
		http_response data = response.get();
		pplx::task<json::value> json = data.extract_json();

		return json.get();
	}
	catch (const http_exception& e) {
		// Print error.
		std::ostringstream ss;
		ss << e.what() << std::endl;
	}

	return NULL;
}

int HttpRestClient::GetSummonerID() {

	json::value& jobj = MakeRestRequest();
	try
	{
		//Print the entire json object
		//std::wcout << "\nValue: " << jobj << std::endl;

		// Set Global sumName
		this->sumName = utility::conversions::to_utf8string(jobj.at(U("name")).as_string());

		//Summoner ID (numeric value)
		int sumID = jobj.at(U("id")).as_integer();
		summonerID = sumID;
		return sumID;
	}
	catch (const http_exception& e) {
		std::ostringstream ss;
		ss << e.what() << std::endl;
	}
	return NULL;
}

utility::string_t HttpRestClient::GetSummonerRank() {

	json::value& jobj = MakeRestRequest();

	try
	{
		//Print out entire JSON object
		//std::wcout << "\nEntire Object: " << jobj.serialize() << std::endl;

		auto league = jobj[0].at(U("tier")).as_string();
		this->leaguePoints = jobj[0].at(U("leaguePoints")).as_integer();

		return league;
	}
	catch (const http_exception& e) {
		std::ostringstream ss;
		ss << e.what() << std::endl;
	}
	return NULL;
}

void HttpRestClient::BuildUrl(utility::string_t firstPartOfUrl, utility::string_t name, utility::string_t key) {
	this->url = firstPartOfUrl + name + key;
}