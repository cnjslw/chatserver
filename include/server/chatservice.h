#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "json.hpp"
#include "offlinemessagemodel.h"
#include "usermodel.h"
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <mutex>
#include <unordered_map>

using json = nlohmann::json;

using namespace std;
using namespace muduo;
using namespace muduo::net;
using MsgHandler = std::function<void(const TcpConnectionPtr& conn, json& js, Timestamp)>;

class ChatService {
public:
    static ChatService* instance();
    void login(const TcpConnectionPtr& conn, json& js, Timestamp time);
    void reg(const TcpConnectionPtr& conn, json& js, Timestamp time);
    void oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time);

    MsgHandler getHandler(int msgid);
    void clientCloseException(const TcpConnectionPtr& conn);
	void reset();

private:
    ChatService();

    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 定义互斥锁，保证userConnMap安全
    mutex _connMutex;
    // 数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
};

#endif