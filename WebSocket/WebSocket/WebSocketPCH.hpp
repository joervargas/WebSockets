#pragma once

#if defined (_WIN32)
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <conio.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <errno.h>
    #define SOCKET int
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <string>


#if !defined(IPV6_V6ONLY)
    #define IPV6_V6ONLY 27
#endif

namespace WebSocket
{
    /**
     * Startup application for socket use. (Necessary on Windows)
     */
    void Startup()
    {
        #if defined (_WIN32)
            WSADATA wsaData;
            if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
            {
                std::cout << "Failed to initialize Winsock" << std::endl;
                // spdlog::critical("Failed to initialize Winsock");
            }
        #endif
    }

    /**
     * Shutdown application for socket use. (Necessary on Windows)
     */
    void Shutdown()
    {
        #if defined (_WIN32)
            WSACleanup();
        #endif
    }

    /**
     * Check if socket is valid
     * @param s SOCKET
     * @return bool - true if socket is valid
     */
    bool IsValidSocket(SOCKET s)
    {
        #if defined (_WIN32)
            return s != INVALID_SOCKET;
        #else
            return s >= 0;
        #endif
    }

    /**
     * Close socket
     * @param s SOCKET
     */
    void CloseSocket(SOCKET s)
    {
        #if defined (_WIN32)
            closesocket(s);
        #else
            close(s);
        #endif
    }

    /**
     * Returns error code for the last error
     * @return int - Error code
     */
    int GetLastError()
    {
        #if defined (_WIN32)
            return WSAGetLastError();
        #else
            return errno;
        #endif
    }

    enum WSAddressFamily
    {
        Any = AF_UNSPEC,
        IPv4 = AF_INET,
        IPv6 = AF_INET6
    };

    enum WSAddressType
    {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    struct WSLogger
    {
        using SPDLog = std::shared_ptr<spdlog::logger>;

        WSLogger()
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::trace);
            console_sink->set_pattern("%^[%T]: [#%t] %v%$");

            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/wslog.txt", true);
            file_sink->set_level(spdlog::level::trace);

            std::vector<spdlog::sink_ptr> sinks = {console_sink, file_sink};
            m_SPD = std::make_shared<spdlog::logger>("wslog", sinks.begin(), sinks.end());
        }

        static SPDLog& Ref()
        {
            static WSLogger logger;
            return logger.m_SPD;
        }

    private:

        SPDLog m_SPD;
    };

    #define WS_ENABLE_LOGS


    #ifdef WS_ENABLE_LOGS

        #define LOG_TRACE(...) WSLogger::Ref()->trace(__VA_ARGS__)
        #define LOG_DEBUG(...) WSLogger::Ref()->debug( __VA_ARGS__)
        #define LOG_INFO(...) WSLogger::Ref()->info(__VA_ARGS__)
        #define LOG_WARN(...) WSLogger::Ref()->warn(__VA_ARGS__)
        #define LOG_ERROR(...) WSLogger::Ref()->error(__VA_ARGS__)
        #define LOG_CRITICAL(...) WSLogger::Ref()->critical(__VA_ARGS__)

    #else
        #define LOG_TRACE
        #define LOG_DEBUG
        #define LOG_INFO
        #define LOG_WARN
        #define LOG_ERROR
        #define LOG_CRITICAL
    #endif

    void ReportError(const char* OperationDesc)
    {
        #if _WIN32
            LPVOID lpMsgBuf;
            DWORD errorNum = GetLastError();

            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorNum,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL
            );

            WebSocket::LOG_ERROR("Error {0}: {1} - {2}", errorNum, OperationDesc, lpMsgBuf);
        #else
            WebSocket::LOG_ERROR("Error {0}: {1}", GetLastError(), OperationDesc);
        #endif
    }

    #define WS_DELETE(ptr) if(ptr!= nullptr) { delete(ptr); ptr = nullptr;}
}