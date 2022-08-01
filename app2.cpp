#include"tools.h"
const char* welcome = "please enter your name: ";
struct socketinfo{
	SOCKET sockfd;
	sockaddr_in addr;
};
struct msgInfo {
	socketinfo* from;
	string username;
	string msgdata;
};
char revBuffer[255] = {};
queue<msgInfo> msg; //接收到的信息
vector<int> clients; //已连接的客户端信息


//向其他用户分发服务器收到的信息
void* sendmsgToanthorClient(void* _socketinfo) {
	while (true) {
		if (!msg.empty())
		{
			msgInfo minfo = msg.front();
			msg.pop();
			socketinfo* _from = minfo.from;
			memset(revBuffer, 0, 255);
			string _msg;
			_msg.append(minfo.username);
			_msg.append(": ");
			_msg.append(minfo.msgdata);
			strcpy(revBuffer, _msg.c_str());
			for (auto client : clients) {
				if (client != _from->sockfd)
				{
					send(client, revBuffer, 255, 0);
				}
			}
		}
	}
}
int main()
{
	initWinsock();
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		cout << "socket error !" << endl;
		return 0;
	}

	//绑定IP和端口
	sockaddr_in sin;//ipv4的指定方法是使用struct sockaddr_in类型的变量
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);//设置端口。htons将主机的unsigned short int转换为网络字节顺序
	sin.sin_addr.S_un.S_addr = INADDR_ANY;//IP地址设置成INADDR_ANY，让系统自动获取本机的IP地址
	//bind函数把一个地址族中的特定地址赋给scket。
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		cout << "listen error !" << endl;
		return -1;
	}

	//循环接收数据
	SOCKET sclient;
	sockaddr_in remoteAddr;//sockaddr_in常用于socket定义和赋值,sockaddr用于函数参数
	int nAddrlen = sizeof(remoteAddr);
	pthread_t _pt;
	pthread_create(&_pt, NULL, sendmsgToanthorClient, NULL);
	while (true)
	{
		sclient = accept(slisten, (sockaddr*)&remoteAddr, &nAddrlen);
		if (sclient == INVALID_SOCKET)
		{
			cout << "accept error !" << endl;
			continue;
		}
		//cout << "接收到一个连接：" << inet_ntoa(remoteAddr.sin_addr) << endl;
		socketinfo* info = new socketinfo{
			sclient,remoteAddr
		};

		//printf("msg was id: %d \n", info->sockfd);
		pthread_t pt;
		clients.push_back(info->sockfd);
		pthread_create(&pt, NULL, [](void* sc)->void* {

			char revData[BUFFER_MAX_SIZE];
			memset(revData, 0, BUFFER_MAX_SIZE);
			socketinfo* info = (socketinfo*)sc;
			pthread_t this_pt = pthread_self();
			int sclient = info->sockfd;

			send(info->sockfd, welcome, strlen(welcome), 0);
			int ret;
			//接收用户名
			ret = recv(sclient, revData, BUFFER_MAX_SIZE, 0);
			revData[ret - 1] = '\0';
			string _username = revData;
			wstring username = Utf82Unicode(_username);
			wcout << "welcome: " << username <<" and your id is :"<<sclient << endl;
			memset(revData, 0, BUFFER_MAX_SIZE);
			//接收信息
			while (ret = recv(sclient, revData, BUFFER_MAX_SIZE, 0))
			{
				string _str(revData, ret);
				wstring str(Utf82Unicode(_str));
				//分发信息到服务器
				msg.push(msgInfo{
					info,_username,_str
					});
				//printf("msg id: %d \n",info->sockfd);
				if (!strcmp(revData, "quit\n"))
					break;
				wcout << username << ": " << str;
				memset(revData, 0, BUFFER_MAX_SIZE);
			}
			closesocket(sclient);
			return nullptr;
			}, info);
	}
	closesocket(slisten);
	WSACleanup();
}
