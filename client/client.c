#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "149.28.223.35" // 替换成您的云服务器 IP 地址
#define SERVER_PORT 8888

#define BUFFER_SIZE 1024

int main() {
    int sock_fd = 0;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    int bytes_sent = 0;
	//int bytes_received = 0;

    // 创建客户端 socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址和端口号
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) 
	{
        perror("invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    // 连接服务器
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);

    // 发送和接收数据
    while (1) 
	{
        // 发送消息
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n') 
		{
            buffer[strlen(buffer) - 1] = '\0'; // 去除 fgets 函数读取的换行符
        }
        bytes_sent = send(sock_fd, buffer, strlen(buffer), 0);
        if (bytes_sent < 0) 
		{
            perror("send failed");
            exit(EXIT_FAILURE);
        }

#if 0
        // 接收消息
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(sock_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) 
		{
            perror("recv failed");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) 
		{
            printf("Server closed the connection\n");
            break;
        } else 
		{
            printf("Received message: %s\n", buffer);
        }
#endif
    }

    // 关闭客户端 socket
    close(sock_fd);
    printf("Connection closed\n");

    return 0;
}

