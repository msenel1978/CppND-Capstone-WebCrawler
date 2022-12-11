#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H

#include <curl/curl.h>

#include <memory>
#include <string>
#include <vector>
#include <iostream>

typedef struct {
  char *buf;
  size_t size;
} memory_t;

class webCrawler {
 private:
  CURL *curl;

  // TODO: Can I have this as a shared_ptr?
  memory_t *mem;
  std::vector<CURLU*> _urls_visited;
  std::vector<CURLU*> _urls_to_be_visited;

 public:
  webCrawler();
  ~webCrawler();

  // TODO: High level function that makes the request and fetch the web-site
  CURLcode make_request(CURLU *destination);

  // Add discovered URL
  void add_url_to_be_visited(CURLU *new_url) {
    _urls_to_be_visited.push_back(new_url);
  }

  /* Utility function to parse urls */
  int findURLs_in_buf(char *received_buf);

  size_t write_data(void *contents, size_t sz, size_t nmemb, void *ctx);

  static void write_data_callback(void *ctx, size_t sz, size_t nmemb,
                                  void *context) {
    // TODO: Verify that the object / ctx is correct conceptually.
    auto *object = (webCrawler *)ctx;
    object->write_data(ctx, sz, nmemb, context);
  }

  // Utility function to print url's to be visited
  void print_to_be_visited() {
    char *url;
    CURLUcode rc;

    std::cout << "URLs to be visited: " << _urls_to_be_visited.size() << std::endl;

    for (CURLU *new_url_handle : _urls_to_be_visited) {
      curl_url_get(new_url_handle, CURLUPART_URL, &url, 0);
      std::cout << url << std::endl;
    }

    curl_free(url);
}

// Utility function to print url's already visited
void print_visited() {
    char *url;
    CURLUcode rc;

    std::cout << "URLs already visited: " << _urls_visited.size() << std::endl;

    for (CURLU *new_url_handle : _urls_visited) {
      curl_url_get(new_url_handle, CURLUPART_URL, &url, 0);
      std::cout << url << std::endl;
    }

    curl_free(url);
}

  // Utility function to check how many bytes are in the buffer
  int buf_size() { return mem->size; }
};

#endif  // WEBCRAWLER_H
