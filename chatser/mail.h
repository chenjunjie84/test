#ifndef MAIL_H
#define MAIL_H
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
using namespace std;

#define SOCKET_ERROR -1
typedef int SOCKET;

class CSendEmail
{
public:
    //���������smtp��������ַ
    void setSmtpServer(string server);
    //����������û���
    void setAuthName(string name);
    //�������������
    void setAuthPwd(string pwd);
    //����Ŀ���ʼ���ַ
    void setDestEmail(string email);
    //���÷����ʼ����ı���Ϣ
    void setEmailText(string text);
    //�����ʼ�
    bool sendEmail();
private:
    string smtpServer;
    string username;
    string pwd;
    string destEmail;
    string sendText;
};

#endif
