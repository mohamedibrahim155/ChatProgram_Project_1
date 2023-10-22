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
#include<thread>
#include <iphlpapi.h>

#include<map>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cctype>
#include <algorithm>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

std::mutex _messageQueueMutex;
std::condition_variable _messageArrived;
std::queue<std::string> _messageQueue;
std::vector<std::thread> clientThreads;
std::vector<SOCKET> clientSockets;
std::map<int, std::vector <SOCKET>> ClientsInRoomID;


bool ContainsLeave(const std::string& message);
  void SendReceivedMessageToTheOtherClients(std::vector<uint8_t>& recvBuffer, ChatMessage& receivedMsg, Client_ID& clientId, SOCKET clientSocket);
  void  ReceiveAndPrintIncomingMessage(SOCKET clientSocket);
  void ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket);
  void RemoveSocketInRoom( SOCKET clientSocket);

void ReceiveMessagesFromClient(SOCKET clientSocket) 
{
    char recvBuffer[DEFAULT_BUFLEN];
    while (true) 
    {
        std::string receivedMsg;
        std::uint32_t receivedMsgNumber32 = 0;
        std::uint16_t receivedMsgNumber16 = 0;
        const int buffersize = DEFAULT_BUFLEN;
        Buffer recevieBuffer(buffersize);
        std::vector<uint8_t> recvBuffer(buffersize);
        ChatMessage receivedGlobal;
        int receivedBytes = recv(clientSocket, reinterpret_cast<char*>(&recvBuffer[0]), buffersize, 0);
        if (receivedBytes > 0)
        {
            recevieBuffer.m_BufferData = recvBuffer;
            recevieBuffer.m_ReadIndex = 0;
            ChatMessage ReceiveMyMessage = recevieBuffer.ReadChatMessage();
            receivedMsg = *static_cast<std::string*>(ReceiveMyMessage.messageToLoad);
            std::lock_guard<std::mutex> lock(_messageQueueMutex);
            _messageQueue.push(receivedMsg);
            _messageArrived.notify_one();
        }
        // Handle other cases
    }
}
void SendReceivedMessageToTheOtherClientsBaseOnRoomID(int _RoomId, std::vector<uint8_t>& recvBuffer, ChatMessage& _receivedMsg, Client_ID& clientId, SOCKET clientSocket);

bool isStringEmptyOrWhitespace(const std::string& str) 
{
    // Check if the string is empty
    if (str.empty()) 
    {
        return true;
    }
    // Check if all characters in the string are whitespace
    return std::all_of(str.begin(), str.end(), [](unsigned char ch)
        {
            return std::isspace(ch);
        });
}
void AcceptClientConnections(SOCKET serverSocket)
{
    while (true)
    {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Invalid client socket." << std::endl;
            continue;
        }
        ReceiveAndPrintIncomingMessageOnSeparateThread(clientSocket);
    }
}
int  main(void)
{
    WSADATA wsaData;
    int intResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    intResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (intResult != 0) {
        printf("WSAStartup failed with error: %d\n", intResult);
        return 1;
    }
    printf("WSAStartup success in server \n");
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    intResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
    if (intResult != 0) 
    {
        printf("getaddrinfo failed with error: %d\n", intResult);
        WSACleanup();
        return 1;
    }
    printf("getaddrinfo success in server \n");
    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    printf("Socket created success in server \n");
    // Setup the TCP listening socket
    intResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (intResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("Socket  binded  success in server \n");

    freeaddrinfo(result);

    intResult = listen(ListenSocket, SOMAXCONN);
    if (intResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf(" Listen success in server \n");


    AcceptClientConnections(ListenSocket);


    // shutdown the connection since we're done
    intResult = shutdown(ClientSocket, SD_SEND);
    if (intResult == SOCKET_ERROR) 
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    printf(" shutdown success in server \n");
    
    // cleanup
    closesocket(ClientSocket);

    for (auto& thread : clientThreads) 
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    WSACleanup();

    clientSockets.clear();
    return 0;
}


void ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket)
{
    clientSockets.push_back(clientSocket);
    std::thread clientThread(ReceiveAndPrintIncomingMessage, clientSocket);
    clientThreads.push_back(std::move(clientThread));
}
void AddSocketToRoom(int roomID, SOCKET clientSocket)
{
    auto it = ClientsInRoomID.find(roomID);
    if (it != ClientsInRoomID.end())
    {
       
        auto it2 = std::find(it->second.begin(), it->second.end(), clientSocket);
        if (it2 == it->second.end())
        {
            it->second.push_back(clientSocket);
        }
       
    }
    else 
    {
        ClientsInRoomID[roomID] = std::vector<SOCKET>{ clientSocket };
    }
}

void RemoveSocketInRoom(SOCKET clientSocket)
{


    for (auto& pair : ClientsInRoomID) 
    {
        std::vector<SOCKET>& sockets = pair.second;
        auto socketIt = std::find(sockets.begin(), sockets.end(), clientSocket);
        if (socketIt != sockets.end()) 
        {
            sockets.erase(socketIt); // Remove the socket from the vector
            std::cout << "Socket " << clientSocket << " removed from room " << pair.first << std::endl;
            //break; // Break  loop if socket is found and removed
        }
    }
  

}
void ReceiveAndPrintIncomingMessage(SOCKET clientSocket)
{
    const int buffersize = DEFAULT_BUFLEN;
    while (true)
    {

        ChatMessage message;
        Client_ID ID;
        Buffer buffer(buffersize);
        std::vector<uint8_t> recvBuffer(buffersize);
        int amountReceived = recv(clientSocket, reinterpret_cast<char*>(&recvBuffer[0]), buffersize, 0);
        if (amountReceived > 0)
        {
            buffer.m_BufferData = recvBuffer;
            buffer.m_ReadIndex = 0;
            message = buffer.ReadChatMessage();
            ID = buffer.ReadClientID();

            std::string receivedMsg = *static_cast<std::string*>(message.messageToLoad);
            std::cout << "Received :" << receivedMsg << " From : " << ID.ClientName << " RoomID : " << ID.RoomID << std::endl;
           
            
            if (!isStringEmptyOrWhitespace(receivedMsg))
            {
                AddSocketToRoom(ID.RoomID, clientSocket);
                //  SendReceivedMessageToTheOtherClients(recvBuffer, message, ID, clientSocket);
                SendReceivedMessageToTheOtherClientsBaseOnRoomID(ID.RoomID, recvBuffer, message, ID, clientSocket);
            }
     

        }
        if (amountReceived == 0)
        {
            //Client Disconnected ...
            RemoveSocketInRoom( clientSocket);
            std::cout << "Client Disconnected .." << std::endl;

            break;
        }
        else if (amountReceived == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            if (error == WSAECONNRESET)
            {
                // Client forcefully disconnected
                RemoveSocketInRoom(clientSocket);
                break;
            }
        }
    }
    closesocket(clientSocket);
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


void SendReceivedMessageToTheOtherClients(std::vector<uint8_t> &recvBuffer, ChatMessage& _receivedMsg, Client_ID& clientId, SOCKET clientSocket)
{
    Buffer sendBuffer(DEFAULT_BUFLEN);
    std::string receivedMsg = *static_cast<std::string*>(_receivedMsg.messageToLoad);

    if (receivedMsg =="Joined")
    {
        receivedMsg = " has joined server ";
        _receivedMsg.messageToLoad = &receivedMsg;
    }
    if (true)
    {

    }
    sendBuffer.WriteBaseOnMessageType(_receivedMsg);
    sendBuffer.WriteBaseOnMessageType(clientId);

    for (size_t i = 0; i < clientSockets.size(); i++)
    {
        
        if (clientSockets[i] != clientSocket)
        {

            send(clientSockets[i], reinterpret_cast<const char*>(&sendBuffer.m_BufferData[0]), sendBuffer.m_WriteIndex, 0);
        }
    }
}

void SendReceivedMessageToTheOtherClientsBaseOnRoomID(int _RoomId, std::vector<uint8_t>& recvBuffer, ChatMessage& _receivedMsg, Client_ID& clientId, SOCKET clientSocket)
{
    Buffer sendBuffer(DEFAULT_BUFLEN);
    std::string receivedMsg = *static_cast<std::string*>(_receivedMsg.messageToLoad);

    if (receivedMsg == "Joined")
    {
        receivedMsg = " has joined room";
        _receivedMsg.messageToLoad = &receivedMsg;
    }
   /* if (receivedMsg == "JoinedRoom")
    {
        receivedMsg = " has joined new room";
        _receivedMsg.messageToLoad = &receivedMsg;
    }*/
     if (receivedMsg == "LeaveRoom")
     {
         receivedMsg = " has left room";
         _receivedMsg.messageToLoad = &receivedMsg;
         RemoveSocketInRoom(clientSocket);

     }
    sendBuffer.WriteBaseOnMessageType(_receivedMsg);
    sendBuffer.WriteBaseOnMessageType(clientId);

    auto it = ClientsInRoomID.find(_RoomId);
    if (it != ClientsInRoomID.end())
    {
        for (const auto& socket : it->second)
        {
            if (socket != clientSocket)
            {
                if (receivedMsg != "JoinedRoom")
                {
                    send(socket, reinterpret_cast<const char*>(&sendBuffer.m_BufferData[0]), sendBuffer.m_WriteIndex, 0);
                }
                
                std::cout << " room ID : " << _RoomId << std::endl;
            }
            
           
        }

    }
}