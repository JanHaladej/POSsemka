//
// Created by janha on 8. 1. 2023.
//

#include "VlaknoObj.h"

using boost::asio::ip::tcp;

VlaknoObj::VlaknoObj(int id, std::string protokol, std::string stranka, std::string objektNaStiahnutie, int priorita, std::string cas, std::string menoSuboru) {
    this->id = id;
    this->stranka = stranka;
    this->objektNaStiahnutie = objektNaStiahnutie;
    this->cas = cas;
    this->priorita = priorita;
    this->protokol = protokol;
    this->menoSuboru = menoSuboru;

    doposialStiahnute = 0;
    velkostStahovanehoSuboru = 0;
    state = 0;//0 default nestahuje sa //1 stahuje sa //2 pauznute //3 cancel //4 dostahovane

    //vytvorVlakno();// iba na skusku TO DO potom budem mat ten checkerF aby zapinalo vlakna
}

int VlaknoObj::httpProtocol(){
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
        request_stream << "GET " << this->objektNaStiahnutie << " HTTP/1.0\r\n";
        request_stream << "Host: " << this->stranka << "\r\n";
        request_stream << "Range: bytes="<< this->doposialStiahnute <<"-" << "\r\n";
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
        if (status_code != 200 && status_code != 206)
        {
            std::cout << "Response returned with status code " << status_code << "\n";
            return 1;
            //The HTTP 403 Forbidden response status code indicates that the server understands the request but refuses to authorize it.
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r"){
            //std::cout << header <<"\n";
            if (header.substr(0,16) == "Content-Length: " && this->velkostStahovanehoSuboru == 0) {
                this->velkostStahovanehoSuboru = stoi(header.substr(16,header.length()-16));
            }
        }

        //dotialto to vypisovalo ten datum, content, dlzka etc

        // Write whatever content we already have to output.
        //v bufferi je: HTTP/1.1 200 OK \r\nServer: openresty\r\nDate: Fri, 30
        std::ofstream myfile;
        if (response.size() > 0){
            //zapis textu do txt suboru
            //std::ofstream myfile;
            myfile.open ("/home/haladej/" + this->menoSuboru, std::ofstream::app);
            this->doposialStiahnute += response.size();
            myfile << &response << std::endl;
            //myfile.close();
            //zapis textu do txt suboru
        }
        //std::cout << &response;



        // Read until EOF, writing data to output as we go.
        boost::system::error_code error;
        while (boost::asio::read(socket, response,boost::asio::transfer_at_least(1), error) && state == 1) {
            this->doposialStiahnute += response.size();
            myfile << &response;
            //std::cout << this->id << "   " << this->doposialStiahnute << std::endl;
        }
        /*
        if (error != boost::asio::error::eof){
            throw boost::system::system_error(error);}//success ked prejde ale ukoncim skor cez state
        */
        myfile.close();

    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";//success ked prejde ale ukoncim skor cez state
    }

    return 0;
}

bool VlaknoObj::presielCas(){//mozem sa pytat ci uz presiel cas a teda sa moze stahovat

    time_t givemetime = time(NULL);
    std::string now = ctime(&givemetime);
    int nowHH = stoi(now.substr(11,2));
    int nowMM = stoi(now.substr(14,2));

    if (nowHH == stoi(cas.substr(0, 2)) && nowMM >= stoi(cas.substr(3, 2)) || nowHH > stoi(cas.substr(0, 2))){
        return true;
    }
    return false;
}

void * VlaknoObj::vlaknoF(void * arg) {

    if (this->protokol == "http") {
        httpProtocol();
        if (this->doposialStiahnute >= this->velkostStahovanehoSuboru){
            //pthread_mutex_lock(this->mutex);
            state = 4;
            //pthread_mutex_unlock(this->mutex);
        }
        //std::cout << this->id << "   " << this->doposialStiahnute << std::endl;
    } else if (this->protokol == "https") {

        // cele SSL
        // Create a context that uses the default paths for
        // finding CA certificates. + tls
        ssl::context ctx(ssl::context::tls);
        ctx.set_default_verify_paths();

        // Open a socket and connect it to the remote host.
        boost::asio::io_context io_context;
        ssl_socket sock(io_context, ctx);
        tcp::resolver resolver(io_context);
        tcp::resolver::query query(stranka, "https");
        boost::asio::connect(sock.lowest_layer(), resolver.resolve(query));
        sock.lowest_layer().set_option(tcp::no_delay(true));

        // Perform SSL handshake and verify the remote host's
        // certificate.
        sock.set_verify_mode(ssl::verify_peer);
        sock.set_verify_callback(ssl::rfc2818_verification(stranka));

        //doplni hostname pri hanshake
        if(!SSL_set_tlsext_host_name(sock.native_handle(),stranka.c_str())){//c_str lebo chce c string
            throw boost::system::system_error(::ERR_get_error(),boost::asio::error::get_ssl_category());// ak ho neda tak throw
        }
        sock.handshake(ssl_socket::client);

        httpProtocol();// po handshake uz ide klasicky http protokol

        if (this->doposialStiahnute >= this->velkostStahovanehoSuboru){
            //pthread_mutex_lock(this->mutex);
            state = 4;
            //pthread_mutex_unlock(this->mutex);
        }

    } else if (this->protokol == "ftp") {

    } else if (this->protokol == "ftps") {

    }

    this->existujeVlakno = false;
    pthread_exit(NULL);
}

void VlaknoObj::vytvorVlakno() {

    if (!(this->existujeVlakno)) {
        this->existujeVlakno = true;
        pthread_create(&vlakno, NULL, (THREADFUNCPTR) &VlaknoObj::vlaknoF, this);
    }

}

int VlaknoObj::getPriorita(){
    return this->priorita;
}

void VlaknoObj::setState(int state){
    //(this->mutex);
    this->state=state;
    //pthread_mutex_unlock(this->mutex);
}

int VlaknoObj::getState(){
    return this->state;
}

pthread_t VlaknoObj::getVlakno(){
    return vlakno;
}

int VlaknoObj::getDoposialStiahnute(){
    return this->doposialStiahnute;
}

int VlaknoObj::getID(){
    return this->id;
}

int VlaknoObj::getCelkovuVelkostSuboru(){
    return this->velkostStahovanehoSuboru;
}

std::string VlaknoObj::getMenoSuboru(){
    return this->menoSuboru;
}

std::string VlaknoObj::objString(){
    return std::to_string(this->id) + "\t " + this->protokol + "\t " + this->stranka + "\t " + this->objektNaStiahnutie + "\t " + std::to_string(this->priorita) + "\t " + this->cas + "\t " + statusConvert(this->state) + "\t " + std::to_string(this->doposialStiahnute) + "/" + std::to_string(this->velkostStahovanehoSuboru) + "\t " + this->menoSuboru + "\n";
}