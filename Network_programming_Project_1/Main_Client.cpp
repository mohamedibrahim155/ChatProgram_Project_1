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
#include <thread>
#include <iphlpapi.h>
#include <cctype>
#include <algorithm>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

std::string ClientName;
std::atomic<bool> isRunning(true);
int roomId;
bool isfirstTime =true;
std::atomic<bool> isLeftRoom = true;
bool isNewRoomJoined = true;
void ClientLeftRoom();
bool isStringEmptyOrWhitespace(const std::string& str) {
    // Check if the string is empty
    if (str.empty()) {
        return true;
    }
    // Check if all characters in the string are whitespace
    return std::all_of(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
        });
}

bool ContainsLeave(const std::string& message)
{
    std::string substringToCheck = "/leave";

    // Check if the string contains the substring "/leave" after trimming leading and trailing whitespaces
    size_t startPos = message.find_first_not_of(" \t");
    size_t endPos = message.find_last_not_of(" \t");

    if (startPos != std::string::npos && endPos != std::string::npos)
    {
        std::string trimmedMessage = message.substr(startPos, endPos - startPos + 1);
        return trimmedMessage.find(substringToCheck) != std::string::npos;
    }
    else
    {
        return false;
    }
}

void ReceiveMessages(SOCKET sock)
{

    while (isRunning)
    {

        std::string receivedMsg;
        std::uint32_t receivedMsgNumber32 = 0;
        std::uint16_t receivedMsgNumber16 = 0;
        const int buffersize = DEFAULT_BUFLEN;
        Buffer recevieBuffer(buffersize);
        std::vector<uint8_t> recvBuffer(buffersize);
        ChatMessage receivedGlobal;
     

            int bytesReceived = recv(sock, reinterpret_cast<char*>(&recvBuffer[0]), buffersize, 0);

            if (bytesReceived > 0)
            {
                recevieBuffer.m_BufferData = recvBuffer;
                recevieBuffer.m_ReadIndex = 0;
                ChatMessage ReceiveMyMessage = recevieBuffer.ReadChatMessage();
                Client_ID ID = recevieBuffer.ReadClientID();
                receivedMsg = *static_cast<std::string*>(ReceiveMyMessage.messageToLoad);
               
               //if (receivedMsg == " has left room")
               // {
               //     std::cout << "[" << ID.ClientName << "]" << " : " << receivedMsg << std::endl;

               //     
               // }
               // if (receivedMsg == " joined  new room")
               // {
               //     std::cout << "[" << ID.ClientName << "]" << " : " << "joined new Room" << std::endl;
               //    
               // }
               // if (receivedMsg == " has joined room")
               // {
               //     std::cout << "[" << ID.ClientName << "]" << " : " << "has joined room" << std::endl;
               // }

               /* if (ID.RoomID == roomId && !receivedMsg.empty() && !isStringEmptyOrWhitespace(receivedMsg))
                {
                    std::cout << "[" << ID.ClientName << "]" << " : " << receivedMsg << std::endl;
                }*/
                /*else if (receivedMsg == "Joined")
                {

                    std::cout << "[" << ID.ClientName << "]" << " : " << "joined Room" << std::endl;

                }*/
                /*else*/ if (!receivedMsg.empty() && !isStringEmptyOrWhitespace(receivedMsg))
                {
                   /* if (receivedMsg=="/leaveroom")
                    {
                        std::cout << "[" << ID.ClientName << "]" << " : " << "has left room" << std::endl;
                    }
                    if (receivedMsg == "/joinroom")
                    {
                        std::cout << "[" << ID.ClientName << "]" << " : " << "has joined room" << std::endl;
                    }*/
                    //else
                    {
                        std::cout << "[" << ID.ClientName << "]" << " : " << receivedMsg << std::endl;

                    }
                }




            }
            if (bytesReceived == 0)
                break;

    }
    closesocket(sock);
}



void readConsoleEntriesAndSendToServer(SOCKET serverSocket);


int main()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
  
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    // Initialize Winsock
    std::cout << "Enter your name : ";
    std::getline(std::cin, ClientName);
    std::cout << "Enter your ROOM ID : ";
    std::cin >> roomId;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    printf("SAStartup  Successful in client\n");
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) 
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    printf("getaddrinfo  Successful in client\n");
    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
        {
            std::cerr << "Can't connect to server!" << std::endl;
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            WSACleanup();
            return 0;
        }
        else
        {
            
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    printf("connected to server Successfully in client\n");


    std::thread receiveThread(ReceiveMessages, ConnectSocket);

        readConsoleEntriesAndSendToServer(ConnectSocket);

        if (receiveThread.joinable())
        {
            receiveThread.join();
        }
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
       WSACleanup();
        return 1;
    }
    printf("shutdown Successfully in client\n");

  
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

void JoinAnotherRoom()
{
    if (!isNewRoomJoined)
    {
        std::cout << "Enter Room ID : ";
        roomId = 0;
        std::cin >> roomId;
    }
   
}

void ClientLeftRoom()
{
    if (!isLeftRoom)
    {
        std::cout << "Enter Room ID : ";
        roomId = 0;
        std::cin >> roomId;
        isLeftRoom = true;
        std::cout << "isFalse from received " << " : " << isLeftRoom << std::endl;

    }
}
void readConsoleEntriesAndSendToServer(SOCKET serverSocket) 
{
    const int buffersize = DEFAULT_BUFLEN;
    std::string userInput;
    std::string previosuSentMsg;
    
    while (isRunning)
    {
        if (previosuSentMsg.length()>0 && previosuSentMsg == "/leaveroom")
        {
            isLeftRoom = false;
            ClientLeftRoom();
            Client_ID ID;
            ID.ClientName = ClientName;
            ID.RoomID = roomId;
            ChatMessage SendMyMessage;
            SendMyMessage.packetSize;
            SendMyMessage.messageType = STRING;
            // std::string MessageSending = /*"[" + ID.ClientName + "]" + " : " +*/ userInput;
            std::string MessageSending = /*"[" + ID.ClientName + "]" + " : " +*/ "has left room\n";
            SendMyMessage.messageLength;
            SendMyMessage.messageToLoad = &MessageSending;


            Buffer SendBuffer(buffersize);
            SendBuffer.WriteBaseOnMessageType(SendMyMessage);
            SendBuffer.WriteBaseOnMessageType(ID);
            int sendResult = send(serverSocket, reinterpret_cast<const char*>(&SendBuffer.m_BufferData[0]), SendBuffer.m_WriteIndex, 0);
            if (sendResult == SOCKET_ERROR)
            {
                std::cerr << "Send failed." << std::endl;
                isRunning = false;
                break;
            }
        }
        if (previosuSentMsg.length() > 0 && previosuSentMsg == "/joinroom")
        {
            isNewRoomJoined = false;
            JoinAnotherRoom();

            Client_ID ID;
            ID.ClientName = ClientName;
            ID.RoomID = roomId;
            ChatMessage SendMyMessage;
            SendMyMessage.packetSize;
            SendMyMessage.messageType = STRING;
           // std::string MessageSending = /*"[" + ID.ClientName + "]" + " : " +*/ userInput;
            std::string MessageSending = /*"[" + ID.ClientName + "]" + " : " +*/ "has joined room\n";
            SendMyMessage.messageLength;
            SendMyMessage.messageToLoad = &MessageSending;


            Buffer SendBuffer(buffersize);
            SendBuffer.WriteBaseOnMessageType(SendMyMessage);
            SendBuffer.WriteBaseOnMessageType(ID);
            int sendResult = send(serverSocket, reinterpret_cast<const char*>(&SendBuffer.m_BufferData[0]), SendBuffer.m_WriteIndex, 0);
            if (sendResult == SOCKET_ERROR)
            {
                std::cerr << "Send failed." << std::endl;
                isRunning = false;
                break;
            }
        }
        

        std::getline(std::cin, userInput);
        previosuSentMsg = "";
        previosuSentMsg = userInput;
        Client_ID ID;
        ID.ClientName = ClientName;
        ID.RoomID = roomId;
        ChatMessage SendMyMessage;
        SendMyMessage.packetSize;
        SendMyMessage.messageType = STRING;

        if (isStringEmptyOrWhitespace(userInput) && isfirstTime)
        {
            isfirstTime = false;
            userInput = "Joined";
        }
        if (userInput == "/joinroom")
        {
            userInput = "JoinedRoom";
        }
        if (userInput == "/leaveroom")
        {
            userInput = "LeaveRoom";
        }
        std::string MessageSending = /*"[" + ID.ClientName + "]" + " : " +*/ userInput;
        SendMyMessage.messageLength;
        SendMyMessage.messageToLoad = &MessageSending;


        Buffer SendBuffer(buffersize);
        SendBuffer.WriteBaseOnMessageType(SendMyMessage);
        SendBuffer.WriteBaseOnMessageType(ID);
        int sendResult = send(serverSocket, reinterpret_cast<const char*>(&SendBuffer.m_BufferData[0]), SendBuffer.m_WriteIndex, 0);
        if (sendResult == SOCKET_ERROR)
        {
            std::cerr << "Send failed." << std::endl;
            isRunning = false;
            break;
        }

    }
    //closesocket(serverSocket);

}


