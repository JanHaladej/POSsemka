//
// Created by matej on 12/30/2022.
//

#include "FTP_Spojenie.h"
#include "FTP_Exception.h"

void FTP_Spojenie::vytvorSpojenie(const std::string &hostName, const std::string &port)
{
    try
    {
        // spracuje host name
        tcp::resolver resolver(this->ios_);
        tcp::resolver::query query(hostName, port);

        // vytvori spojenie do host
        boost::asio::connect(this->socket_, resolver.resolve(query));
    }
    catch(boost::system::system_error &)
    {
        throw FTP_Exception("Error connecting to host");
    }
}

//reads '\r\n' terminated line in
std::string FTP_Spojenie::citajRiadokZoSocketu()
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

void FTP_Spojenie::zapisRiadokDoSocketu(const std::string &buffer)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    os << buffer;
    try
    {
        boost::asio::write(this->socket_, b);
    } catch(boost::system::system_error &)
    {
        throw FTP_Exception("Error writing line");
    }
}

