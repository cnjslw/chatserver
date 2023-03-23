#include "chatservice.h"
#include "public.h"
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}
void ChatService::login(const TcpConnection& conn, json& js, Timestamp time) { }
void ChatService::reg(const TcpConnection& conn, json& js, Timestamp time) { }
ChatService::ChatService()
{
    _msgHandlerMap.insert({ REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3) });
    _msgHandlerMap.insert({ LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3) });
}
