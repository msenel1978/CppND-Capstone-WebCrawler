#include <curl/curl.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
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

  while (crawl.requests < MAX_REQUESTS) {
    if (request_count == (MAX_REQUESTS_PER_URL)) {
      cout << "Giving up! Will try another url!" << endl;
      if (crawl.fetch_new_destination(&url_handle) == 0) {
        cout << "No URLs to be visited: List is empty" << endl;
        break;
      }
    }
    // Start crawling
    if (crawl.make_request(url_handle) == CURLE_OK) {
      request_count = 0;

      // Write visited web-site to the file
      visited_savefile.open("visited_urls.txt", ios::app);
      if (!visited_savefile)
        cout << "Could not write to file - No such file found";
      else {
        curl_url_get(url_handle, CURLUPART_URL, &url, 0);
        visited_savefile << url << endl;
        visited_savefile.close();
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

  // Print URLs to be visited
  cout << "Following urls are in the queue to be visited:" << endl;
  cout << "---------------------" << endl;
  crawl.print_to_be_visited();

  return 0;
}