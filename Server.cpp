#pragma once
/*
	Socket服务器端代码
	服务器监听端口8888
	建立连接后，接收Client传输的文件路径
	若文件路径存在，则发送该文件给Client，发送完毕则关闭连接。
*/
#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include<mutex>
#include<string>
#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>   
#include <string.h>   
#include <Ws2tcpip.h>
#include<iostream>
#include<thread>
#include<vector>
#include<io.h>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")
std::string password("*#06*0x7bn");
#define BUFFER_SIZE 2048
#define FILE_NAME_MAX_SIZE 512 
std::vector<std::string> filelst;
std::vector<int> lst;
std::string dir_path = "C:\\Server\\data\\";
void find()
{
	/*
		查找根目录下的文件
	*/
	//vec.push_back("Video_Pixel_Networks.pdf");
	//vec.push_back("ML.pdf");2
	//std::memset(filelst, 0, BUFFER_SIZE);
	long handle = 0;
	struct _finddata_t fileinfo;
	std::string p;
	if ((handle = _findfirst(p.assign(dir_path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					continue;
			}
			else
				filelst.push_back(fileinfo.name);
		} while (_findnext(handle, &fileinfo) == 0);
	}
	_findclose(handle);
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
		类型是TCP
		并检测是否创建成功
	*/
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		//如果创建的socket无效，则结束程序
		perror("socket error !\n");
		return 0;
	}

	/*
	   创建地址，server_addr，并设置端口和IP
	*/
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	//端口号 6666
	server_addr.sin_port = htons(6666);
	//INADDR_ANY表示本机任意IP地址
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;


	//将socket与地址server_addr绑定
	if (bind(server_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		perror("bind error !\n");
		return 0;
	}

	//server_socket开始监听
	if (listen(server_socket, 20) == SOCKET_ERROR)
	{
		perror("listen error !\n");
		return 0;
	}

	while (1)
	{
		printf("等待连接...\n");

		// 定义客户端的socket和socket地址结构
		SOCKET client_socket;
		sockaddr_in client_addr;
		int client_addr_length = sizeof(client_addr);

		// 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信 
		// accept函数会把连接到的客户端信息写到client_addr中 
		client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_length);
		if (client_socket == INVALID_SOCKET)
		{
			perror("Socket连接建立失败:\n");
			continue;
		}

		char IP_BUFFER[256];
		memset(IP_BUFFER, 0, 256);
		InetNtop(AF_INET, &client_addr.sin_addr, IP_BUFFER, 256);
		printf("Socket连接建立，客户端IP为：%s，端口为：%d\n", IP_BUFFER, ntohs(client_addr.sin_port));
	
		std::string send_str;
		for (std::vector<std::string>::iterator itr = filelst.begin(); itr != filelst.end(); ++itr)
		{
			send_str = send_str + *itr;
			send_str = send_str + ',';
			std::cout << *itr << std::endl;
		}
		send(client_socket, send_str.c_str(), strlen(send_str.c_str()), 0);
		
		//接收客户端请求的的文件路径
		// recv函数接收数据到缓冲区buffer中 
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		if (recv(client_socket, buffer, sizeof(buffer), 0) < 0)
		{
			perror("接收文件名失败:\n");
			continue;
		}

		// 然后从buffer(缓冲区)拷贝到file_name中 
		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
		sprintf(file_name, "%s%s", dir_path.c_str(), buffer);
		//strncpy_s(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));

		// 打开文件并读取文件数据 
		FILE* fp;
		errno_t F_ERR = fopen_s(&fp, file_name, "rb");
		if (F_ERR != 0)
		{
			printf("文件打开失败:%s\n", file_name);
			continue;
		}
		else
		{
			printf("开始传输文件：%s\n", file_name);
			memset(buffer, 0, BUFFER_SIZE);
			int length = 0;
			// 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
			while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
			{
				if (send(client_socket, buffer, length, 0) < 0)
				{
					printf("文件发送失败:%s\n", file_name);
					continue;
				}
				memset(buffer, 0, BUFFER_SIZE);
			}

			// 关闭文件 
			fclose(fp);
			printf("文件传输完成:%s!\n", file_name);
		}
		// 关闭与客户端的连接 
		closesocket(client_socket);
	}


	// 关闭监听用的socket 
	closesocket(server_socket);
	WSACleanup();
}



void res(int sock,std::string str)
{
	send(sock, str.c_str(), sizeof(str), 0);
}
int chat()
{
	//初始化
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	/*
		创建监听用套接字，server_socket
		类型是TCP
		并检测是否创建成功
	*/
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		//如果创建的socket无效，则结束程序
		perror("socket error !\n");
		return 0;
	}

	//监听套接字设置为非阻塞
	unsigned long ulMode = 1;
	ioctlsocket(server_socket, FIONBIO, &ulMode);

	/*
	   创建地址，server_addr，并设置端口和IP
	*/
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	//端口号 8888
	server_addr.sin_port = htons(8888);
	//INADDR_ANY表示本机任意IP地址
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;


	//将socket与地址server_addr绑定
	if (bind(server_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		perror("bind error !\n");
		return 0;
	}

	//server_socket开始监听
	if (listen(server_socket, 20) == SOCKET_ERROR)
	{
		perror("listen error !\n");
		return 0;
	}
	//thread listenThread(Listen, listenSock);
	std::thread listen_thread([](int server_socket)
		{
			SOCKET client_socket;
			sockaddr_in client_addr;
			int client_addr_length = sizeof(client_addr);
			while (1)
			{
				//std::cout << "等待连接..." << std::endl;
				/*
					std::this_thread::sleep_for：
					表示当前线程休眠一段时间，休眠期间不与其他线程竞争，
					根据线程需求，等待若干时间。
				*/
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_length);
				if (client_socket != INVALID_SOCKET)
				{
					lst.push_back(client_socket);
					std::cout << "connect:" << client_socket << std::endl;
					res(client_socket,"successfully connected");
				}
				if (client_socket == -1)
					continue;
			}
		}, server_socket);

	std::thread recv_thread([]()
		{
			char buffer[256];
			while (1)
			{
				bool flag = true;
				time_t t = time(0);
				char time[32] = { NULL };
				strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", localtime(&t));
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
				for (std::vector<int>::iterator itr = lst.begin(); itr != lst.end();)
				{
					if (recv(*itr, buffer, sizeof(buffer), 0) < 0)
						itr++;
					else if (recv(*itr, buffer, sizeof(buffer), 0) == 0)
					{
						itr = lst.erase(itr);
						std::cout << "disconnect:" << *itr << std::endl;
					}
					else
					{
						std::cout << "recv:" << *itr << ':' << buffer << "	@"<< time << std::endl;
						itr++;
						if (flag == true)
						{
							for (std::vector<int>::iterator i = lst.begin(); i != lst.end(); i++)
								send(*i, buffer, sizeof(buffer), 0);
							flag = false;
						}
					}
				}
			}
		});
	listen_thread.join();
	recv_thread.join();
	closesocket(server_socket);
	WSACleanup();
	return 0;

}

int login()
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
		类型是TCP
		并检测是否创建成功
	*/
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		//如果创建的socket无效，则结束程序
		perror("socket error !\n");
		return 0;
	}

	/*
	   创建地址，server_addr，并设置端口和IP
	*/
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	//端口号 7777
	server_addr.sin_port = htons(7777);
	//INADDR_ANY表示本机任意IP地址
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;


	//将socket与地址server_addr绑定
	if (bind(server_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		perror("bind error !\n");
		return 0;
	}

	//server_socket开始监听
	if (listen(server_socket, 20) == SOCKET_ERROR)
	{
		perror("listen error !\n");
		return 0;
	}
	while (1)
	{
		printf("等待连接...\n");

		// 定义客户端的socket和socket地址结构
		SOCKET client_socket;
		sockaddr_in client_addr;
		int client_addr_length = sizeof(client_addr);

		// 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信 
		// accept函数会把连接到的客户端信息写到client_addr中 
		client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_length);
		if (client_socket == INVALID_SOCKET)
		{
			perror("Socket连接建立失败:\n");
			continue;
		}

		char IP_BUFFER[256];
		memset(IP_BUFFER, 0, 256);
		InetNtop(AF_INET, &client_addr.sin_addr, IP_BUFFER, 256);
		printf("Socket连接建立，客户端IP为：%s，端口为：%d\n", IP_BUFFER, ntohs(client_addr.sin_port));

		// recv函数接收数据到缓冲区buffer中 
		char pass[11];
		memset(pass, 0, 11);
		if (recv(client_socket, pass, sizeof(pass), 0) < 0)
		{
			perror("接收失败:\n");
			continue;
		}
		std::cout << password << std::endl;
		std::cout << "recvpass:" << pass << std::endl;
		if (std::strcmp(pass,password.c_str()) == 0)
		{
			send(client_socket, "true", sizeof("true"), 0);
			std::cout << "send:true" << std::endl;
		}
		else
		{
			send(client_socket, "false", sizeof("false"), 0);
			std::cout << "send:false" << std::endl;
		}
		// 关闭与客户端的连接 
		closesocket(client_socket);
	}
	// 关闭监听用的socket 
	closesocket(server_socket);
	WSACleanup();
	return 0;
}
int main(int argc, char* argv[])
{
	std::thread login_thread(login);
	find();
	std::thread chat_thread(chat);
	std::thread file_thread(file);
	login_thread.join();
	file_thread.join();
	chat_thread.join();
	return 0;
}