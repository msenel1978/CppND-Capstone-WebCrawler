#include "webcrawler.h"

#include <curl/curl.h>

#include <iostream>
#include <memory>
#include <regex>
#include <set>
#include <string>

#include "html_parser.h"

using namespace std;

webCrawler::webCrawler() {
  // Stop after MAX_REQUESTS requests
  requests = 0;
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // curl handle
  curl = curl_easy_init();

  // Verbose mode
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

  if (curl) {
    /* Important: use HTTP2 over HTTPS */
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                     webCrawler::write_data_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, mem.get());
    curl_easy_setopt(curl, CURLOPT_PRIVATE, mem.get());

    /* For completeness */
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "mini crawler");
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, 1L);
    curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_EXPECT_100_TIMEOUT_MS, 0L);
  }
}

/// @brief
/// @param destination_handle
/// @return
CURLcode webCrawler::make_request(CURLU *destination_url_handle) {
  char *url;
  CURLUcode rc = curl_url_get(destination_url_handle, CURLUPART_URL, &url, 0);
  CURLcode res;
  long res_status;

  if (!rc) {
    // Buffer for received data
    mem->size = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
      curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_status);

      //  Successful request
      if (res_status == 200) {
        char *ctype;
        curl_easy_getinfo(this->curl, CURLINFO_CONTENT_TYPE, &ctype);

        // Debug prints
        // cout << "HTTP 200 (" << ctype << "): " << url << endl;
        // cout << "We received " << mem->size << "B of data" << endl;

        if (is_html(ctype) && this->mem->size > 100) {
          HTML_Parser parser;

          parser.follow_links(this->curl, this->mem, url, this);

          // Increment the counter for requests.
          // If max. is reached, the object will exit
          requests++;
        }

      } else {
        cout << "HTTP " << (int)res_status << " " << url << endl;
      }

      curl_free(url);

    } else
      cout << "Connection failure - CURLCode: " << curl_easy_strerror(res)
           << " to: " << url << endl;
  }
  return res;
}

size_t webCrawler::write_data(void *contents, size_t sz, size_t nmemb,
                              void *ctx) {

  size_t realsize = sz * nmemb;
  memory_t *tmp_mem = (memory_t *)ctx;
  char *ptr = (char *)realloc(tmp_mem->buf.release(), tmp_mem->size + realsize);
  if (!ptr) {
    /* out of memory */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  tmp_mem->buf.reset(ptr); // Assign the new buffer to the unique_ptr
  memcpy(&(tmp_mem->buf.get()[tmp_mem->size]), contents, realsize);
  tmp_mem->size += realsize;
  return realsize;
}

// TODO: Pop only if the request is successfull
size_t webCrawler::fetch_new_destination(CURLU **url_handle) {
  size_t num_queue = _urls_to_be_visited.size();

  // Nothing in the queue, exit
  if (num_queue == 0) return num_queue;

  // Get a new url handle from the queue
  *url_handle = _urls_to_be_visited.front();
  // Remove this from the queue
  // TODO: Pop only if the request is successfull
  _urls_to_be_visited.pop();

  // Add it to the _urls_visited
  _urls_visited.push(*url_handle);

  return _urls_to_be_visited.size();
}

webCrawler::~webCrawler() {
  CURLU *url_handle;
  char *url;

  if (curl) {
    // Clean the url handles

    // For the implementation that _urls_visited and _urls_to_be_visited are
    // vectors
    // for (CURLU *url_handle : _urls_visited) curl_url_cleanup(url_handle);

    // for (CURLU *url_handle : _urls_to_be_visited)
    // curl_url_cleanup(url_handle);

    while (!_urls_visited.empty()) {
      url_handle = _urls_visited.front();
      curl_url_get(url_handle, CURLUPART_URL, &url, 0);
      _urls_visited.pop();

      // TODO: I thought I need to do this
      // curl_url_cleanup(url_handle);
    }

    while (!_urls_to_be_visited.empty()) {
      url_handle = _urls_visited.front();
      _urls_to_be_visited.pop();

      // TODO: I thought I need to do this
      // curl_url_cleanup(url_handle);
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }
}
