#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/queue.h>
#include <string>
#include <vector>
#include <event.h>
#include <evhttp.h>
#include <event2/keyvalq_struct.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include "curl/curl.h"
#include "json.hpp"
//#include "cdatabaseobject.h"
#include "easylogging++.h"

//#define HTTP_OK 0
//#define HTTP_INTERNAL 1
class HTTPRequest;
//class CDatabaseObject;

using json = nlohmann::json;
using HTTPRequestHandler = std::function<void(std::unique_ptr<HTTPRequest> req)>;

//extern std::unique_ptr<CDatabaseObject> dbptr;

enum RESPONSE_TPYE { ERROR = 0,OK = 1 };

static const int HAHS_SIZE = 64;

static const std::string ERROR_REQUEST ="invalid request";

struct HTTPPathHandler
{
    HTTPPathHandler() {}
    HTTPPathHandler(std::string _prefix, HTTPRequestHandler _handler):prefix(_prefix), handler(_handler){}
    std::string prefix;
    HTTPRequestHandler handler;
};
//
struct UTXOauth
{
    UTXOauth(){}
    std::string url;
    std::string pass;
};

class HTTPRequest
{
private:
    struct evhttp_request* req;
public:
    HTTPRequest(struct evhttp_request* req);
    ~HTTPRequest();

    enum RequestMethod
    {
        UNKNOWN,
        GET,
        POST,
        HEAD,
        PUT,
		OPTIONS
    };

    std::string GetURI();

    RequestMethod GetRequestMethod();

    std::string GetHeader();

    std::string ReadBody();

    void WriteHeader(const std::string& hdr, const std::string& value);

    void WriteReply(int nStatus, const std::string& strReply = "");
};



void signalHandler(int sig);
void registerHTTPHandler(const std::string &prefix, const HTTPRequestHandler &handler);
void httpRequestCb(struct evhttp_request *req, void *arg);
//balanc taoist_ma 
void GetBasicInformation(std::unique_ptr<HTTPRequest>  req);
bool getBalance(const std::string& response);
bool getBlockCount(const std::string& blockcount);
bool getBlockHash(const  std::string& blockcount, const std::string &blockhash);
bool getBlock(const std::string& blockhash, const std::string &blockinfo);
bool getRawtransaction(const std::string& txhash, const std::string &txInformation);
bool curlBitcoinReq(const std::string &data, const std::string &response = "");



template < class T>
std::string makeReplyMsg(bool type,T& t)
{
    json response = json::object();

    response["code"] = type ? RESPONSE_TPYE::OK : RESPONSE_TPYE::ERROR;
    response["data"] = t;

    LOG(INFO) << "REPLY_MSG : " << response.dump() << "\n\n";
    return  response.dump();
}

#endif //server.h
