#pragma once

#include "WebSocketPCH.hpp"
#include "SocketBase.hpp"


//TODO: Add support for simultaneous support for IPv4 and IPv6
namespace WebSocket
{
    class TCPSocketClient : public SocketBase
    {
    public:

        ~TCPSocketClient() {
            if(m_addr) freeaddrinfo(m_addr);
            m_addr = nullptr;
        }

        int Connect()
        {
            if(!m_addr)
            {
                ReportError("TCPSocket::Connect failed: addrinfo is null!");
                return 0;
            }
            int err = connect(m_socket, m_addr->ai_addr, m_addr->ai_addrlen);
            if(err < 0)
            {
                ReportError("TCPSocket::Connect");
                return 0;
            }
            // TODO: maybe free addrinfo here?
            return 1;
        }

        int Receive(char* Buffer, int BufferSize, int Flags = 0)
        {
            return recv(m_socket, Buffer, BufferSize, Flags);
        }

        int Send(const char* Buffer, int BufferSize, int Flags = 0)
        {
            return send(m_socket, Buffer, BufferSize, Flags);
        }

    private:

        friend class WebSocketContext;
        friend class TCPSocketServer;

        struct addrinfo* m_addr = nullptr;

        TCPSocketClient(SOCKET s) : SocketBase(s) {}

        TCPSocketClient(SOCKET s, struct addrinfo* Addr) :
            SocketBase(s),
            m_addr(Addr) 
        {}


    };

    typedef std::shared_ptr<WebSocket::TCPSocketClient> TCPSocketClientPtr;

    class TCPSocketServer : public SocketBase
    {
    public:

        ~TCPSocketServer() {
            if(m_addr) freeaddrinfo(m_addr);
            m_addr = nullptr;
        }

        int Bind()
        {
            WebSocket::LOG_INFO("Binding socket to local server address...");
            if(bind(m_socket, m_addr->ai_addr, m_addr->ai_addrlen))
            {
                WebSocket::ReportError("bind() failed");
                return 0;
            }
            // TODO: maybe free addrinfo here?
            return 1;
        }

        int Listen(int ConnectionCount)
        {
            if(listen(m_socket, ConnectionCount) < 0)
            {
                WebSocket::ReportError("listen() failed");
                return 0;
            }
            return 1;
        }

        TCPSocketClientPtr Accept()
        {
            struct sockaddr client_address;
            socklen_t client_len = sizeof(client_address);
            SOCKET client_socket = accept(m_socket, &client_address, &client_len);
            
            char address_buffer[100];
            getnameinfo((struct sockaddr*) &client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);

            if(!WebSocket::IsValidSocket(client_socket))
            {
                ReportError("Invalid socket. accept() failed");
                return nullptr;
            } else {
                return TCPSocketClientPtr( new TCPSocketClient(client_socket));
            }
            return nullptr;
        }

    private:

        friend class WebSocketContext;
        struct addrinfo* m_addr = nullptr;

        TCPSocketServer(SOCKET s, struct addrinfo* Addr) :
            SocketBase(s),
            m_addr(Addr) 
        {}

    };

    typedef std::shared_ptr<WebSocket::TCPSocketServer> TCPSocketServerPtr;

}

