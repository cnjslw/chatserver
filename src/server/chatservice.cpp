#include "chatservice.h"
#include "public.h"
#include <muduo/base/Logging.h>
#include <string>
#include <vector>

using namespace std;
using namespace muduo;
using namespace muduo::net;

ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msgHandlerMap.insert({ REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3) });//注册事件
    _msgHandlerMap.insert({ LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3) });//登录事件
    _msgHandlerMap.insert({ ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3) });//聊天事件
}

void ChatService::reset()
{
    _userModel.resetState();
}

// 根据JSON的msgid键值对，从_msgHandlerMap获取对应的回调函数
//e.g. : {"msgid":3,"name":"xxx","password":"123456"}
/*enum EnMsgType {
    LOGIN_MSG = 1, // 登录消息
    LOGIN_MSG_ACK, // 登录响应消息
    REG_MSG, // 注册消息             3号事件
    REG_MSG_ACK, // 注册响应消息
    ONE_CHAT_MSG, // 聊天消息
};*/
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) {
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp) { LOG_ERROR << "msgid: " << msgid << " can not find handler"; };
    } else {
        return _msgHandlerMap[msgid];
    }
}

// 处理登录业务
//e.g. {"msgid":1,"id":1"password":"123"}
void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    int id = js["id"];
    string pwd = js["password"];

    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == pwd) {
        // 账户存在，密码正确
        if (user.getState() == "online") {
            // 不允许重复登陆
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账户已经登录，请重新输入新账号";
            conn->send(response.dump());
        } else {
            // 登录成功,记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({ id, conn });
            }

            // 更新用户状态信息
            user.setState("online");
            _userModel.update(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 登录成功以后，查询用户是否有离线消息需要接受
            vector<string> svec = _offlineMsgModel.query(id);
            if (!svec.empty()) {
                response["offlinemsg"] = svec;
                _offlineMsgModel.remove(id);
            }
            conn->send(response.dump());
        }

    } else {
        // 登录失败情况1：用户存在，但是密码不正确。情况2：用户不存在。
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
}

// 处理注册业务
// msgHandler(conn, js, time);
//e.g. : {"msgid":3,"name":"xxx","password":"123456"}
void ChatService::reg(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);

    bool state = _userModel.insert(user);
    if (state) {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } else {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

// 处理用户异常退出
void ChatService::clientCloseException(const TcpConnectionPtr& conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);

        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++) {
            if (it->second == conn) {
                // 删除用户连接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 更新用户状态
    if (user.getId() != -1) {
        user.setState("offline");
        _userModel.update(user);
    }
}

// 一对一聊天
void ChatService::oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time)
{

    int toid = js["to"].get<int>();

    // 标识用户是否在线
    bool userState = false;
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end()) {
            // toid在线转发消息
            it->second->send(js.dump());
            return;
        }
    }

    // 如果离线，则存储消息，并在目标用户上线后发送
    _offlineMsgModel.insert(toid, js.dump());
}
