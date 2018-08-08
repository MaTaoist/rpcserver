#include "server.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{

    el::Configurations conf("./conf/server_log.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    LOG(INFO) << "---  start server  ---";

    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);

    // read conf file ?

    std::string httpd_option_listen = "0.0.0.0";
    int httpd_option_port = 8080;
    int httpd_option_daemon = 0;
    int httpd_option_timeout = 30; 

    event_init();

    struct evhttp *httpd;

    ////db ?
    //assert(!dbptr);
    //dbptr = std::unique_ptr<CDatabaseObject>(new CDatabaseObject());
    //assert(dbptr);
    //dbptr->openDB("poker.db",true);



    // conf file ?
 
	registerHTTPHandler("/GetBasicInformation", GetBasicInformation);
	

    httpd = evhttp_start(httpd_option_listen.c_str(), httpd_option_port);

    evhttp_set_allowed_methods(httpd, EVHTTP_REQ_GET | EVHTTP_REQ_POST |  EVHTTP_REQ_HEAD | EVHTTP_REQ_PUT | EVHTTP_REQ_OPTIONS);
    evhttp_set_timeout(httpd, httpd_option_timeout);
    evhttp_set_gencb(httpd, httpRequestCb, nullptr);

    event_dispatch();

    evhttp_free(httpd);

    LOG(INFO)  << "---  stop server  ---";
    return 0;
}
