#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "webcrawler.h"

int is_html(char *ctype);

class HTML_Parser {
 private:
  static int const max_link_per_page = 100;
  static int const follow_relative_links = 0;

 public:
  // size_t follow_links(CURLM *multi_handle, memory *mem, char *url);
  size_t follow_links(CURL *curl_handle, std::shared_ptr<memory_t> mem,
                      char *url, webCrawler *crawler);
  // size_t follow_links(CURL *curl_handle, memory_t *mem, char *url);
};

#endif  // HTML_PARSER_H