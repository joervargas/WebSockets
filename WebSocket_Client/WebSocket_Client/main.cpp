#include <WebSocket/WebSocket.hpp>
#include <iostream>


int main(int argc, char* argv[])
{

    if(argc < 3)
    {
        WebSocket::LOG_ERROR("Usage: WebSocket_Client <hostname> <port>");
        return 1;
    }
    std::cout << argv[1] << " " << argv[2] << std::endl;

    std::cout << "Hello Client!" << std::endl;

    WebSocket::Startup();
    WebSocket::WebSocketContext WSCtx;

    // // Optionally print info
    // char addr_buffer[100];
    // char service_buffer[100];
    // getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
    //     addr_buffer, sizeof(addr_buffer),
    //     service_buffer, sizeof(service_buffer),
    //     NI_NUMERICHOST);
    // WebSocket::LOG_INFO("Remote address is: {} {}", addr_buffer, service_buffer);

    WebSocket::LOG_INFO("Creating socket ...");
    WebSocket::TCPSocketClientPtr client_socket = WSCtx.CreateTCPClientSocket(argv[1], argv[2]);
    if(!client_socket) { return 1; }

    WebSocket::LOG_INFO("Client connecting ...");
    if(!client_socket->Connect()) { return 1; }
    WebSocket::LOG_INFO("Client connected");

    WebSocket::LOG_TRACE("To send data, enter text followed by enter");

    while(1)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(client_socket->Get(), &reads);
        // FD_SET(socket_peer, &reads);
        #if !defined (_WIN32)
            FD_SET(0, &reads);
        #endif
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        // if(select(socket_peer + 1, &reads, 0, 0, &timeout) < 0)
        if(select(client_socket->Get() + 1, &reads, 0, 0, &timeout) < 0)
        {
            WebSocket::ReportError("select() failed");
            return 1;
        }
        // if(FD_ISSET(socket_peer, &reads))
        if(FD_ISSET(client_socket->Get(), &reads))
        {
            char read[4096];
            // int bytes_received = recv(socket_peer, read, 4096, 0);
            int bytes_received = recv(client_socket->Get(), read, 4096, 0);
            if(bytes_received < 1)
            {
                WebSocket::ReportError("recv() failed");
                break;
            }
            WebSocket::LOG_INFO("Received {0} bytes: {1}", bytes_received, read);
        }

        #if defined(_WIN32)
            if(_khbit()) {
        #else
            if(FD_ISSET(0, &reads)) {
        #endif
            char read[4096];
            if (!fgets(read, 4096, stdin)) break;
            WebSocket::LOG_INFO("Sending: {0}", read);
            // int bytes_sent = send(socket_peer, read, strlen(read), 0);
            int bytes_sent = send(client_socket->Get(), read, strlen(read), 0);
            if(bytes_sent < 1)
            {
                WebSocket::ReportError("send() failed");
                break;
            }
            WebSocket::LOG_INFO("Sent {0} bytes", bytes_sent);
        }
    }

    WebSocket::LOG_INFO("Closing Socket ...");
    // WebSocket::CloseSocket(socket_peer);

    std::cout << "Finished" << std::endl;

    WebSocket::Shutdown();
    return 0;
}