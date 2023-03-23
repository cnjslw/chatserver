#include "charserver.h"
#include <iostream>
using namespace std;

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6002);
    ChatServer server(&loop, addr, "ChatServer");

    server.Start();
    loop.loop();

    return 0;
}