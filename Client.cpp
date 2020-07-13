#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<tchar.h>
#include<stdio.h>
#include<winsock2.h>
#include<stdlib.h>  
#include<string.h>  
#include<Ws2tcpip.h>
#include<iostream>
#include<thread>
#include<vector>
#include<graphics.h>
#include<string>
#include<fstream>
#pragma comment(lib,"ws2_32.lib")
char name[16];
#define BUFFER_SIZE 2048
#define FILE_NAME_MAX_SIZE 512 
const std::string cache_dir = "cache.txt";
const std::string payimg_dir = "alipay.jpg";
int password();
int file();
int chat();
int __main__();
std::string readCache(std::string file);
void saveCache(std::string file, std::string str);

void saveCache(std::string file, std::string str)
{
	std::ofstream os;     //创建一个文件输出流对象
	os.open(file.data());//将对象与文件关联
	os << str;   //将输入的内容放入txt文件中
	os.close();
}
std::string readCache(std::string file)
{
	std::ifstream f;
	f.open(file.data());   //将文件流对象与文件连接起来 

	std::string s;
	while (getline(f, s))
	{
		//std::cout << s << std::endl;
		continue;
	}
	f.close();             //关闭文件输入流 
	return s;
}

int file()
{
	/*
	初始化WSA，使得程序可以调用windows socket
	*/
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	/*
	创建监听用套接字，server_socket
	并检测是否创建成功
	*/
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); ;
	if (client_socket == INVALID_SOCKET) {
		//如果创建的socket无效，则结束程序
		perror("socket error !");
		return 0;
	}

	/*
	创建地址结构，server_addr，并设置端口和IP
	*/
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//要连接的服务器端口号 8888
	server_addr.sin_port = htons(6666);
	//指定服务器的地址127.0.0.1
	//InetPton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
	InetPton(AF_INET, "47.93.38.43", &server_addr.sin_addr.s_addr);
	//与地址server_addr建立连接
	if (connect(client_socket, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)))
	{
		perror("connect error !\n");
		return 0;
	}
	
	char file_name[BUFFER_SIZE];
	memset(file_name, 0, BUFFER_SIZE);
	if (recv(client_socket, file_name, sizeof(file_name), 0) > 0)
	{
		std::cout << "successfully connect" << std::endl;
		//std::cout << file_name << std::endl;
	}
	else
	{
		std::cout << "文件列表空！" << std::endl;
		closesocket(client_socket);
		WSACleanup();
		exit(0);
	}
	std::cout << "文件列表:" << std::endl;
	for (int i = 0; i < sizeof(file_name); ++i)
	{
		if (file_name[i] == ',')
		{
			std::cout << std::endl;
			continue;
		}
		std::cout << file_name[i];
	}

	char REMOTE_file_name[FILE_NAME_MAX_SIZE + 1];
	memset(REMOTE_file_name, 0, FILE_NAME_MAX_SIZE + 1);
	printf("请输入要获取的服务器文件名:\n");
	scanf_s("%s", REMOTE_file_name, FILE_NAME_MAX_SIZE);

	char LOCAL_file_name[FILE_NAME_MAX_SIZE + 1];
	memset(LOCAL_file_name, 0, FILE_NAME_MAX_SIZE + 1);
	printf("请输入保存文件的本地路径:\n");
	scanf_s("%s", LOCAL_file_name, FILE_NAME_MAX_SIZE);

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	strncpy_s(buffer, REMOTE_file_name, strlen(REMOTE_file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(REMOTE_file_name));

	// 向服务器发送buffer中的数据 
	if (send(client_socket, buffer, sizeof(buffer), 0) < 0)
	{
		perror("发送文件名失败:");
		closesocket(client_socket);
		WSACleanup();
		system("pause");
		__main__();
		exit(1);
	}
	// 打开文件，准备写入 

	FILE* fp;
	errno_t F_ERR = fopen_s(&fp, LOCAL_file_name, "wb");
	if (F_ERR != 0)
	{
		printf("文件打开失败：%s/n", LOCAL_file_name);
		closesocket(client_socket);
		WSACleanup();
		system("pause");
		__main__();
		exit(1);
	}

	// 从服务器接收数据到buffer中 
	// 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
	memset(buffer, 0, BUFFER_SIZE);
	int length = 0;
	while ((length = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
	{
		if (fwrite(buffer, sizeof(char), length, fp) < length)
		{
			printf("文件写入失败：%s/n", LOCAL_file_name);
			__main__();
			break;
		}
		memset(buffer, 0, BUFFER_SIZE);
	}
	std::cout << "成功从服务器接收文件" << REMOTE_file_name << "存入本地目录" << LOCAL_file_name << std::endl;
	// 接收成功后，关闭文件，关闭socket、WSA 
	fclose(fp);
	closesocket(client_socket);
	WSACleanup();
	__main__();
	system("pause");
}
int chat()
{
	/*
	初始化WSA，使得程序可以调用windows socket
	*/
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	/*
	创建监听用套接字，server_socket
	并检测是否创建成功
	*/
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); ;
	if (client_socket == INVALID_SOCKET) {
		//如果创建的socket无效，则结束程序
		perror("socket error !");
		return 0;
	}

	/*
	创建地址结构，server_addr，并设置端口和IP
	*/
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//要连接的服务器端口号 8888
	server_addr.sin_port = htons(8888);
	//指定服务器的地址127.0.0.1
	//InetPton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
	InetPton(AF_INET, "47.93.38.43", &server_addr.sin_addr.s_addr);
	//与地址server_addr建立连接
	if (connect(client_socket, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)))
	{
		perror("connect error !\n");
		return 0;
	}

	initgraph(200, 100, SHOWCONSOLE);
	HWND hwnd = GetHWnd();
	MoveWindow(hwnd, 960, 800, 96, 54, false);
	setbkcolor(WHITE);
	outtextxy(100, 50, "connected");
	cleardevice();

	std::thread recv_thread([](int client_socket)
		{
			char buffer[256];
			while (1)
			{
				time_t t = time(0);
				char time[32] = { NULL };
				strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", localtime(&t));

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				if (recv(client_socket, buffer, sizeof(buffer), 0) > 0)
				{
					//std::cout << client_socket << ':' << buffer << "		@" << time <<std::endl;
					std::string outstr = std::string(buffer) + std::string("		@") + std::string(time);
					std::cout <<  buffer << "	@" << time << std::endl;
				}
				else if (recv(client_socket, buffer, sizeof(buffer), 0) < 0)
					continue;
				else if (recv(client_socket, buffer, sizeof(buffer), 0) == 0)
				{
					std::cout << "disconnect:" << client_socket << std::endl;
					return 0;
				}

			}
		}, client_socket);

	std::thread send_thread([](int client_socket)
		{
			char buffer[256];
			char buffer_d[239];
			//InputBox(buffer_d, 239, "r");
			while (1)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				//std::cout << "请输入发送信息:";
				InputBox(buffer_d, 239, "'_quit' to quit,'Ctrl + Enter' to send msg", "INPUTBOX",false,800,128);
				//std::cin >> buffer_d;
				//buffer = name + buffer_d;
				sprintf(buffer, "%s%s", name, buffer_d);
				//std::strcat(buffer, name);
				//std::strcat(buffer, ":");
				//std::strcat(buffer, buffer_d);
				if (strcmp(buffer_d, "_quit") == 0)
				{
					closesocket(client_socket);
					__main__();
					exit(0);
				}
				int len = send(client_socket, buffer, sizeof(buffer), 0);
				if (len < 0) {
					std::cout << "发送失败！" << std::endl;
					//break;
				}
			}
		}, client_socket);
	send_thread.join();
	recv_thread.join();
		/*
		int recv_len = recv(client_socket, buff, sizeof(buff), 0);
		if (recv_len < 0) {
			std::cout << "接受失败！" << std::endl;
			break;
		}
		else {
			std::cout << "服务端信息:" << buff << std::endl;
		}*/

	closesocket(client_socket);
	WSACleanup();
	system("pause");
}

int __main__()
{
	std::cout << "------1.chatroom------" << std::endl;
	std::cout << "------2.download------" << std::endl;
	std::cout << "------other.quit------" << std::endl;
	int choose;
	std::cin >> choose;
	if (choose == 1)
	{
		std::cout << "输入您的昵称:";
		std::cin >> name;
		name[strlen(name)] = ':';
		chat();
	}
	else if (choose == 2)
		file();
	else
		return 0;
}

int password() 
{
	// 初始化绘图窗口
	initgraph(480, 640);
	IMAGE img;
	loadimage(&img, payimg_dir.c_str(),480,640);
	putimage(0, 0, &img);
	// 定义字符串缓冲区，并接收用户输入
	char pass[11];
	memset(pass, 0, 11);

	InputBox(pass, 11, "欢迎使用！请输入邀请码：\n（若已填写过可忽略不填，点击确定按钮进入）");
	if (strlen(pass) == 0)
	{
		std::strcpy(pass, readCache(cache_dir).c_str());
		//pass = readCache(cache_dir);
	}
	closegraph();
	/*
	初始化WSA，使得程序可以调用windows socket
	*/
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		exit(1);
	}

	/*
	创建监听用套接字，server_socket
	并检测是否创建成功
	*/
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == INVALID_SOCKET) {
		//如果创建的socket无效，则结束程序
		perror("socket error !");
		exit(1);
	}

	/*
	创建地址结构，server_addr，并设置端口和IP
	*/
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//要连接的服务器端口号 7777
	server_addr.sin_port = htons(7777);
	//指定服务器的地址127.0.0.1
	//InetPton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
	InetPton(AF_INET, "47.93.38.43", &server_addr.sin_addr.s_addr);
	//与地址server_addr建立连接
	if (connect(client_socket, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)))
	{
		perror("connect error !\n");
		exit(1);
	}


	if (send(client_socket, pass, sizeof(pass), 0) < 0)
	{
		perror("send error!");
		closesocket(client_socket);
		WSACleanup();
		return -1;
	}

	char get[6];
	int recv_answer = recv(client_socket, get, sizeof(get), 0);
	if (strcmp(get,"false") == 0) {
		perror("邀请码错误!");
		closesocket(client_socket);
		WSACleanup();
		return -1;
	}
	else {
		saveCache(cache_dir,pass);
		return 1;
	}
	closesocket(client_socket);
	WSACleanup();

}
int main()
{
	if (password() == 1)
		__main__();
	else
		return 0;
	return 0;
}