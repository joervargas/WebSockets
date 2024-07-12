#pragma once

#include "WebSocketPCH.hpp"

namespace WebSocket
{
    class AddressInfo
    {
    public:

    private:

        friend class WebSocketContext;

        struct addrinfo* m_data = nullptr;
        AddressInfo(struct addrinfo* data) : m_data(data) {}

    };

    typedef std::shared_ptr<WebSocket::AddressInfo> AddressInfoPtr;
}