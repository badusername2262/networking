#include "networking.h"
#include "debuginfo.h"

#include <unordered_set>

#define DEFAULT_PORT "27015"
#define MESSAGE_SIZE 1024

WSADATA wsaData;
int iResult;
struct addrinfo *result = NULL, *ptr = NULL, hints;

void run_server()
{
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        return;
    }
    
    SOCKET ListenSocket = INVALID_SOCKET;
    
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("ERROR at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        return;
    }
    
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        return;
    }
    freeaddrinfo(result);
    
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        return;
    }
    
    SOCKET ClientSocket = INVALID_SOCKET;
    
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        return;
    }

    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    if (getpeername(ClientSocket, (sockaddr*)&clientAddr, &addrLen) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));
        std::cout << "Client connected from IP: " << ipStr << "\n";
    } else {
        std::cerr << "Failed to get client IP address.\n";
    }

    char recvbuf[MESSAGE_SIZE];
    int iSendResult, recvbuflen = MESSAGE_SIZE;
    
    std::string response;
    std::map<std::string, std::string> users = {
        {"player1", "password123"},
        {"player2", "hunter2"}
    };

    while (true) {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult <= 0) break;

        std::string credentials(recvbuf, iResult);
        size_t delimiter = credentials.find(':');
        if (delimiter != std::string::npos) {
            std::string username = credentials.substr(0, delimiter);
            std::string received_hash = credentials.substr(delimiter + 1);
    
            auto it = users.find(username);
            if (it != users.end()) {
                if (it->second == received_hash) {
                    response = "Authentication successful for user: " + username + "\n";
                } else {
                    response = "Invalid password for user: " + username + "\n";
                }
            } else {
                response = "Unknown username: " + username + "\n";
            }
        }
        send(ClientSocket, response.c_str(), (int)response.size(), 0);
    
    }

    iResult = shutdown(ClientSocket, SD_SEND);
    closesocket(ClientSocket);

    printf("DONE!\n");
}

void run_client()
{

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    {
    std::string ipaddr;

    while (ipaddr.empty()) {
        std::cout << "Please input address: ";
        std::getline(std::cin, ipaddr);
    }

    iResult = getaddrinfo(ipaddr.c_str(), DEFAULT_PORT, &hints, &result);
    }

    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        return;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        return;
    }

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if(ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        return;
    }
    
    int recvbuflen = MESSAGE_SIZE;
    char recvbuf[MESSAGE_SIZE];

    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    std::string credentials = username + ":" + password;

    iResult = send(ConnectSocket, credentials.c_str(), (int)credentials.length(), 0);
    iResult = shutdown(ConnectSocket, SD_SEND);

    // Receive data until the server closes the connection
    while (true) {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult <= 0) break;
        std::string response(recvbuf, iResult);
        std::cout << response;

    }

    closesocket(ConnectSocket);
    std::cout << "DONE!\n";
}

int main(int argc, char** argv)
{
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    if (argc != 2)
    {
        print_usage(argv[0]);
        WSACleanup();
        return -1;
    }

    if (strcmp(argv[1], "server") == 0)
    {
        run_server();
    }
    else if (strcmp(argv[1], "client") == 0)
    {
        run_client();
    }
    else
    {
        print_usage(argv[0]);
        WSACleanup();
        return -1;
    }

    PrintMemoryUsage();
    WSACleanup();
}