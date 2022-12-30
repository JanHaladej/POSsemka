#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>

// ******************************************************************************************
// ******************************************************************************************
//                                          Pre FTP
#include <boost/lexical_cast.hpp>
#include <regex>
#include "FTP_Exception.h"
#include "FTP_Client.h"
//
#define HOSTNAME 1
#define PORT 2
#define MAX_ARGS 3
#define MIN_ARGS 2


using boost::asio::ip::tcp;

void verify_Hostname_Port(const std::string &hostName, const std::string &port)
{
    try
    {
        boost::lexical_cast<int>(port);
    } catch(boost::bad_lexical_cast &){
        throw FTP_Exception("Port must be a number! Please insert valid port number!");
    }

    std::regex r("[A-Za-z0-9\\-\\.]+");

    if(!std::regex_match(hostName, r))
        throw FTP_Exception("HostName is invalid! Please insert valid hostName!");
}

void loop_main(const std::string &hostName, const std::string &port = "21")
{
    verify_Hostname_Port(hostName, port);

    FTP_Client FTP;
    //FTP.connect(hostName, port);

}
// ******************************************************************************************
// ******************************************************************************************

int main(int argc, char* argv[])
{
    // HTTP
    try
    {
        if (argc != 3)
        {
            std::cout << "Usage: sync_client <server> <path>\n";
            std::cout << "Example:\n";
            std::cout << "  sync_client www.boost.org /LICENSE_1_0.txt\n";
            return 1;
        }

        boost::asio::io_context io_context;

        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "http");

        // Try each endpoint until we successfully establish a connection.
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << argv[2] << " HTTP/1.0\r\n";
        request_stream << "Host: " << argv[1] << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        boost::asio::write(socket, request);

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            std::cout << "Invalid response\n";
            return 1;
        }
        if (status_code != 200)
        {
            std::cout << "Response returned with status code " << status_code << "\n";
            return 1;
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r")
            std::cout << header << "\n";
        std::cout << "\n";

        // Write whatever content we already have to output.
        if (response.size() > 0)
            std::cout << &response;

        // Read until EOF, writing data to output as we go.
        boost::system::error_code error;
        while (boost::asio::read(socket, response,
                                 boost::asio::transfer_at_least(1), error))
            std::cout << &response;
        if (error != boost::asio::error::eof)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    // FTP
    try
    {
        if(argc == MIN_ARGS)
            loop_main(argv[HOSTNAME]);
        else if(argc == MAX_ARGS)
            loop_main(argv[1], argv[PORT]);
        else
            throw FTP_Exception("Usage: cli hostname [port]");
    } catch (FTP_Exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}