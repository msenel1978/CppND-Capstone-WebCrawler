#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "webcrawler.h"

int is_html(char *ctype);

//TODO: Any invariants that need to be maintained?
/* This class is responsible for parsing the HTML and extracting the links
from the page.
The urls parsed are added to the webCrawler's queue of urls to be visited.
*/
class HTML_Parser {
 private:
  int max_link_per_page;
  static int const follow_relative_links = 0;

 public:
  HTML_Parser(int max_links) : max_link_per_page(max_links) {}

  // size_t follow_links(CURLM *multi_handle, memory *mem, char *url);
  size_t follow_links(CURL *curl_handle, std::shared_ptr<memory_t> mem,
                      char *url, webCrawler *crawler);
  // size_t follow_links(CURL *curl_handle, memory_t *mem, char *url);
};

#endif  // HTML_PARSER_H