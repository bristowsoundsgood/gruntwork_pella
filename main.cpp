// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.


// Send as header, not as query
// JSON -> Object needed

/*
	Database search query endpoint:
	https://api.discogs.com/database/search?q={query}&{?type,title,release_title,credit,artist,anv,label,genre,style,country,year,format,catno,barcode,track,submitter,contributor}
*/

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <string_view>

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
	std::string result(buf, sz);

	free(buf);
	return result;
}

int main () {

	CURL* curl = curl_easy_init ();		// Handles a network request

	// Discogs API search requests need authentication (env variables for privacy...)
	std::string consumer_key = get_environment_variable ("DISCOGS_CONSUMER_KEY");
	std::string consumer_secret = get_environment_variable ("DISCOGS_CONSUMER_SECRET");
	
	std::string endpoint = "https://api.discogs.com/database/search?q=acapella";
	std::string auth_header = "&key=" + consumer_key + "&secret=" + consumer_secret;


	std::string full_url = endpoint + auth_header;		// Must concatenate before passing into the next line (avoids dangling pointers caused by temporary object)
	const char* url = full_url.c_str ();				// curl is a C library, therefore std::strings cannot be passed

	if (curl) {
		curl_easy_setopt (curl, CURLOPT_URL, url);		// Set URL to work with		
		
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
