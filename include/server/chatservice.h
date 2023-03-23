#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "json.hpp"
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>

using json = nlohmann::json;

using namespace std;
using namespace muduo;
using namespace muduo::net;
using MsgHandler = std::function<void(const TcpConnection& conn, json& js,Timestamp)>;

class ChatService {
public:
    static ChatService* instance();
    void login(const TcpConnection& conn, json& js, Timestamp time);
    void reg(const TcpConnection& conn, json& js, Timestamp time);

private:
    ChatService();

    unordered_map<int, MsgHandler> _msgHandlerMap;
};

#endif