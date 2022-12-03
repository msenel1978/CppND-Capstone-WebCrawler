#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "webcrawler.h"

class HTML_Parser {
 private:
  static int const max_link_per_page = 5;
  static int const follow_relative_links = 0;

 public:
  size_t follow_links(CURLM *multi_handle, memory_t *mem, char *url);
  int is_html(char *ctype);
};

#endif  // HTML_PARSER_H