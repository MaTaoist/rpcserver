#include <iostream>
#include "json.hpp"

using json = nlohmann::json;
template<class T>
std::string makeReplyMsg(bool type,T &t)
{
    json response = json::object();

    response["code"] = type ? 1 : 0;
    response["msg"] = "";
    response["data"] =  t;
    std::cout  << "REPLY_MSG : " << response.dump(4) << std::endl;
    return  response.dump(4);

}


int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;
    json js;
    js["hello"]="world";
    js["你好"]="世界";

    json jArray;
    jArray.push_back("hello world");
    jArray.push_back("你好 世界");
    jArray.push_back(123456);

    js["array"] = jArray;

    auto j3 = json::parse("{ \"happy\": true, \"pi\": 3.141  }");
    auto j4 = json::parse("{ \"你好\": true, \"世界\": 3.141  }");

    std::cout << js.dump(4) << std::endl;
    std::cout << std::endl;
    std::cout << jArray.dump(4) << std::endl;
    std::cout << j3.dump(4) << std::endl;
    std::cout << j4.dump(4) << std::endl;
    makeReplyMsg(true,"hello world");
    makeReplyMsg(true,js);

    return 0;
}
