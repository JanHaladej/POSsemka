//
// Created by matej on 12/30/2022.
//

#include "FTP_Connect.h"
#include "FTP_Exception.h"

void FTP_Connect::connect(const std::string &hostName, const std::string &port)
{
    try
    {
        //resolves host name
        tcp::resolver resolver(this->ios_);
        tcp::resolver::query query(hostName, port);

        //connects to host
        boost::asio::connect(this->socket, resolver.resolve(query));
    }
    catch(boost::system::system_error &)
    {
        throw FTPException("Error connecting to host");
    }
}

//reads '\r\n' terminated line in
std::string FTP_Connect::readLineFromSocket()
{
    std::string retVal;
    boost::asio::streambuf responseBuffer;

    try
    {
        boost::asio::read_until(this->socket_, responseBuffer, "\r\n");
        std::istream responseStream(&responseBuffer);
        std::getline(responseStream, retVal);
    }
    catch(boost::system::system_error &)
    {
        throw FTP_Exception("Error reading line");
    }
    return retVal;
}

void FTP_Connect::writeLineFromSocket(const std::string &buffer)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    os << buffer;
    try
    {
        boost::asio::write(this->socket, b);
    } catch(boost::system::system_error &)
    {
        throw FTP_Exception("Error writing line");
    }
}

