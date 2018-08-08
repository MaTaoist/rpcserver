
#include "server.h"

std::vector<HTTPPathHandler> pathHandlers;
//std::unique_ptr<CDatabaseObject> dbptr;

HTTPRequest::HTTPRequest(struct evhttp_request* _req) : req(_req){}
HTTPRequest::~HTTPRequest()
{
    LOG(INFO) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"  ;
}

HTTPRequest::RequestMethod HTTPRequest::GetRequestMethod()
{
    switch (evhttp_request_get_command(req)) {
    case EVHTTP_REQ_GET:
        return GET;
        break;
    case EVHTTP_REQ_POST:
        return POST;
        break;
    case EVHTTP_REQ_HEAD:
        return HEAD;
        break;
    case EVHTTP_REQ_PUT:
        return PUT;
        break;
	case EVHTTP_REQ_OPTIONS:
        return OPTIONS;
        break;
    default:
        return UNKNOWN;
        break;
    }
}

static std::string RequestMethodString(HTTPRequest::RequestMethod m)
{
    switch (m) {
    case HTTPRequest::GET:
        return "GET";
        break;
    case HTTPRequest::POST:
        return "POST";
        break;
    case HTTPRequest::HEAD:
        return "HEAD";
        break;
    case HTTPRequest::PUT:
        return "PUT";
        break;
    case HTTPRequest::OPTIONS:
        return "OPTIONS";
        break;
    default:
        return "unknown";
    }
}

void registerHTTPHandler(const std::string &prefix, const HTTPRequestHandler &handler)
{
    LOG(INFO) << "Registering HTTP handler for " << prefix;

    pathHandlers.push_back(HTTPPathHandler(prefix, handler));
}

std::string HTTPRequest::GetURI()
{
    return evhttp_request_get_uri(req);
}
std::string HTTPRequest::GetHeader()
{
    std::string urlheader;
    struct evkeyvalq *headers;
    struct evkeyval *header;
    headers = evhttp_request_get_input_headers(req);

    for (header = headers->tqh_first; header;header = header->next.tqe_next)
    {
        urlheader = urlheader + header->key + " : " + header->value + "\n";
    }

    return urlheader;
}
void HTTPRequest::WriteHeader(const std::string& hdr, const std::string& value)
{
    struct evkeyvalq* headers = evhttp_request_get_output_headers(req);
    assert(headers);
    evhttp_add_header(headers, hdr.c_str(), value.c_str());
}

void HTTPRequest::WriteReply(int nStatus, const std::string& strReply)
{
    assert(req);
    struct evbuffer* evb = evhttp_request_get_output_buffer(req);
    assert(evb);
    evbuffer_add(evb, strReply.data(), strReply.size());
    auto req_copy = req;

    evhttp_send_reply(req_copy, nStatus, nullptr, nullptr);
    // Re-enable reading from the socket. This is the second part of the libevent
    // workaround above.
    if (event_get_version_number() >= 0x02010600 && event_get_version_number() < 0x02020001)
    {
       evhttp_connection* conn = evhttp_request_get_connection(req_copy);
       if (conn)
       {
           bufferevent* bev = evhttp_connection_get_bufferevent(conn);
           if (bev)
           {
               bufferevent_enable(bev, EV_READ | EV_WRITE);
           }
       }
    }
}

std::string HTTPRequest::ReadBody()
{
    struct evbuffer* buf = evhttp_request_get_input_buffer(req);
    if (!buf)
    {
        LOG(INFO) << "READ_BODY ERROR 1";
        return "";
    }

    size_t size = evbuffer_get_length(buf);

    const char* data = (const char*)evbuffer_pullup(buf, size);
    if (!data)
    {
        LOG(INFO) << "READ_BODY ERROR 2   " << size;
        return "";
    }
    std::string rv(data, size);
    evbuffer_drain(buf, size);

    LOG(INFO) << "READ_BODY : " << rv;
    return rv;
}
//bool checkHash(const std::string &txid)
//{
//    return isHex(txid) && HAHS_SIZE == txid.length();
//}

//void httpRequestCb(struct evhttp_request *req, void *arg)
//{
//
//    if (event_get_version_number() >= 0x02010600 && event_get_version_number() < 0x02020001)
//    {
//        evhttp_connection* conn = evhttp_request_get_connection(req);
//        if (conn)
//        {
//            bufferevent* bev = evhttp_connection_get_bufferevent(conn);
//            if (bev)
//            {
//                bufferevent_disable(bev, EV_READ);
//            }
//        }
//    }
//
//    std::unique_ptr<HTTPRequest> hreq(new HTTPRequest(req));
//
//    LOG(INFO) << "Received a " <<  RequestMethodString(hreq->GetRequestMethod()) << " request for " <<  hreq->GetURI() << " from "; //<<  hreq->GetPeer().ToString();
//    LOG(INFO) << "HEADER : " << hreq->GetHeader();
////    LOG(INFO) << "BODY : "<< hreq->ReadBody();
//
//    if (hreq->GetRequestMethod() == HTTPRequest::UNKNOWN)
//    {
//        hreq->WriteReply(HTTP_BADMETHOD);
//        return;
//    }
//
//	if (hreq->GetRequestMethod() == HTTPRequest::OPTIONS)
//    {
//
//		hreq->WriteHeader("Access-Control-Allow-Origin", "*");
//		hreq->WriteHeader("Access-Control-Allow-Credentials", "true");
//		hreq->WriteHeader("Access-Control-Allow-Headers", "access-control-allow-origin,Origin, X-Requested-With, Content-Type, Accept, Authorization");
//		hreq->WriteReply(HTTP_OK);
//        return ;
//	}
//
//    if (hreq->GetRequestMethod() != HTTPRequest::POST)
//    {
//        hreq->WriteReply(HTTP_BADMETHOD);
//        return;
//    }
//
//
//    hreq->WriteHeader("Access-Control-Allow-Origin", "*");
//    hreq->WriteHeader("Access-Control-Allow-Credentials", "true");
//    hreq->WriteHeader("Access-Control-Allow-Headers", "access-control-allow-origin,Origin, X-Requested-With, Content-Type, Accept, Authorization");
//
//
//    std::string strURI = hreq->GetURI();
//    std::string path;
//    std::vector<HTTPPathHandler>::const_iterator i = pathHandlers.begin();
//    std::vector<HTTPPathHandler>::const_iterator iend = pathHandlers.end();
//    for (; i != iend; ++i)
//    {
//        bool match = (strURI == i->prefix);
//        if (match)
//        {
//            path = strURI;
//            break;
//        }
//    }
//
//    if(i != iend)
//    {
//        LOG(INFO) << "FOUND_PATH : " << path;
//        i->handler(std::move(hreq));
//    }
//    else
//    {
//        LOG(INFO) << "NOT_FOUND_PATH : " <<  strURI;
//        hreq->WriteReply(HTTP_NOTFOUND);
//    }
//}



//void parseGetTableTxs(std::unique_ptr<HTTPRequest> req)
//{
//    do {
//        try
//        {
//            std::string post_data = req->ReadBody();
//            auto jsonData = json::parse(post_data);
//
//            if(!jsonData.is_object())
//            {
//                LOG(ERROR) << "PARSE_JSON_ERROR" ;
//                break;
//            }
//
//            std::string tableid = jsonData["tableid"].get<std::string>();
//            LOG(INFO) << "TABLE_ID : " << tableid;
//            if(!checkHash(tableid))
//            {
//                LOG(ERROR) << "TABLE_ID" ;
//                break;
//            }
//            std::string dbres;
//            bool ret = dbptr->selecGameTx(tableid,dbres);
//            if(!ret)
//            {
//                dbres = "not found match tableid tx";
//            }
//            LOG(INFO) << "DB_RES : " << dbres;
//
//            std::string response = makeReplyMsg(ret,dbres);
//            req->WriteHeader("Content-Type", "application/json");
//            req->WriteReply(HTTP_OK,response);
//            return ;
//        }
//        catch(...)
//        {
//
//            break;
//        }
//    } while(0);
//
//    req->WriteReply(HTTP_INTERNAL,ERROR_REQUEST);
//    return;
//}

void httpRequestCb(struct evhttp_request *req, void *arg)
{

	if (event_get_version_number() >= 0x02010600 && event_get_version_number() < 0x02020001)
	{
		evhttp_connection* conn = evhttp_request_get_connection(req);
		if (conn)
		{
			bufferevent* bev = evhttp_connection_get_bufferevent(conn);
			if (bev)
			{
				bufferevent_disable(bev, EV_READ);
			}
		}
	}

	std::unique_ptr<HTTPRequest> hreq(new HTTPRequest(req));

	LOG(INFO) << "Received a " << RequestMethodString(hreq->GetRequestMethod()) << " request for " << hreq->GetURI() << " from "; //<<  hreq->GetPeer().ToString();
	LOG(INFO) << "HEADER : " << hreq->GetHeader();
	//    LOG(INFO) << "BODY : "<< hreq->ReadBody();

	if (hreq->GetRequestMethod() == HTTPRequest::UNKNOWN)
	{
		hreq->WriteReply(HTTP_BADMETHOD);
		return;
	}

	if (hreq->GetRequestMethod() == HTTPRequest::OPTIONS)
	{

		hreq->WriteHeader("Access-Control-Allow-Origin", "*");
		hreq->WriteHeader("Access-Control-Allow-Credentials", "true");
		hreq->WriteHeader("Access-Control-Allow-Headers", "access-control-allow-origin,Origin, X-Requested-With, Content-Type, Accept, Authorization");
		hreq->WriteReply(HTTP_OK);
		return;
	}

	if (hreq->GetRequestMethod() != HTTPRequest::POST)
	{
		hreq->WriteReply(HTTP_BADMETHOD);
		return;
	}


	hreq->WriteHeader("Access-Control-Allow-Origin", "*");
	hreq->WriteHeader("Access-Control-Allow-Credentials", "true");
	hreq->WriteHeader("Access-Control-Allow-Headers", "access-control-allow-origin,Origin, X-Requested-With, Content-Type, Accept, Authorization");


	std::string strURI = hreq->GetURI();
	std::string path;
	std::vector<HTTPPathHandler>::const_iterator i = pathHandlers.begin();
	std::vector<HTTPPathHandler>::const_iterator iend = pathHandlers.end();
	for (; i != iend; ++i)
	{
		bool match = (strURI == i->prefix);
		if (match)
		{
			path = strURI;
			break;
		}
	}

	if (i != iend)
	{
		LOG(INFO) << "FOUND_PATH : " << path;
		i->handler(std::move(hreq));
	}
	else
	{
		LOG(INFO) << "NOT_FOUND_PATH : " << strURI;
		hreq->WriteReply(HTTP_NOTFOUND);
	}
}

void GetBasicInformation(std::unique_ptr<HTTPRequest>  req)
{
	do {
		try
		{
		//	std::string response;
			std::string blockcount;
			std::string blockhash;
			std::string blockinfo;
			std::string txInfo;
			std::string balance;


			getBalance(balance);//balance
			auto Balance = json::parse(balance);
            int token=Balance["result"].get<int>();
            LOG(INFO) << "//////////////////////////////// : " << token;
			getBlockCount(blockcount);
			LOG(INFO) << "RESPONSEqqqq : " << blockcount;
			LOG(INFO)  << "------------------------------------";


           auto jsonDataBlockHash = json::parse(blockcount);
            int num = jsonDataBlockHash["result"].get<int>();
            
			LOG(INFO) << "RESPONSEnum : " << num;
			getBlockHash(std::to_string(num),blockhash);
			LOG(INFO) << "RESPONSEbbbb : " << blockhash;
			LOG(INFO)  << "------------------------------------------";
 //           auto jsonDataBlockHash = json::parse(blockinfo);
            auto jsonDataBlock = json::parse(blockhash);
            nlohmann::json hashBlock =nlohmann::json::object();
            hashBlock= jsonDataBlock["result"];
			LOG(INFO) << "aaaaa : " <<hashBlock.dump();
			getBlock(hashBlock,blockinfo);
			LOG(INFO) << "RESPONSEdddddd : " <<blockinfo;

			LOG(INFO)  << "-------------------------------------";
			//解block信息
            auto jsonData = json::parse(blockinfo);
			LOG(INFO)<<"json ::"  <<jsonData;
            nlohmann::json bodys =nlohmann::json::object();
            bodys= jsonData["result"];
			LOG(INFO) << "bbbbbbbb: " <<bodys.dump();
			
                
//			auto jsondata = json::parse(jsonData);
		//	std::string body = jsonData["result"];
		//	LOG(INFO)<<"body ::"  <<body;
		//	auto bodyJson = json::parse(body);
            int Txtime = bodys["time"].get<int>();
			LOG(INFO)<<"time ::"  << Txtime;
			LOG(INFO)  << "-------------------------------------";
            //std::string iTxHash = bodys["tx"].get<std::string>();
		//	LOG(INFO)<<"cccccccccccc ::"  << iTxHash;

            nlohmann::json iTxHash =nlohmann::json::array();

		//	LOG(INFO)  << "+++++++++++++++++++++++++++++++";
            iTxHash= bodys["tx"];
		//	LOG(INFO)  << "+++++++++++++++++++++++++++++++";
			LOG(INFO) << "ccccccc: " <<iTxHash.dump();
			LOG(INFO) << "size: " <<iTxHash.size();
			LOG(INFO)  << "-------------------------------------";
            std::vector<std::string> TxHash;
            for (json::iterator it = iTxHash.begin(); it != iTxHash.end(); ++it) {
    			LOG(INFO) << "cdddddddd: " <<(*it).dump();
                TxHash.push_back(*it);
            }
                std::vector<std::string> TxInformation;//tx信息
                std::string txInformation;
                int txcount = 0;

            for (std::vector<std::string>::iterator it = TxHash.begin(); it != TxHash.end(); it++)
            {
                txcount++;
                getRawtransaction(*it, txInformation);
                TxInformation.push_back(txInformation);
            }
    			LOG(INFO) << "eeeeeeeeeeeeee: " <<txcount;
			LOG(INFO)  << "-------------------------------------";
            nlohmann::json zxsw = nlohmann::json::object();
            for (std::vector<std::string>::iterator it = TxInformation.begin(); it != TxInformation.end(); it++)
            {
                                            // txInfo = (*it) + ":";
               auto temp = json::parse(*it);
                
    			LOG(INFO) << "fffffffffffffffffff: " << temp;
                auto data= temp["result"]; 
    			LOG(INFO) << "ffaaaaaaaaaaaaaaaaaf: " << data;
                auto aaa=data["time"];
    			LOG(INFO) << "ggggggggggggggg: " << aaa;

           //     auto temp = json::parse(body);

    		//	LOG(INFO) << "fffffffffffffffffff: " <<*it;
    		//	LOG(INFO) << "ffaaaaaaaaaaaaaaaaaa: " <<typeid(*it).name();
    		//	LOG(INFO) << "fffffffffffffffffff: " <<temp["hash"];
                zxsw["swhx"] = data["hash"];
                zxsw["scsj"] = data["blocktime"];
                zxsw["skzh"] = data["addresses"];
                
            }
    		LOG(INFO) << "aaaaaaaaaaaa: " <<zxsw.dump();

            nlohmann::json nodes = nlohmann::json::object();
            nodes["lszbgd"] = 1;
            nodes["nhgd"] = num;
    		LOG(INFO) << "bbbbbbbbbbbbbb: " <<nodes.dump();

            nlohmann::json js = nlohmann::json::object();
            js["scsj"] =Txtime;
            js["sw"] = txcount;
            js["gd"] = num;
    		LOG(INFO) << "ccccccccc: " <<js.dump();


            nlohmann::json info = nlohmann::json::object();
            info["zxqksj"] = Txtime;
            info["zxqk"] = txcount;
            info["jczc"] = token;
            info["xybb"] = "1";
    		LOG(INFO) << "ddddddddddddddd: " <<info.dump();

            nlohmann::json response = nlohmann::json::object();
            response["errorCode"] = "0";
            response["errorDesc"] = "成功";
            response["elapsedMilliseconds"] = 0;
            response["token"] = "btc";
            response["success"] = true;
            response["nodes"] = nodes;
            response["js"] = js;
            response["zxsw"] = zxsw;
            response["info"] = info;
            nlohmann::json peer = nlohmann::json::array();
            peer.push_back("47.88.154.211");
            peer.push_back("47.75.103.81");
    		LOG(INFO) << "eeeeeeeeeeee: " <<response;
            response["peer"] = peer;

			LOG(INFO) << "RESPONSE : " << response.size();
			req->WriteHeader("Content-Type", "application/json");

			req->WriteReply(HTTP_OK, response.dump());
            
			return;
		}
		catch (...)
		{
			break;
		}
	} while (0);

	req->WriteReply(HTTP_INTERNAL, ERROR_REQUEST);
	return;
}
bool getBalance(const std::string &response)
{
	std::string rpcData = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getbalance\", \"params\": [] }";
	return curlBitcoinReq(rpcData, response);
}
bool getBlockCount(const std::string &blockcount)
{
	std::string rpcData = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getblockcount\", \"params\": [] }";
	return curlBitcoinReq(rpcData, blockcount);
}
bool getBlockHash(const std::string& blockcount, const std::string &blockhash)
{
	std::string rpcData = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getblockhash\", \"params\": [" + blockcount + "] }";
	return curlBitcoinReq(rpcData, blockhash);
}
bool getBlock(const std::string& blockhash, const std::string &blockinfo)
{
	std::string rpcData = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getblock\", \"params\": [\"" + blockhash + "\"] }";
	return curlBitcoinReq(rpcData, blockinfo);
}
bool getRawtransaction(const std::string& txHash, const std::string &txInformation)
{
	std::string rpcData = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getrawtransaction\", \"params\": [\"" + txHash + "\",1] }";
	return curlBitcoinReq(rpcData, txInformation);
}
size_t reqReply(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::string *str = (std::string*)stream;
	(*str).append((char*)ptr, size*nmemb);
	return size * nmemb;
}
bool curlBitcoinReq(const std::string &data, const std::string &response)
{
	LOG(INFO) << "CURL_data : " << data;
	CURL *curl = curl_easy_init();
	struct curl_slist *headers = NULL;
	CURLcode res;

	const std::string url = "http://127.0.0.1:8332";

	if (curl)
	{
		headers = curl_slist_append(headers, "content-type: text/plain;");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)data.size());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, reqReply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

		curl_easy_setopt(curl, CURLOPT_USERPWD, "taoist:marongxin");
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		res = curl_easy_perform(curl);
	}
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
	{
		LOG(ERROR) << "CURL_FAILED : " << curl_easy_strerror(res);
		return false;
	}
	LOG(INFO) << "CURL_RESULT : " << response;

	return true;
}



const signed char p_util_hexdigit[256] =
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, };

signed char hexDigit(char c)
{
    return p_util_hexdigit[(unsigned char)c];
}

bool isHex(const std::string& str)
{
    for(std::string::const_iterator it(str.begin()); it != str.end(); ++it)
    {
        if (hexDigit(*it) < 0)
            return false;
    }
    return (str.size() > 0) && (str.size()%2 == 0);
}

void signalHandler(int sig)
{
    switch (sig)
    {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
        {
            event_loopbreak();
        }
        break;
    }
}

void runDaemon(bool daemon)
{
    if (daemon) {
        pid_t pid;
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
    }
}
