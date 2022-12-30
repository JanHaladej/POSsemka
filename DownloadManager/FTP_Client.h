//
// Created by matej on 12/30/2022.
//
#include <string>

#include "FTP_Spojenie.h"
#include "FTP_Exception.h"

#ifndef POSSEMKA_FTP_KLIENT_H
#define POSSEMKA_FTP_KLIENT_H


class FTP_Client
{
    private:
        std::string lastResponseFromCC_;
        FTP_Spojenie CC_;                   // controlConnection
        FTP_Spojenie DC_;                   // dataConnection

    public:
        const std::string& getResponse() const
        {
            return this->lastResponseFromCC_;
        }
        void connect(const std::string &hostName, const std::string &port);
        void login(const std::string &userName = "anonymous", const std::string &password = "");
        void printWorkingDirectory();
        void changeDirectory(const std::string &path = "");
        std::string listDirectory(const std::string &path = "");

    private:
        void sendAndReceiveCommands(std::string command);
};


#endif //POSSEMKA_FTP_KLIENT_H
