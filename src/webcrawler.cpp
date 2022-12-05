#include "webcrawler.h"

#include <curl/curl.h>

#include <iostream>
#include <memory>
#include <regex>
#include <set>
#include <string>

#include "html_parser.h"

using namespace std;

size_t webCrawler::write_data(void *contents, size_t sz, size_t nmemb,
                              void *ctx) {
  size_t realsize = sz * nmemb;
  memory_t *tmp_mem = (memory_t *)ctx;
  char *ptr = (char *)realloc(tmp_mem->buf, tmp_mem->size + realsize);
  if (!ptr) {
    /* out of memory */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  tmp_mem->buf = ptr;
  memcpy(&(tmp_mem->buf[tmp_mem->size]), contents, realsize);
  tmp_mem->size += realsize;
  return realsize;
}

webCrawler::webCrawler() {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // curl handle
  curl = curl_easy_init();

  // Verbose mode
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

  if (curl) {
    // Buffer for received data
    mem = (memory_t *)malloc(sizeof(memory_t));
    mem->size = 0;
    mem->buf = (char *)malloc(1);
    const char *data = "robots.txt";

    /* Important: use HTTP2 over HTTPS */
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                     webCrawler::write_data_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, mem);
    curl_easy_setopt(curl, CURLOPT_PRIVATE, mem);

    /* For completeness */
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "mini crawler");
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, 1L);
    curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_EXPECT_100_TIMEOUT_MS, 0L);
  }
}

CURLcode webCrawler::make_request(CURLU *destination_handle) {
  char *url;
  CURLUcode rc = curl_url_get(destination_handle, CURLUPART_URL, &url, 0);
  CURLcode res;
  long res_status;

  if (!rc) {
    curl_easy_setopt(curl, CURLOPT_URL, url);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
      curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_status);

      //  Successful request
      if (res_status == 200) {
        char *ctype;
        curl_easy_getinfo(this->curl, CURLINFO_CONTENT_TYPE, &ctype);
        cout << "HTTP 200 (" << ctype << "): " << url << endl;
        cout << "We received " << mem->size << "B of data" << endl;

        if (is_html(ctype) && this->mem->size > 100) {
          HTML_Parser parser;

          parser.follow_links(this->curl, this->mem, url);
        }

      } else {
        cout << "HTTP " << (int)res_status << " " << url << endl;
      }

    } else
      cout << "Connection failure - CURLCode: " << curl_easy_strerror(res)
           << " to: " << url << endl;

    curl_free(url);
    curl_easy_cleanup(curl);
  }

  return res;
}

webCrawler::~webCrawler() {
  if (curl) {
    // Buffer clean-up
    free(mem->buf);
    free(mem);

    curl_global_cleanup();
  }
}
