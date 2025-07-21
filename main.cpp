// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.


/*
	TO DO:
	- JSON -> C++ Object
*/

/*
	Database search query endpoint:
	https://api.discogs.com/database/search?q={query}&{?type,title,release_title,credit,artist,anv,label,genre,style,country,year,format,catno,barcode,track,submitter,contributor}
*/

#include <iostream>
#include <format>
#include <string>
#include <string_view>
#include <curl/curl.h>

// For JSON
#include <fstream>
#include <nlohmann/json.hpp>

// For modelling search results
#include "search_result.h"

size_t write_callback(void *ptr, size_t size, size_t count, void *stream) {
	((std::string*) stream)->append((char*)ptr, 0, size*count);
	return size*count;
}

std::string get_environment_variable(const std::string_view& varname) {
	char* buf = nullptr;
	size_t sz = 0;

	if (_dupenv_s(&buf, &sz, varname.data()) != 0 || buf == nullptr) {
		throw std::runtime_error(std::format("Failed to get environment variable: {}", varname));
	}
	std::string result(buf);

	free(buf);
	return result;
}

int main () {

	CURL* curl = curl_easy_init ();											// Handles a network request
	struct curl_slist* header_list = NULL;									// List of request headers to send

	std::string endpoint = "https://api.discogs.com/database/search?q=acapella&sort=date_changed";
	
	// Discogs API search requests need authentication (env variables for privacy...)
	std::string consumer_key = get_environment_variable ("DISCOGS_CONSUMER_KEY");
	std::string consumer_secret = get_environment_variable ("DISCOGS_CONSUMER_SECRET");	

	std::string header_auth = "Authorization: Discogs key=" + consumer_key + ", secret=" + consumer_secret;
	std::string header_user_agent = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:140.0) Gecko/20100101 Firefox/140.0";		
																			// User Agent header is needed for access (else will be forbidden)	

	if (curl) {

		// Build the network request (url, header, useragent). c_str() is used because std::strings cannot be passed (curl is a C library)	
		curl_easy_setopt (curl, CURLOPT_URL, endpoint.c_str());											
		header_list = curl_slist_append (header_list, header_auth.c_str());
		curl_easy_setopt (curl, CURLOPT_HTTPHEADER, header_list);		
		curl_easy_setopt (curl, CURLOPT_USERAGENT, header_user_agent.c_str());

		std::cout << '\n' << (endpoint + "&key=" + consumer_key + "&secret=" + consumer_secret) << '\n';

		// Get response and save
		std::string response{};
		curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_callback);		// Callback function is invoked as soon as data are received. Called many times until all packets have arrived.
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);				// Write data to response object
		curl_easy_perform (curl);											// Perform network request (blocking manner, waits for transfer to complete/fail)
		
		if (!response.empty()) {
			std::cout << '\n' << "RETURNED:" << '\n';
			// std::cout << response << '\n';
		}

		else {
			std::cout << "Response is empty." << '\n';
		}
		
		// HANDLE JSON
		using json = nlohmann::json;
		auto json_data = json::parse (response);
		auto results = json_data["results"];

		for (int i = 0; i < results.size (); i++) {
			search_result sr;
			sr.title = results[i]["title"];
			sr.genre = results[i]["genre"][0];

			std::cout << i << ": " << sr.title << " (" << sr.genre << ")" << '\n';
		}

		// Clean up after request(s) complete
		curl_easy_cleanup (curl);
		curl_slist_free_all (header_list);
	}

	return 0;
}
