#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int client_count = 0;
int client_sockets[MAX_CLIENTS];
pthread_t threads[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg)
{
    int client_sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
	int current_client = 0;

    // 添加客户端套接字到客户端表中；并更新client_count
    pthread_mutex_lock(&mutex);
    client_sockets[client_count++] = client_sock;
    pthread_mutex_unlock(&mutex);

	current_client = client_count;
	printf("Client %d has connected!\n", current_client);

    while (1) 
	{
        memset(buffer, 0, BUFFER_SIZE);

		// 接收客户端消息
        int bytes_recv = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_recv <= 0) 
		{
            break;// 套接字被关闭
        }

		// 把接收到的消息发送到其他客户端
        for (int i = 0; i < client_count; i++) 
		{
            if (client_sockets[i] != client_sock) 
			{
                send(client_sockets[i], buffer, bytes_recv, 0);
            }
        }

		// 把接收到的消息打印出来
		printf("Client %d : %s\n", current_client, buffer);
    }

    // 在客户端表中删除客户端套接字
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; i++) 
	{
        if (client_sockets[i] == client_sock) 
		{
            client_sockets[i] = client_sockets[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);


    // 关闭套接字并退出线程
    printf("Client %d has exited!\n", current_client);
    close(client_sock);
    pthread_exit(NULL);
}

int creat_server_socket(int *server_sockfd)
{
	int *fd = server_sockfd;

	// 申请socket
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd < 0) 
	{
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = 
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };
		
	//允许地址复用，一般只在调试时使用
	int opt = 1;
	if (setsockopt(*fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

    if (bind(*fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(*fd, MAX_CLIENTS) < 0) 
	{
        perror("listen");
        exit(EXIT_FAILURE);
    }


	return 0;

}

int pocess_client(int server_sockfd)
{
	int client_sock;
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	pthread_t thread;


	// 等待客户端连接
	client_sock = accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_len);
	if (client_sock < 0) 
	{
		perror("accept");
	}

	// 创建线程
	if (pthread_create(&thread, NULL, handle_client, &client_sock) != 0)
	{
		perror("pthread_create");
	}

	return 0;
}

int main()
{
	int server_sockfd = 0;

	//创建服务端socket
	creat_server_socket(&server_sockfd);
    printf("Server started, waiting for connections...\n");



    while (1) 
	{
		// 处理客户端连接
		pocess_client(server_sockfd);

        // 当线程数大于等于MAX_CLIENTS，等待线程被释放
        pthread_mutex_lock(&mutex);
        while (client_count >= MAX_CLIENTS) 
		{
            pthread_mutex_unlock(&mutex);
            sleep(1);
            pthread_mutex_lock(&mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    close(server_sockfd);
    return 0;
}


//printf("Function: %s, Line: %d\n", __func__, __LINE__);





