#include "html_parser.h"

#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "webcrawler.h"

using namespace std;

int is_html(char *ctype) {
  return ctype != NULL && strlen(ctype) > 10 && strstr(ctype, "text/html");
}

/* HREF finder implemented in libxml2 but could be any HTML parser */
size_t HTML_Parser::follow_links(CURL *curl_handle,
                                 std::shared_ptr<memory_t> mem, char *url,
                                 webCrawler *crawler) {
  // size_t HTML_Parser::follow_links(CURL *curl_handle, memory_t *mem, char
  // *url) {
  int opts = HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING |
             HTML_PARSE_NONET;

  htmlDocPtr doc = htmlReadMemory(mem->buf, mem->size, url, NULL, opts);
  if (!doc) return 0;
  xmlChar *xpath = (xmlChar *)"//a/@href";
  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
  xmlXPathFreeContext(context);
  if (!result) return 0;
  xmlNodeSetPtr nodeset = result->nodesetval;
  if (xmlXPathNodeSetIsEmpty(nodeset)) {
    xmlXPathFreeObject(result);
    return 0;
  }
  size_t count = 0;
  int i;
  for (i = 0; i < nodeset->nodeNr; i++) {
    double r = rand();
    int x = r * nodeset->nodeNr / RAND_MAX;
    const xmlNode *node = nodeset->nodeTab[x]->xmlChildrenNode;
    xmlChar *href = xmlNodeListGetString(doc, node, 1);
    if (follow_relative_links) {
      xmlChar *orig = href;
      href = xmlBuildURI(href, (xmlChar *)url);
      xmlFree(orig);
    }
    char *link = (char *)href;
    if (!link || strlen(link) < 20) continue;
    if (!strncmp(link, "http://", 7) || !strncmp(link, "https://", 8)) {
      // curl_multi_add_handle(multi_handle, make_handle(link));
      CURLU *new_url_handle = curl_url();
      CURLUcode rc;

      rc = curl_url_set(new_url_handle, CURLUPART_URL, link, 0);

      // Add new url to the visited url list
      crawler->add_url_to_be_visited(new_url_handle);
      // TODO: Write the link to a file

      if (count++ == max_link_per_page) break;
    }
    xmlFree(link);
  }
  xmlXPathFreeObject(result);

  return count;
}
