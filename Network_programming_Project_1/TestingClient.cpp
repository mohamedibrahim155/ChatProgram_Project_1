//#include <iostream>
//#include <thread>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//#include<iostream>
//#include <string>
//#pragma comment(lib, "Ws2_32.lib")
//
//void ReceiveMessages(SOCKET sock) {
//    while (true) {
//        char buffer[1024];
//        int bytesReceived = recv(sock, buffer, 1024, 0);
//        if (bytesReceived > 0) {
//            std::cout << "Received: " << std::string(buffer, 0, bytesReceived) << std::endl;
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
//    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
//    if (sock == INVALID_SOCKET) {
//        std::cerr << "Socket creation failed." << std::endl;
//        WSACleanup();
//        return 1;
//    }
//
//    // Connect to the server
//    sockaddr_in hint;
//    hint.sin_family = AF_INET;
//    hint.sin_port = htons(54000);
//
//    // Convert the IP address from string to the appropriate format
//    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);
//
//    if (connect(sock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
//        std::cerr << "Connection failed." << std::endl;
//        closesocket(sock);
//        WSACleanup();
//        return 1;
//    }
//
//    std::thread receiveThread(ReceiveMessages, sock);
//
//    // Sending messages
//    std::string userInput;
//    while (true) {
//        std::getline(std::cin, userInput);
//        int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
//        if (sendResult == SOCKET_ERROR) {
//            std::cerr << "Send failed." << std::endl;
//            break;
//        }
//    }
//
//    // Cleanup Winsock
//    closesocket(sock);
//    WSACleanup();
//
//    return 0;
//}
