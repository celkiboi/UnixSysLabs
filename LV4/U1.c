#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define COMMUNICATION_BUFFER 256

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Error. Incorrect param number. Param number must be 3.\nUsage: client [NAME] [IP] [PORT]\n");
        return 1;
    }

    char* ipAddress = argv[2];
    uint16_t port = (uint16_t)atoi(argv[3]);
    char* name = argv[1];
    size_t nameLength = strlen(name);

    int communicationSocket;
    struct sockaddr_in socketAddress;

    communicationSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (communicationSocket == -1)
    {
        perror("Cannot create socket");
        return 3;
    }

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, ipAddress, &socketAddress.sin_addr) <= 0)
    {
        perror("Invalid IP address format");
        close(communicationSocket);
        return 4;
    }

    if (connect(communicationSocket, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) == -1)
    {
        perror("Cannot connect");
        close(communicationSocket);
        return 5;
    }

    char messageBuffer[COMMUNICATION_BUFFER] = { 0 };
    snprintf(messageBuffer, sizeof(messageBuffer), "N%s", name);
    send(communicationSocket, messageBuffer, nameLength + 1, 0);

    char replyBuffer[COMMUNICATION_BUFFER] = { 0 };
    ssize_t bytesReceived = recv(communicationSocket, replyBuffer, sizeof(replyBuffer), 0);
    if (bytesReceived > 0)
    {
        printf("Server responded with a message: %s\n", replyBuffer);
    }

    bytesReceived = recv(communicationSocket, replyBuffer, sizeof(replyBuffer), 0);
    if (bytesReceived > 0)
    {
        printf("Server responded with a message: %s\n", replyBuffer);
        send(communicationSocket, "OK", 3, 0);
    }

    close(communicationSocket);
    return 0;
}
