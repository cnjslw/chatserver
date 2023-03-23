#include <functional>
#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

class ChatServer {
public:
    ChatServer(EventLoop* loop, // 事件循环
        const InetAddress& listenAddr, // IP+Port
        const string& nameArg) // 服务器的名字
        : _server(loop, listenAddr, nameArg)
        , _loop(loop)
    {
        // 创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
        // 读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        // 设置服务器端的线程数量
        _server.setThreadNum(4);
    }

    void Start()
    {
        _server.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected()) {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << endl;
        }
    }
    void onMessage(const TcpConnectionPtr& conn, // 连接
        Buffer* buffer, // 缓冲区
        Timestamp time) // 接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time" << time.toString() << endl;
        conn->send(buf);
    }
    TcpServer _server;
    EventLoop* _loop;
};

int main()
{
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.Start();
    loop.loop();

    return 0;
}