//// Server.cpp
//#include <iostream>
//#include <thread>
//#include <vector>
//#include <WinSock2.h>
//#pragma comment(lib, "Ws2_32.lib")
//
//std::vector<SOCKET> connectedClients;
//
//void ClientHandler(SOCKET clientSocket) {
//    while (true) {
//        char buffer[1024];
//        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
//        if (bytesReceived > 0) {
//            // Send the received message to all other connected clients
//            for (SOCKET otherClient : connectedClients) {
//                if (otherClient != clientSocket) {
//                    send(otherClient, buffer, bytesReceived, 0);
//                }
//            }
//        }
//    }
//}
//
//int main() {
//    // Winsock initialization
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        std::cerr << "WSAStartup failed." << std::endl;
//        return 1;
//    }
//
//    // Create a socket
//    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//    if (serverSocket == INVALID_SOCKET) {
//        std::cerr << "Socket creation failed." << std::endl;
//        WSACleanup();
//        return 1;
//    }
//
//    // Bind the socket to an IP address and port
//    sockaddr_in hint;
//    hint.sin_family = AF_INET;
//    hint.sin_port = htons(54000);
//    hint.sin_addr.S_un.S_addr = INADDR_ANY;
//
//    bind(serverSocket, (sockaddr*)&hint, sizeof(hint));
//
//    // Tell Winsock the socket is for listening
//    listen(serverSocket, SOMAXCONN);
//
//    // Wait for a connection
//    while (true) {
//        sockaddr_in clientAddr;
//        int clientSize = sizeof(clientAddr);
//
//        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
//
//        if (clientSocket == INVALID_SOCKET) {
//            std::cerr << "Invalid client socket." << std::endl;
//            continue;
//        }
//
//        connectedClients.push_back(clientSocket);
//        std::thread clientThread(ClientHandler, clientSocket);
//        clientThread.detach();
//    }
//
//    // Close listening socket
//    closesocket(serverSocket);
//
//    // Cleanup Winsock
//    WSACleanup();
//
//    return 0;
//}
