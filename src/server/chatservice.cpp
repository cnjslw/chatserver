#include "chatservice.h"
#include "public.h"
#include <muduo/base/Logging.h>
#include <string>
using namespace std;
using namespace muduo;

ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msgHandlerMap.insert({ REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3) });
    _msgHandlerMap.insert({ LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3) });
}

void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time) { }
void ChatService::reg(const TcpConnectionPtr& conn, json& js, Timestamp time) { }

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) {
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp) { LOG_ERROR << "msgid: " << msgid << " can not find handler"; };
    } else {
        return _msgHandlerMap[msgid];
    }
    return _msgHandlerMap[msgid];
}
