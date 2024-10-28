# CPPND: Capstone WebCrawler Repo

This is the repo for the Capstone project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).

This Capstone Project is a web-crawler application: Once given an initial url, the application pulls the web-site, parses urls on that site and writes into a queue. Then it deques urls from the queue and pulls those web-sites. In another thread, the application writes the visited urls into a file.

This application is based on the crawler application here: https://curl.se/libcurl/c/crawler.html, which is implemented in C. However, I extended it with C++ clases smart poimters and concurrency.

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* libcurl
  *  [click here for installation instructions](https://ec.haxx.se/install/)
* LibXml2
  * Most Linux and BSD distributions include libxml
  * Official releases can be downloaded from https://download.gnome.org/sources/libxml2/
  * The git repository with build instructions is hosted on GNOME's GitLab server: https://gitlab.gnome.org/GNOME/libxml2

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./WebCrawler http://www.reddit.com`.

## Rubric Points:
### README (All Rubric Points REQUIRED) (MET 3/3)
* A README with instructions is included with the project. MET:This file
* The README indicates the new features you added to the game. MET:
  * My project is based on the old Capstone instructions so it is a completely new application, which is based on the crawler application here: https://curl.se/libcurl/c/crawler.html. The original application is implemented in C. However, I extended it with C++ clases and added smart pointers and concurrency.

* The README includes information about each rubric point addressed. MET: This section

### Compiling and Testing (All Rubric Points REQUIRED) (MET 1/1)
* The submission must compile and run without errors on the Udacity project workspace. MET:
  * CMake and Make is used. LibXml2 and libcurl are needed and install instructios are included in the "Dependencies for Running Locally" Section.

### Loops, Functions, I/O - meet at least 2 criteria (MET 4/4)
* The project demonstrates an understanding of C++ functions and control structures. MET:
  * A variety of member functions in webcrawler.h/cpp.

* The project reads data from a file and process the data, or the program writes data to a file. MET:
  * In webcraler.cpp webCrawler::flush_visited_sites writes to "visited_urls.txt"

* The project accepts user input and processes the input. MET:
  * The program is started with the initial url as the input. This initial url is the web-site to start crawling with, e.g.; ./WebCrawler http://www.reddit.com

* The project uses data structures and immutable variables. MET:
  * memory_t (declared in webcrawler.h). 
  * 2 queues are used extensizely to process urls to be visited (queue _urls_to_be_visited in webcrawler.h). As soon a url is visited, it is moved to another queue, i.e.; _urls_visited in webcrawler.h)

### Object Oriented Programming - meet at least 3 criteria (MET 3/6)
* One or more classes are added to the project with appropriate access specifiers for class members. - MET:
  * Two classes are defined: 1) webCrawler  and HTML_Parser with public, protected, or private members. private member data hidden and accesible via member methods, e.g., CURL handle *curl in webCrawler class or max_link_per_page in HTML_Parser.

* Class constructors utilize member initialization lists. MET:
  * All class members that are set to argument values are initialized through member initialization lists.   HTML_Parser(int max_links) : max_link_per_page(max_links) {} in html_parser.h,

* Classes abstract implementation details from their interfaces. MET:
  * All class member functions document their effects, either through function names or comments.

* Overloaded functions allow the same function to operate on different parameters. (NOT MET/N.A.)
* Classes follow an appropriate inheritance hierarchy with virtual and override functions. (NOT MET/N.A.)
* Templates generalize functions or classes in the project. (NOT MET/N.A.)

### Memory Management - meet at least 3 criteria (MET 5/6)
* The project makes use of references in function declarations. MET:
  * e.g.; size_t HTML_Parser::follow_links(CURL *curl_handle, std::shared_ptr<memory_t> mem, char *url, webCrawler *crawler) in html_parser.cpp OR CURLcode make_request(CURLU *destination_url_handle) in webcrawler.cpp

* The project uses destructors appropriately. MET:
  * webCrawler::~webCrawler() makes sure that the queues are popped until empty. Furthermore, curl clean-up functions are called.

* The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate: The project follows the Resource Acquisition Is Initialization pattern where appropriate, by allocating objects at compile-time, initializing objects when they are declared, and utilizing scope to ensure their automatic destruction. MET:
  * e.g.; std::lock_guard<std::mutex> lock(file_mutex) in webcrawler.cpp

* The project follows the Rule of 5. - NOT MET/N.A.

* The project uses move semantics to move data instead of copying it, where possible. MET:
  *  htmlReadMemory(std::move(mem->buf).get(), mem->size, url, NULL, opts) in html_parser.cpp

* The project uss smart pointers instead of raw pointers. The project uses at least one smart pointer: unique_ptr, shared_ptr, or weak_ptr. MET:
  * In webCrawlerclass, we have used a nique_ptr std::unique_ptr<char> buf{new char} and std::shared_ptr<memory_t> mem{new memory_t};


### Concurrency - meet at least 2 criteria (MET 2/4)
* The project uses multithreading. MET:
  * std::thread file_writer_thread(std::bind(&webCrawler::flush_visited_sites, &crawl, std::ref(visited_savefile), std::ref(file_mutex))) declared in main.cpp. Based on this thread, visited sites are popped from a queue and flushed (written) to a file.

* A promise and future is used in the project. (NOT MET/N.A.)
A promise and future is used to pass data from a worker thread to a parent thread in the project code.

* A mutex or lock is used in the project. MET:
  * In webcrawler.h/cpp: void flush_visited_sites(std::ofstream &visited_savefile, **std::mutex &file_mutex**);
  * In webcrawler.cpp, in webCrawler::flush_visited_sites std::**lock_guard**<std::mutex> lock(file_mutex)

* A condition variable is used in the project. (NOT MET/N.A.)
A std::condition_variable is used in the project code to synchronize thread execution.