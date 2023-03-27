#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "friendmodel.h"
#include "groupmodel.h"
#include "json.hpp"
#include "offlinemessagemodel.h"
#include "redis.h"
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
    // 单例对象的接口
    static ChatService* instance();
    // 登录业务
    void login(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 注册业务
    void reg(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 一对一聊天
    void oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 添加好友
    void addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 创建群组业务
    void createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 获取消息对应的处理函数设置
    MsgHandler getHandler(int msgid);
    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr& conn);
    // 服务器异常，业务重置方法
    void reset();
    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);

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
    FriendModel _friendModel;
    GroupModel _groupModel;
    // redis操作对象
    Redis _redis;
};

#endif