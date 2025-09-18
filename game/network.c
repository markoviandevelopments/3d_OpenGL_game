#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void initNetwork(void)
{
    gameState.clientSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (gameState.clientSock < 0)
    {
        perror("socket");
        exit(1);
    }
    fcntl(gameState.clientSock, F_SETFL, O_NONBLOCK);

    gameState.serverAddr.sin_family = AF_INET;
    gameState.serverAddr.sin_port = htons(8042);
    gameState.serverAddr.sin_addr.s_addr = inet_addr("192.168.1.126"); // Match server IP
    gameState.addrLen = sizeof(gameState.serverAddr);

    gameState.clientMessage.command = 1;
    gameState.clientMessage.position.x = gameState.posX;
    gameState.clientMessage.position.y = gameState.posY;
    gameState.clientMessage.position.z = gameState.posZ;
    ssize_t sent = sendto(gameState.clientSock, &gameState.clientMessage, sizeof(ClientMessage), 0,
                          (struct sockaddr *)&gameState.serverAddr, gameState.addrLen);
    if (sent < 0)
    {
        perror("Main server registration failed");
    }

    gameState.sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (gameState.sockFd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    fcntl(gameState.sockFd, F_SETFL, O_NONBLOCK);

    gameState.agentServerAddr.sin_family = AF_INET;
    gameState.agentServerAddr.sin_addr.s_addr = inet_addr("192.168.1.126"); // Match server IP
    gameState.agentServerAddr.sin_port = htons(8099);                       // Match agent_server.c

    struct sockaddr_in localAddr = {0};
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(0);

    if (bind(gameState.sockFd, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    int dummy = 1;
    sent = sendto(gameState.sockFd, &dummy, sizeof(int), 0,
                  (struct sockaddr *)&gameState.agentServerAddr, sizeof(gameState.agentServerAddr));
    if (sent < 0)
    {
        perror("Agent server registration failed");
    }
    else
    {
        printf("Client bound to dynamic port and registered with agent server...\n");
    }
}

void timer(int value)
{
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);
    ssize_t ret = recvfrom(gameState.clientSock, &gameState.serverMessage, sizeof(ServerMessage), 0,
                           (struct sockaddr *)&fromAddr, &fromLen);
    if (ret < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
    {
        fprintf(stderr, "Main server receive failed: %s (errno: %d)\n", strerror(errno), errno);
    }
    else if (ret > 0)
    {
        gameState.cubeX = gameState.serverMessage.browniancube[0];
        gameState.cubeY = gameState.serverMessage.browniancube[1];
    }

    gameState.clientMessage.command = gameState.message;
    gameState.clientMessage.position.x = gameState.posX;
    gameState.clientMessage.position.y = gameState.posY;
    gameState.clientMessage.position.z = gameState.posZ;
    sendto(gameState.clientSock, &gameState.clientMessage, sizeof(ClientMessage), 0,
           (struct sockaddr *)&gameState.serverAddr, gameState.addrLen);
    gameState.message = 1;

    struct sockaddr_in senderAddr;
    socklen_t addrLen = sizeof(senderAddr);
    ssize_t bytesReceived = recvfrom(gameState.sockFd, &gameState.agentServerInfo, sizeof(AgentServerInfo), 0,
                                     (struct sockaddr *)&senderAddr, &addrLen);
    if (bytesReceived < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            fprintf(stderr, "Agent Server receive failed: %s (errno: %d)\n", strerror(errno), errno);
        }
    }
    else if (bytesReceived == 0)
    {
        fprintf(stderr, "Agent Server sent empty packet\n");
    }
    else
    {
        printf("Agent Server received %zd bytes\n", bytesReceived);
        // Update game state with received agent data
        gameState.agents = gameState.agentServerInfo.agents;
    }

    static int heartbeatCounter = 0;
    heartbeatCounter++;
    if (heartbeatCounter >= HEARTBEAT_INTERVAL)
    {
        int dummy = 1;
        ssize_t sent = sendto(gameState.sockFd, &dummy, sizeof(int), 0,
                              (struct sockaddr *)&gameState.agentServerAddr, sizeof(gameState.agentServerAddr));
        if (sent < 0)
        {
            perror("Agent server heartbeat failed");
        }
        heartbeatCounter = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(FRAME_INTERVAL_MS, timer, 0);
}