#ifndef CHATVIEW_H
#define CHATVIEW_H
#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <json/json.h>
#include <time.h>
#include "public.h"
#include <queue>
#include <utility>
#include <semaphore.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>
using namespace std;

class ChatView
{
public:
	void run(int fd);
	void fun(int fd);
};
#endif
