#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H

#include <curl/curl.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <string>

#define MAX_REQUESTS_PER_URL 3
#define MAX_REQUESTS 10000
#define MAX_LINKS_PER_PAGE 100

typedef struct memory {
  std::unique_ptr<char> buf{new char};
  size_t size;
} memory_t;

class webCrawler {
 private:
  CURL *curl;

  std::shared_ptr<memory_t> mem{new memory_t};
  std::queue<CURLU *> _urls_visited;
  std::queue<CURLU *> _urls_to_be_visited;

 public:
  int requests;

  webCrawler();
  ~webCrawler();

  // TODO: High level function that makes the request and fetch pull web-site
  CURLcode make_request(CURLU *destination_url_handle);
  // Pop a new url from the list of to be visited
  size_t fetch_new_destination(CURLU **url_handle);

  // Add discovered URL
  void add_url_to_be_visited(CURLU *new_url) {
    _urls_to_be_visited.push(new_url);
  }

  // Function to flush visited sites to a file
  void flush_visited_sites(std::ofstream &visited_savefile, std::mutex &file_mutex);

  /* Utility function to parse urls */
  int findURLs_in_buf(char *received_buf);

  size_t write_data(void *contents, size_t sz, size_t nmemb, void *ctx);

  static void write_data_callback(void *ctx, size_t sz, size_t nmemb,
                                  void *context) {
    // TODO: Verify that the object / ctx is correct conceptually.
    auto *object = (webCrawler *)ctx;
    object->write_data(ctx, sz, nmemb, context);
  }

  int visited_site_num() { return _urls_visited.size(); }

  // Utility function to check whether there urls in the queue to be visited
  bool more_urls_to_visit() { return !_urls_to_be_visited.empty(); }

  // Utility function to print url's to be visited
  // TODO: Template for print?
  void print_to_be_visited() {
    CURLU *new_url_handle;
    char *url;
    CURLUcode rc;
    // Create a copy of _urls_visited
    std::queue<CURLU *> print_queue = _urls_to_be_visited;

    std::cout << "URLs to be visited: " << _urls_to_be_visited.size()
              << std::endl;

    while (!print_queue.empty()) {
      new_url_handle = print_queue.front();
      curl_url_get(new_url_handle, CURLUPART_URL, &url, 0);
      std::cout << url << std::endl;
      print_queue.pop();
    }

    // Vector implementation of _urls_to_be_visited
    /*for (CURLU *new_url_handle : _urls_to_be_visited) {
      curl_url_get(new_url_handle, CURLUPART_URL, &url, 0);
      std::cout << url << std::endl;
    }*/

    if (_urls_to_be_visited.size()) curl_free(url);
  }

  // Utility function to print url's already visited
  void print_visited() {
    CURLU *new_url_handle;
    char *url;
    CURLUcode rc;
    // Create a copy of _urls_visited
    std::queue<CURLU *> print_queue = _urls_visited;

    std::cout << "URLs already visited: " << _urls_visited.size() << std::endl;

    while (!print_queue.empty()) {
      new_url_handle = print_queue.front();
      curl_url_get(new_url_handle, CURLUPART_URL, &url, 0);
      std::cout << url << std::endl;
      print_queue.pop();
    }

    // Vector implementation of _urls_visited
    /*for (CURLU *new_url_handle : _urls_visited) {
      curl_url_get(new_url_handle, CURLUPART_URL, &url, 0);
      std::cout << url << std::endl;
    }*/

    if (_urls_visited.size()) curl_free(url);
  }

  // Utility function to check how many bytes are in the buffer
  int buf_size() { return mem->size; }
};

#endif  // WEBCRAWLER_H
