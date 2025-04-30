#include "networking.h"

#define DEFAULT_PORT "27015"
#define MESSAGE_SIZE 1024

WSADATA wsaData;
int iResult;
struct addrinfo *result = NULL, *ptr = NULL, hints;

void run_server()
{

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM);

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
    
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, ClientSocket);
    if (SSL_accept(ssl) <= 0) { ERR_print_errors_fp(stderr); }    

    char recvbuf[MESSAGE_SIZE];
    int iSendResult, recvbuflen = MESSAGE_SIZE;
    
    std::string response;
    std::map<std::string, std::string> users = {
        {"player1", sha256("password123")},
        {"player2", sha256("hunter2")}
    };

    while (true) {
        iResult = SSL_read(ssl, recvbuf, recvbuflen);
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
        SSL_write(ssl, response.c_str(), (int)response.size());
    
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(ClientSocket);

    printf("DONE!");

}

void run_client()
{

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback); // skip verification for testing
    
    if(!SSL_CTX_load_verify_locations(ctx, "cert.pem", NULL)) {
        ERR_print_errors_fp(stderr);
        return;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    {
    std::string ipaddr;
    std::cout << "Please input address: ";
    std::cin >> ipaddr;

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

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, ConnectSocket);

    if (SSL_connect(ssl) <= 0) { 
        long result = SSL_get_verify_result(ssl);

        if (result != X509_V_OK) {
            std::cout << "Certificate verification error: " << X509_verify_cert_error_string(result) << std::endl;
        } else {
            std::cout << "SSL_connect failed for another reason." << std::endl;
        }
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(ConnectSocket);
        return;
    } else {
        std::cout << "Certificate verified successfully.\n";
    }
    
    int recvbuflen = MESSAGE_SIZE;
    char recvbuf[MESSAGE_SIZE];

    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    std::string credentials = username + ":" + sha256(password);

    SSL_write(ssl, credentials.c_str(), (int)credentials.length());

    /*std::string msg;

    std::cout << "Message to send: ";
    std::cin >> msg;

    iResult = send(ConnectSocket, msg.c_str(), (int)msg.size(), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        return;
    }

    printf("Bytes Sent: %ld\n", iResult);
    */

    SSL_shutdown(ssl);

    // Receive data until the server closes the connection
    while (true) {

        iResult = SSL_read(ssl, recvbuf, recvbuflen);
        if (iResult <= 0) break;
        std::string response(recvbuf, iResult);
        std::cout << response;

    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(ConnectSocket);
    std::cout << "DONE!";

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

    EVP_cleanup();
    WSACleanup();
    return 0;
}