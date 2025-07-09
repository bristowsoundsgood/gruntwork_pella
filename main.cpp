// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <curl/curl.h>
#include <string>

size_t write_callback(void *ptr, size_t size, size_t count, void *stream) {
	((std::string*) stream)->append((char*)ptr, 0, size*count);
	return size*count;
}

int main () {

	CURL* curl = curl_easy_init ();		// Handles a single network request
	const char* endpoint = "https://meowfacts.herokuapp.com/";

	if (curl) {
		curl_easy_setopt (curl, CURLOPT_URL, endpoint);		// Set URL to work with		

		
		std::string response{};
		curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_callback);		// Callback function is invoked as soon as data are received. Called many times until all packets have arrived.
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);				// Write data to response object
		curl_easy_perform (curl);											// Perform network request (blocking manner, waits for transfer to complete/fail)

		
		if (!response.empty()) {
			std::cout << '\n' << "RETURNED:" << '\n';
			std::cout << response << '\n';
		}

		else {
			std::cout << "Response is empty." << '\n';
		}

		curl_easy_cleanup (curl);
	}


	return 0;
}
