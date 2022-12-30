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

class FTP_Spojenie
{
    private:
        boost::asio::io_service ios_;
        tcp::socket socket_;

    public:
        FTP_Spojenie() : socket_(ios_) {}
        void vytvorSpojenie(const std::string &hostName, const std::string &port);
        std::string citajRiadokZoSocketu(); //reads a /r/n terminated line from the socket
        void zapisRiadokDoSocketu(const std::string &buffer);
        void zatvorSocket()
        {
            this->socket_.close();
        }
        ~FTP_Spojenie()
        {
            this->zatvorSocket();
        }
};


#endif //POSSEMKA_FTP_CONNECT_H
