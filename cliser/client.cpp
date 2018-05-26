#include "client.h"


Client::Client()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000);//服务器的端口
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");//服务器的IP


	int res = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
	if (res == -1)
	{
		cout << "connect failure!" <<endl;
		exit(0);
	}

}




int  Client::loginMode()
{
	LoginView login;
	return login.run(sockfd);
}
void Client::RegisterMode()
{
	RView rgiser;
	rgiser.run(sockfd);
}
int Client::ExitMode()
{
	Exitview Eview;
	return Eview.run(sockfd);
}
void Client::run()
{
	while(1)
	{
		int i = 0;
		int _select = 0;
		cout << "***********ChatSysterm**********" << endl;
		cout << "1.login    2.regiser    3.exit " << endl;
		cout << "********************************" << endl;
		while (1)
		{
			fflush(stdin);
			fflush(stdout);
			cout << "Please enter your selection:";
			cin >> _select;
			cout << endl;
			if (_select < 1 || _select > 3)
				continue;
			else break;
		}
		switch (_select)
		{
		case 1:
			i = loginMode();
			break;
		case 2:
			RegisterMode();
			break;
		case 3:
			i = ExitMode();
			break;
		default:
			   break;
		}
		if(i == 1)
			break;
	}
}
