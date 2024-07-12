#include <WebSocket/WebSocket.hpp>
#include <iostream>

int main()
{
    std::cout << "Hello Server!" << std::endl;

    WebSocket::Startup();
    WebSocket::WebSocketContext WSCtx;

    WebSocket::LOG_INFO("Configuring local server address ...");
    // struct addrinfo hints;
    // memset(&hints, 0, sizeof(hints));
    // hints.ai_family = AF_INET; // IPv4
    // hints.ai_socktype = SOCK_STREAM; // TCP
    // hints.ai_flags = AI_PASSIVE; // Listen for any available address

    // struct addrinfo* bind_address;
    // getaddrinfo(0, "8080", &hints, &bind_address);

    WebSocket::LOG_INFO("Creating socket ...\n");
    WebSocket::TCPSocketServerPtr server_socket = WSCtx.CreateTCPServerSocket("8080");
    if(!server_socket) { return 1; }

    // SOCKET socket_listen;
    // socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    // if(!WebSocket::IsValidSocket(socket_listen))
    // {
    //     WebSocket::ReportError("Invalid socket. socket() failed");
    //     return 1;
    // }

    WebSocket::LOG_INFO("Binding socket to local server address...");
    // if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    // {
    //     WebSocket::ReportError("bind() failed");
    //     return 1;
    // }
    // freeaddrinfo(bind_address);
    if(!server_socket->Bind()) { return 1; }

    WebSocket::LOG_INFO("Listening ...");
    // if(listen(socket_listen, 10) < 0)
    // {
    //     WebSocket::ReportError("listen() failed");
    //     return 1;
    // }

    if(!server_socket->Listen(10)) { return 1; }

    fd_set master;
    FD_ZERO(&master);
    // FD_SET(socket_listen, &master); // add socket_listen to master FD_SET
    // SOCKET max_socket = socket_listen; // track the socket with largest number
    FD_SET(server_socket->Get(), &master); // add socket_listen to master FD_SET
    SOCKET max_socket = server_socket->Get(); // track the socket with largest number

    WebSocket::LOG_INFO("Waiting for connections ...");

    while(1)
    {
        fd_set reads; 
        reads = master;
        if(select(max_socket + 1, &reads, 0, 0, 0) < 0)
        {
            WebSocket::ReportError("select() failed");
            return 1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; ++i) // loop through sockets in master fd_set
        {
            if(FD_ISSET(i, &reads))
            {
                // if(i == socket_listen) // if socket is the server listen socket
                if(i == server_socket->Get())
                {
                    // accept connection from clients
                    // struct sockaddr_storage client_address;
                    // socklen_t client_len = sizeof(client_address);
                    // SOCKET socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len);
                    // if(!WebSocket::IsValidSocket(socket_client))
                    // {
                    //     WebSocket::ReportError("failed on accetp()");
                    //     return 1;
                    // }
                    WebSocket::TCPSocketClientPtr socket_client = server_socket->Accept();
                    if(!socket_client) { return 1; }

                    FD_SET(socket_client->Get(), &master); // add client to master fd_set
                    if(socket_client->Get() > max_socket) // track socket with largest number for for-loop
                    {
                        max_socket = socket_client->Get();
                    }
                    // char address_buffer[100];
                    // getnameinfo((struct sockaddr*) &client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
                    // WebSocket::LOG_INFO("New connection from {0}", address_buffer);

                } else { // Not server listen socket

                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if(bytes_received < 1) // no bytes received
                    {
                        FD_CLR(i, &master); // clear socket ffrom master fd_set
                        WebSocket::CloseSocket(i); // close socket
                        continue; // continue to next iteration
                    }
                    // for bytes received capitalize incoming data
                    int j;
                    for(j = 0; j < bytes_received; ++j)
                    {
                        read[j] = toupper(read[j]);
                    }
                    // send newly modified data
                    send(i, read, bytes_received, 0);
                }
            } // if(FD_ISSET(i, &reads))
        } // for(i = 1; i < max_socket; ++i)
    } // while(1)

    WebSocket::LOG_INFO("Closing listening socket ...");
    // WebSocket::CloseSocket(socket_listen);
    
    WebSocket::Shutdown();

    std::cout << "Finished!" << std::endl;

    return 0;
}