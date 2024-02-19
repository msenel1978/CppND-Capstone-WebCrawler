#include <curl/curl.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#include "webcrawler.h"

using namespace std;

ofstream visited_savefile;

int main() {
  int request_count = 0;
  webCrawler crawl;
  CURLU *url_handle = curl_url();
  char *url;
  CURLUcode rc;
  static std::mutex file_mutex;

  // First url handle
  //rc = curl_url_set(url_handle, CURLUPART_URL, "https://www.reuters.com", 0);
  rc = curl_url_set(url_handle, CURLUPART_URL, "http://www.reddit.com", 0);

  //TODO: Do this for all URLs
  /* Some web-sites does not allow carwling  / scrapping and return HTTP 403 (Forbidden Error)
    There are different ways to by-pass this, i.e.; via a proxy. 
    Another way is to start scraping with the Google Cache version, which is implemented here */
  //rc = curl_url_set(url_handle, CURLUPART_URL, "https://webcache.googleusercontent.com/search?q=cache:http://www.reddit.com", 0);


  if (rc) {
    // curl_url_strerror available with libcurl >=7.80.0
    // cout << "Problem with 1st destination: " << curl_url_strerror(rc) <<
    // endl;
    cout << "Problem with first destination: URL handle" << rc << endl;
    return rc;
  }

  // TODO: Worker thread(s)
  visited_savefile.open("visited_urls.txt", ios::app);

  while (crawl.requests < MAX_REQUESTS) {
    if (request_count == (MAX_REQUESTS_PER_URL)) {
      cout << "Giving up! Will try another url!" << endl;
      request_count = 0;
      if (crawl.fetch_new_destination(&url_handle) == 0) {
        cout << "No URLs to be visited: List is empty" << endl;
        break;
      }
    }
    // Start crawling
    if (crawl.make_request(url_handle) == CURLE_OK) {
      // Will exist after MAX_REQUESTS successful requests
      crawl.requests++;

      request_count = 0;

      if (!visited_savefile)
        cout << "Could not write to file - No such file found";
      else {
        // RAII-style mechanism for owning a mutex
        // Lock |file_mutex| before accessing |visited_savefile|.
        std::lock_guard<std::mutex> lock(file_mutex);

        // Write visited web-site to the file
        curl_url_get(url_handle, CURLUPART_URL, &url, 0);
        visited_savefile << url << endl;
      }

      // TODO: Clean the debug prints
      cout << "Crawl object has " << crawl.buf_size() << "B buffered data"
           << "\n";
      if (crawl.fetch_new_destination(&url_handle) == 0) {
        cout << "No URLs to be visited: List is empty" << endl;
        break;
      }
      curl_url_get(url_handle, CURLUPART_URL, &url, 0);
      cout << "Following url is next: " << url << endl;
    } else {
      request_count++;
      cout << " Will try: " << MAX_REQUESTS_PER_URL - request_count << " times"
           << endl;
    }

    // Print URLs already visited
    // cout << "Following sites have been visited so far:" << endl;
    // crawl.print_visited();
  }
  visited_savefile.close();

  // Print URLs to be visited
  cout << "Following urls are in the queue to be visited:" << endl;
  cout << "---------------------" << endl;
  crawl.print_to_be_visited();

  return 0;
}