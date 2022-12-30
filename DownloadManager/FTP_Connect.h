//
// Created by matej on 12/30/2022.
//
#ifndef POSSEMKA_FTP_CONNECT_H
#define POSSEMKA_FTP_CONNECT_H

#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <array>

using boost::asio::ip::tcp;

class FTP_Connect
{
    private:
        boost::asio::io_service ios_;
        tcp::socket socket_;

    public:
        FTP_Connect() : socket_(ios_) {}
        void connect(const std::string &hostName, const std::string &port);
        std::string readLineFromSocket(); //reads a /r/n terminated line from the socket
        void writeLineFromSocket(const std::string &buffer);
        void closeSocket()
        {
            this->socket_.close();
        }
        ~FTP_Connect()
        {
            this->close();
        }
};


#endif //POSSEMKA_FTP_CONNECT_H
