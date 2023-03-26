#include "charserver.h"
#include "chatservice.h"
#include <iostream>
#include <signal.h>
using namespace std;

// 处理服务器ctrl+c 结束后，重置user的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}
int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6002);
    ChatServer server(&loop, addr, "ChatServer");

	
    signal(SIGINT, resetHandler);
	
    server.Start();
    loop.loop();

    return 0;
}
