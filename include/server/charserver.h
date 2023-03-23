#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer {
public:
    ChatServer(EventLoop* loop,
        const InetAddress& listenAddr,
        const string& nameArg);

    // 启动服务
    void Start();

private:
    // 上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr& conn);

    // 上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr& conn,
        Buffer* buffer,
        Timestamp time);

    TcpServer _server;
    EventLoop* _loop; // 指向循环对象的指针
};

#endif