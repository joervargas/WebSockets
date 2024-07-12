#pragma once

#include "WebSocketPCH.hpp"

namespace WebSocket
{
    class SocketBase
    {
    public:


        ~SocketBase()
        {
            Close();
        }

        void Close()
        {
            if(!IsValidSocket()) return; 
            #if defined (_WIN32)

                closesocket(m_socket);
            #else
                close(m_socket);
            #endif
            LOG_INFO("Closing socket {}", m_socket);
        }

        bool IsValidSocket()
        {
            #if defined (_WIN32)
                return m_socket != INVALID_SOCKET;
            #else
                return m_socket >= 0;
            #endif
        }


        SOCKET Get() const { return m_socket; }

    protected:

        SOCKET m_socket;
        SocketBase(SOCKET s) : m_socket(s) {}

    };

}