// g++ -std=c++11 -pthread crawler.cpp -o crawler -lcurl -lboost_regex

#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <mutex>  
#include <chrono>

#include <boost/regex.hpp>
#include <curl/curl.h>

using namespace std;


class Archive {
private:
    set<string> urls; 
    mutex rmtx, mtx;
    int rcnt;
public:

    Archive() {
        rcnt = 0;
    }

    bool check(string s) {
           rmtx.lock();
           rcnt++;
           if(rcnt==1) {
               mtx.lock();
           }
           rmtx.unlock();

           bool ans = urls.count(s)>0;

           rmtx.lock();
           rcnt--;
           if(rcnt==0) {
               mtx.unlock();
           }
           rmtx.unlock();

           return ans;
    }

    void add(string s) {
        mtx.lock();
        urls.insert(s);
        mtx.unlock();
    }

    string pop() {
        mtx.lock();
        string ans="";
        if(!urls.empty()) {
            ans = *urls.begin();
            urls.erase(urls.begin());
        }
        mtx.unlock();
        return ans;
    }

    int size() {
        mtx.lock();
        int ans = urls.size();
        mtx.unlock();
        return ans;
    }

    void print() {
        while(!urls.empty()) {
            string s = pop();
            cout<<s<<endl;
        }
    }
};

Archive host_urls;
Archive all_urls;
Archive working_urls;

// **************************** cURL specific code **********************************

size_t string_write(void *contents, size_t size, size_t nmemb, void *userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// is thread safe, see http://curl.haxx.se/libcurl/features.html#thread
CURLcode curl_read(const string& url, string& buffer, long timeout = 30) {
    CURLcode code(CURLE_FAILED_INIT);

    CURL* curl = curl_easy_init();

    if(curl) {    
        if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))
        && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, string_write))
        && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer))
        && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L))
        && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L))
        && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
        && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
        && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))) {

          code = curl_easy_perform(curl);
        }
        curl_easy_cleanup(curl);
    }
    return code;
}

// ************* Extracting links from document using Boost Regex ***************

// hack to combine 'tail' uri from page 'head' (tail can be relative)
string merge_url(const string& head, const string& tail) {
    if (! tail.empty()) {
        if (tail.find("http") == 0) {
            return tail;
        } else if (tail.find("//") == 0) {
            return "http" + tail;
        } else if (tail[0] != '/') {
            return head + '/' + tail;
        }

        size_t proto_pos = head.find("//");
        size_t first_slash_pos = head.find("/", proto_pos + 2);

        if (first_slash_pos != string::npos) {
            return head.substr(0, first_slash_pos) + tail;
        } else {
            return head + tail; 
        }
    } else {
        return head;
    }
}

set<string> extract_links(const string& document_url, const string& document) {
    boost::regex re("<a href=\"([^\"#]+)\"");
    boost::sregex_token_iterator iter(document.begin(), document.end(), re, 0);
    boost::sregex_token_iterator iter_end;
    set<string> extracted_urls;

    for (; iter != iter_end; ++iter) {
        string url = (*iter);
        url = merge_url(document_url, url.substr(9, url.size() - 10)); // strip <a href= and " chars
        extracted_urls.insert(url);
    }
    return extracted_urls;
}

// ************* Example of downloading specified URL and printing extracted links ************


set<string> get_references(string url) {
    string document;
    CURLcode res = curl_read(url, document);
    set<string> links;
    if (res == CURLE_OK) {
        links = extract_links(url, document);
    } else {
        cerr << "CURL ERROR: " << url << "\n" << curl_easy_strerror(res) << endl;
    }
    return links;
}

string get_host(string &s) {
   int pos = s.find("//");
   pos = s.find("/", pos+2);
   string host = s.substr(0, pos);
//cout<<s<<" "<<host<<endl;
   return host;
}

const int NEED_URLS = 100;

void thread_func() {
   for(;;) {
//#cerr<<all_urls.size()<<endl;
       string url = working_urls.pop();
       if(all_urls.size()>NEED_URLS)
               break;
       if(all_urls.check(url) || url.size()>33 || url.size()==0) continue;
       all_urls.add(url);
       
       set<string> to = get_references(url);
       for(string s : to) {
           if(!all_urls.check(s) && !working_urls.check(s))
               working_urls.add(s),
                host_urls.add(get_host(s));
       }
   }
}

int main(int argc, const char* argv[]) {
    if (argc < 1) {
        printf("Usage: %s URL\n", argv[0]);
        return 1;
    }
    string url = argv[1];

    working_urls.add(url);

    // not thread safe, must be initialized once outside threads
    curl_global_init(CURL_GLOBAL_ALL);

    vector<thread> workers;
    const int THREADS = 1;
    for(int i=0; i<THREADS; ++i)
        workers.push_back(thread(thread_func));

//this_thread::sleep_for(chrono::seconds(10));
    for (thread &t : workers) t.join();

    cout << "Found " << all_urls.size() << " links:" << endl;
    all_urls.print();


    cout << "Found " << host_urls.size() << " hosts:" << endl;
    host_urls.print();

    // not thread safe, must be deinitialized once outside threads
    curl_global_cleanup();

    return 0;    
}
