#include <curl/curl.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <functional>
#include <stdexcept>
#include <string>

#include <thread>

#include "webcrawler.h"

using namespace std;

#ifdef _DEBUG
#define DEBUG_PRINT(x) std::cout << "DEBUG: "<< x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

ofstream visited_savefile;

int main(int argc, char* argv[])
{
  int request_count = 0;
  webCrawler crawl;
  CURLU *url_handle = curl_url();
  char *url;
  CURLUcode rc;
  static std::mutex file_mutex;
  std::thread file_writer_thread(std::bind(&webCrawler::flush_visited_sites, &crawl,
                                std::ref(visited_savefile), std::ref(file_mutex)));

   if (argc < 2) {
    cout << "Usage: WebCrawler URL" << endl;
    cout << "Example: WebCrawler http://www.reddit.com" << endl;
    return 1;
  }

  // First url handle
  //rc = curl_url_set(url_handle, CURLUPART_URL, "http://www.reddit.com", 0);
  rc = curl_url_set(url_handle, CURLUPART_URL, argv[1], 0);

  //TODO: #6 Do this for all URLs
  /* Some web-sites do not allow carwling  / scrapping and return HTTP 403 (Forbidden Error)
    There are different ways to by-pass this, i.e.; via a proxy. 
    Another way is to start scraping with the Google Cache version, which is implemented here */
  //rc = curl_url_set(url_handle, CURLUPART_URL, "https://webcache.googleusercontent.com/search?q=cache:http://www.reddit.com", 0);

  if (rc) {
    // curl_url_strerror available with libcurl >=7.80.0
    // cout << "Problem with 1st destination: " << curl_url_strerror(rc) <<
    // endl;
    cout << "Problem with first destination: URL handle " << rc << endl;
    cout << "Example: WebCrawler http://www.reddit.com" << endl;
    return rc;
  }

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

      // TODO: Clean the debug prints
      //cout << "Crawl object has " << crawl.buf_size() << "B buffered data"
      //     << "\n";
      if (crawl.fetch_new_destination(&url_handle) == 0) {
        cout << "No URLs to be visited: List is empty" << endl;
        break;
      }
      curl_url_get(url_handle, CURLUPART_URL, &url, 0);
      // TODO: Clean this debug print
      // cout << "Following url is next: " << url << endl;
    } else {
      request_count++;
      cout << " Will try: " << MAX_REQUESTS_PER_URL - request_count << " times"
           << endl;
    }
    // TODO: Clean this debug print
    cout << "Visited queue size: " << crawl.visited_site_num()  << endl;
  }

  // Print URLs to be visited
  // cout << "Following urls are in the queue to be visited:" << endl;
  // cout << "---------------------" << endl;
  // crawl.print_to_be_visited();

  // Print URLs already visited
  cout << "1) Following visited sites have been left out in the queue:" << endl;
  crawl.print_visited();

  file_writer_thread.join();
  cout << "2) Following visited sites have been left out in the queue:" << endl;
  crawl.print_visited();

  return 0;
}
