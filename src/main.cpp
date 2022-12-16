#include <curl/curl.h>
#include <stdio.h>

#include <iostream>
#include <memory>
#include <string>

#include "webcrawler.h"

using namespace std;

int main() {
  int request_count = 0;
  webCrawler crawl;
  CURLU *url_handle = curl_url();
  CURLUcode rc;

  // First url handle
  rc = curl_url_set(url_handle, CURLUPART_URL, "https://www.reuters.com", 0);

  if (rc) {
    // curl_url_strerror available with libcurl >=7.80.0
    // cout << "Problem with 1st destination: " << curl_url_strerror(rc) <<
    // endl;
    cout << "Problem with first destination: URL handle" << rc << endl;
    return rc;
  }
  // TODO: Worker thread(s)
  // while (crawl.fetch_visited_site_num() < MAX_REQUESTS) {

  // TODO: While queue is not empty!!!
  while (request_count < MAX_REQUESTS) {
    // Start crawling
    if (crawl.make_request(url_handle) == CURLE_OK) {
      request_count = 0;

      // TODO: Clean the debug prints
      cout << "Crawl object has " << crawl.buf_size() << "B buffered data"
           << "\n";
      if (crawl.fetch_new_destination(&url_handle) == 0) {
        cout << "Nor URLs to be visited: List is empty" << endl;
        break;
      }
      char *url;
      curl_url_get(url_handle, CURLUPART_URL, &url, 0);
      cout << "Following url is next: " << url << endl;
    } else
      cout << " Will try: " << MAX_REQUESTS - request_count << " times" << endl;

    // Print URLs already visited
    // cout << "Following sites have been visited so far:" << endl;
    // crawl.print_visited();
  }

  // Print URLs to be visited
  crawl.print_to_be_visited();

  return 0;
}