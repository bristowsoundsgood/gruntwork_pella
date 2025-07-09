// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <curl/curl.h>
#include <string>

size_t write_function (void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append ((char*)ptr, size * nmemb);
	return size * nmemb;
}

int main () {

	// Pointer to curl struct. Handle that is used for calls to API
	CURL *curl = curl_easy_init ();
	const char *endpoint = "https://meowfacts.herokuapp.com/";

	if (curl) {
		curl_easy_setopt (curl, CURLOPT_URL, endpoint);
		curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 1L);


		std::string response_string;
		std::string header_string;

		curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_function);
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response_string);
		curl_easy_setopt (curl, CURLOPT_HEADERDATA, &header_string);

		char* url;
		long response_code;
		double elapsed;

		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &response_code);
		curl_easy_getinfo (curl, CURLINFO_TOTAL_TIME, &elapsed);
		curl_easy_getinfo (curl, CURLINFO_EFFECTIVE_URL, &url);
	
		curl_easy_perform (curl);
		curl_easy_cleanup (curl);
		curl = NULL;
	}


	return 0;
}
