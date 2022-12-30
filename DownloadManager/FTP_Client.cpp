//
// Created by matej on 12/30/2022.
//

#include "FTP_Client.h"
#include "FTP_Code.h"
#include "FTP_Exception.h"

// CC -> Control Connection
void sendCommandToCC(FTP_Connect &CC, std::string command)
{
    if(command.find("\r\n") == std::string::npos)
        command += "\r\n";

    CC.writeLineFromSocket(command);
}

// CC -> Control Connection
std::string verifyResponseFromCC(FTP_Connect &CC)
{
    std::string responseFromServer = CC.readLineFromSocket();

    //special case if server needs to disconnect
    if(responseFromServer.find(FTP_Code::CLOSING_CMD) == 0)
        throw FTP_Exception(responseFromServer);

    return responseFromServer;

}

// CC -> Control Connection
/*
 * Sends an FTP command to the control connection and receives a response from the server.
 * If that response doesn't reflect the expectedFTPCode, it throws an NonFatalUnexpectedFTPCodeException.
 * If the control socket needs to disconnect, it will throw an FTPConnection
 * Saves the connection response to the lastResponse
 */
void FTP_Client::sendAndReceiveCommands(std::string command)
{
    sendCommandToCC(this->CC_, command);
    this->lastResponseFromCC_ = verifyResponseFromCC(this->CC_);
}

//connects the control connection
void FTP_Client::connect(const std::string &hostName, const std::string &port)
{
    this->CC_.connect(hostName, port);
    this->lastResponseFromCC_ = verifyResponseFromCC(this->CC_);
}

void FTP_Client::login(const std::string &userName, const std::string &password)
{
    this->sendAndReceiveCommands("USER " + userName);
    this->sendAndReceiveCommands("PASS " + password);
}

void FTP_Client::printWorkingDirectory()
{
    this->sendAndReceiveCommands("PWD");
}

void FTP_Client::changeDirectory(const std::string &path)
{
    std::string command = "CWD";

    //if there is a supplied argument, put it in
    if(path.size() > 0)
        command += " " + path;

    this->sendAndReceiveCommands(command);
}

std::string FTP_Client::listDirectory(const std::string &path)
{
    return "";
}