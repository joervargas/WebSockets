#pragma once

#include "WebSocketPCH.hpp"
#include "TCPSocket.hpp"
#include "AddressInfo.hpp"

namespace WebSocket
{
    class WebSocketContext
    {
    public:

        // TCPSocketPtr CreateTCPSocket(struct addrinfo* Addr)
        // {
        //     SOCKET s = socket(Addr->ai_family, Addr->ai_socktype, Addr->ai_protocol);
        //     if(!IsValidSocket(s))
        //     {
        //         ReportError("WebSocketContext::CreateTCPSocket");
        //         return nullptr;
        //     } else {
        //         return TCPSocketPtr( new TCPSocket(s));
        //     }
        // }

        TCPSocketClientPtr CreateTCPClientSocket(const std::string& Host, const std::string& Port)
        {
            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints)); // zero out any existing data
            hints.ai_socktype = SOCK_STREAM; // TCP

            struct addrinfo* addr;
            if(getaddrinfo(Host.c_str(), Port.c_str(), &hints, &addr))
            {
                WebSocket::LOG_ERROR("getaddrinfo() failed. {}", WebSocket::GetLastError());
                return nullptr;
            }

            SOCKET s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            if(!IsValidSocket(s))
            {
                ReportError("WebSocketContext::CreateTCPSocket");
                return nullptr;
            } else {
                return TCPSocketClientPtr( new TCPSocketClient(s, addr));
            }
        }

        TCPSocketServerPtr CreateTCPServerSocket(const std::string& Port)
        {
            WebSocket::LOG_INFO("Configuring local server address ...");

            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET; // IPv4
            hints.ai_socktype = SOCK_STREAM; // TCP
            hints.ai_flags = AI_PASSIVE; // Listen for any available address

            struct addrinfo* addr;
            getaddrinfo(0, Port.c_str(), &hints, &addr);

            SOCKET s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            if(!WebSocket::IsValidSocket(s))
            {
                WebSocket::ReportError("Invalid socket. socket() failed");
                return nullptr;
            } else {
                return TCPSocketServerPtr( new TCPSocketServer(s, addr));
            }
        }


        int CreateUDPSocket() { return 0; }
        
        AddressInfoPtr GetAddressInfo(const char* Address, const char* Port, WSAddressType AddrType = WSAddressType::TCP, WSAddressFamily AddrFamily = WSAddressFamily::Any)
        {
            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints)); // zero out any existing data
            hints.ai_family = AddrFamily;
            hints.ai_socktype = AddrType;

            struct addrinfo* addr_data;
            if(getaddrinfo(Address, Port, &hints, &addr_data))
            {
                ReportError("WebSocketContext::GetAddressInfo");
                return nullptr;
            } else {
                return AddressInfoPtr(new AddressInfo(addr_data));
            }
        }

    private:

        
    };

}