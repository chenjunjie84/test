#include <iostream>
#include "json/json.h"
#include <string>
#include <string.h>
#include "public.h"
#include "view.h"
#include "mysqlserver.h"
#include<semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

class  FriendView:public View
{
public:
void process(Json::Value &root);
void response();
};
class GroupView:public View
{
public:
void process(Json::Value &root);
void response();
};
