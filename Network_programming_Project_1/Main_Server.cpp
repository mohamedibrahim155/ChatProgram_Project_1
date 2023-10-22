#pragma once

// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include "Buffer.h"

#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"

#define DEFAULT_BUFLEN 512
int main() 
{

    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    int Iresult;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    Iresult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (Iresult != 0)
    {
        printf("WSAStartup failed: %d\n", Iresult);
        return 1;
    }
    printf("WSAStartup success: %d\n", Iresult);


    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
  //  Iresult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    Iresult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
    if (Iresult != 0)
    {
        printf("getaddrinfo failed: %d\n", Iresult);
        WSACleanup();
        return 1;
    }
    printf("getaddrinfo success: %d\n", Iresult);

;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    printf("socket() success: %d\n", Iresult);


    // Setup the TCP listening socket
    Iresult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (Iresult == SOCKET_ERROR) 
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("bind successfull !\n");

    freeaddrinfo(result);

    Iresult = listen(ListenSocket, SOMAXCONN);
    if (Iresult == SOCKET_ERROR)
    {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("Listen successfull !\n");



    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("accepted client successfully !\n");


    // No longer need server socket
    closesocket(ListenSocket);


    // Receive until the peer shuts down the connection
    do {

        Iresult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (Iresult > 0)
        {
            printf("Bytes received: %d\n", Iresult);

            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, Iresult, 0);
            if (iSendResult == SOCKET_ERROR) 
            {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (Iresult == 0)
            printf("Connection closing...\n");
        else 
        {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (Iresult > 0);


    Iresult = shutdown(ClientSocket, SD_SEND);
    if (Iresult == SOCKET_ERROR) 
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    printf("shutdown Successful from server \n");

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
