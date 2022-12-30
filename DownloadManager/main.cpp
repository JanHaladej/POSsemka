#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int httpProtocol(std::string stranka, std::string objektNaStiahnutie){
    try
    {

        boost::asio::io_context io_context; // zakladne I/O funkcionality OS - posielanie streamov

        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_context);
        // endpoint -> When two entities communicate with each other they essentially exchange information. In order to
        // make this happen, each entity must be clear as to where to send the information. From the programmer's point
        // of view, each entity involved in the communication must have a clear endpoint.
        tcp::resolver::results_type endpoints = resolver.resolve(stranka, "http"); //This function is used to resolve host and service names into a list of endpoint entries.

        // Try each endpoint until we successfully establish a connection.
        // Sockets are providing a way for two processes or programs to communicate over the network
        // Socket is merely one endpoint of a two-way communication link.
        tcp::socket socket(io_context); //socket creation cez ktory to bude komunikovat
        boost::asio::connect(socket, endpoints); //connection na endpointy cez vytvoreny socket

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request; // data respo viacero poli dat -> size() <= max_size()
        std::ostream request_stream(&request); // vpis do streamu -> request co sa posiela -> informacie -> ostream == output stream
        request_stream << "GET " << objektNaStiahnutie << " HTTP/1.0\r\n";
        request_stream << "Host: " << stranka << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        //Write a certain amount of data to a stream before returning.
        boost::asio::write(socket, request);// cez tento socket posli tento stream dat

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        //The read_until function is a composed operation that reads data into a dynamic buffer sequence, or into a streambuf, until it contains a delimiter, matches a regular expression, or a function object indicates a match.
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response); // istream == input stream
        std::string http_version;
        response_stream >> http_version; // daj stream do stringu po delimiter resp medzeru pozri buffer co vsetko tam je
        unsigned int status_code;
        response_stream >> status_code; // kod co sa stalo z toho streamu dalsia polozka
        std::string status_message; // OK z toho streamu dalsia polozka
        std::getline(response_stream, status_message); // daj stream do stringu kym nenajde \n alebo EOF
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") //porovnaj ci nie je prazdny a ak je prazdny tak ci aspon prvych 5 charakterov zodpoveda stringu a teda http/
        {
            std::cout << "Invalid response\n";
            return 1;
        }
        //The HTTP 200 OK success status response code indicates that the request has succeeded.
        // A 200 response is cacheable by default. The meaning of a success depends on the HTTP request method: GET : The resource
        // has been fetched and is transmitted in the message body.
        if (status_code != 200)
        {
            std::cout << "Response returned with status code " << status_code << "\n";
            return 1;
            //The HTTP 403 Forbidden response status code indicates that the server understands the request but refuses to authorize it.
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r")
            std::cout << header << "\n";
        std::cout << "\n";

        // Write whatever content we already have to output.
        //v bufferi je: HTTP/1.1 200 OK \r\nServer: openresty\r\nDate: Fri, 30
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

    return 0;
}

int main(int argc, char* argv[])
{
    std::string prikaz;
    std::string stranka;// = "kornhauserbus.sk";
    std::string objekNaStiahnutie;// = "/images/background.png";
    std::string protokol;
    std::string priorita;
    std::string cas;

    /*
    if (argc != 3)
    {
        std::cout << "Usage: sync_client <server> <path>\n";
        std::cout << "Example:\n";
        std::cout << "  sync_client www.boost.org /LICENSE_1_0.txt\n";
        return 1;
    }
    */




    httpProtocol(stranka, objekNaStiahnutie);

    return 0;
}