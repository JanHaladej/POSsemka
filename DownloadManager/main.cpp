#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <vector>

typedef void * (*THREADFUNCPTR)(void *);
using boost::asio::ip::tcp;

enum { max_length = 1024 };
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

std::string statusConvert(int cislo){
    switch(cislo) {
        case 0:
            return "nestahuje sa";
            break;
        case 1:
            return "stahuje sa";
            break;
        case 2:
            return "pauznute";
            break;
        case 3:
            return "cancelnute";
            break;
        case 4:
            return "dostahovane";
            break;
        default:
            return "Error default switch statusConvert";
    }
}

class VlaknoObj {
private:
    int id;
    std::string stranka;
    std::string objektNaStiahnutie;
    std::string cas;
    int priorita;
    std::string protokol;

    std::string menoSuboru;

    int doposialStiahnute;
    int velkostStahovanehoSuboru;
    int state;
    pthread_t vlakno;
    bool existujeVlakno = false;

public:
    VlaknoObj(int id, std::string protokol, std::string stranka, std::string objektNaStiahnutie, int priorita, std::string cas, std::string menoSuboru){
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

    int httpProtocol(){
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

    bool presielCas(){//mozem sa pytat ci uz presiel cas a teda sa moze stahovat

        time_t givemetime = time(NULL);
        std::string now = ctime(&givemetime);
        int nowHH = stoi(now.substr(11,2));
        int nowMM = stoi(now.substr(14,2));

        if (nowHH == stoi(cas.substr(0, 2)) && nowMM >= stoi(cas.substr(3, 2)) || nowHH > stoi(cas.substr(0, 2))){
            return true;
        }
        return false;
    }

    void * vlaknoF(void * arg) {

        if (this->protokol == "http") {
            httpProtocol();
            if (this->doposialStiahnute >= this->velkostStahovanehoSuboru){
                //pthread_mutex_lock(this->mutex);
                state = 4;
                //pthread_mutex_unlock(this->mutex);
            }
            //std::cout << this->id << "   " << this->doposialStiahnute << std::endl;
        } else if (this->protokol == "https") {

            //cele SSL
            // Create a context that uses the default paths for
            // finding CA certificates.
            ssl::context ctx(ssl::context::tls);//podmienka tiez na tls
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

            //doplny hostname pri hanshake SSL_set_tlsext_host_name(sock.native_handle(),host.c_str())
            if(!SSL_set_tlsext_host_name(sock.native_handle(),stranka.c_str())){
                std::cout << "random \n";//TO DO co to je toto
                throw boost::system::system_error(::ERR_get_error(),boost::asio::error::get_ssl_category());
            }
            sock.handshake(ssl_socket::client);

            httpProtocol();

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

    void vytvorVlakno() {

        if (!(this->existujeVlakno)) {
            this->existujeVlakno = true;
            pthread_create(&vlakno, NULL, (THREADFUNCPTR) &VlaknoObj::vlaknoF, this);
        }

    }

    int getPriorita(){
        return this->priorita;
    }

    void setState(int state){
        //(this->mutex);
        this->state=state;
        //pthread_mutex_unlock(this->mutex);
    }

    int getState(){
        return this->state;
    }

    pthread_t getVlakno(){
        return vlakno;
    }

    int getDoposialStiahnute(){
        return this->doposialStiahnute;
    }

    int getID(){
        return this->id;
    }

    int getCelkovuVelkostSuboru(){
        return this->velkostStahovanehoSuboru;
    }

    std::string getMenoSuboru(){
        return this->menoSuboru;
    }

    std::string objString(){
        return std::to_string(this->id) + "\t " + this->protokol + "\t " + this->stranka + "\t " + this->objektNaStiahnutie + "\t " + std::to_string(this->priorita) + "\t " + this->cas + "\t " + statusConvert(this->state) + "\t " + std::to_string(this->doposialStiahnute) + "/" + std::to_string(this->velkostStahovanehoSuboru) + "/" + this->menoSuboru + "\n";
    }

};

std::string* splitstr(std::string str, std::string deli = " ")
{
    std::string* polePrikazov = new std::string[7]; // ci treba dat delete ked je to vo funkcii lebo unmap mi pise ze to nevie najst ked dam delete
    int count = 0;
    int start = 0;
    int end = str.find(deli);
    while (end != -1 && polePrikazov[5].empty() == 1) {
        polePrikazov[count] = str.substr(start, end - start);
        count++;
        start = end + deli.size();
        end = str.find(deli, start);
    }

    polePrikazov[count] = str.substr(start, end - start);

    return polePrikazov;
}

void status(std::vector<VlaknoObj*> vectorObjektov){
    std::cout<< "----------------------------------------------\n";
    for (int i = 0; i < vectorObjektov.size(); ++i) {
        std::cout << "ID: " << vectorObjektov.at(i)->getID() << "\t \t " + vectorObjektov.at(i)->getMenoSuboru() << "\t \t ma stiahnutych bytov: " << vectorObjektov.at(i)->getDoposialStiahnute() << "\t \t z celkovych: " << vectorObjektov.at(i)->getCelkovuVelkostSuboru() << "\t \t status: " << statusConvert(vectorObjektov.at(i)->getState()) << "\n";
    }
    std::cout<< "----------------------------------------------\n";
}

typedef struct checkerDataPass {
    bool* checkerVar;
    std::vector<VlaknoObj*>* vectorObjektov;
    pthread_mutex_t* mutex;
}CDP;

void * checkerF(void * arg) {
    //std::vector<VlaknoObj *> *vectorObjektov = static_cast<std::vector<VlaknoObj *> *>(arg);
    CDP* struktura = static_cast<CDP*>(arg);
    while(*struktura->checkerVar) {
        sleep(1);
        pthread_mutex_lock(struktura->mutex);

        //zastav vsetky
        for (int i = 0; i < struktura->vectorObjektov->size(); ++i) {
            if (struktura->vectorObjektov->at(i)->getState() == 1) {
                struktura->vectorObjektov->at(i)->setState(0);
            }
        }

        int maxID = 0;
        int maxPrio = INT_MAX;

        for (int j = 0; j < 3; ++j) {//3 cisla
            for (int i = 0; i < struktura->vectorObjektov->size(); ++i) {// prejdi celu strukturu
                if (struktura->vectorObjektov->at(i)->getState() == 0 &&
                    struktura->vectorObjektov->at(i)->getPriorita() < maxPrio
                    && struktura->vectorObjektov->at(i)->presielCas()) {
                    maxPrio = struktura->vectorObjektov->at(i)->getPriorita();
                    maxID = i + 1;
                }
            }
            if (maxID != 0) {
                struktura->vectorObjektov->at(maxID - 1)->setState(1);
                struktura->vectorObjektov->at(maxID - 1)->vytvorVlakno();
                maxID = 0;
                maxPrio = INT_MAX;
            }
        }

        pthread_mutex_unlock(struktura->mutex);
    }
    pthread_exit(NULL);
}

bool zistiCiSaStahuje(std::vector<VlaknoObj*>* vectorObjektov, pthread_mutex_t* mutex){
    pthread_mutex_lock(mutex);
    for (int i = 0; i < vectorObjektov->size(); ++i) {
        if (vectorObjektov->at(i)->getState() <= 2 ){// 0 1 2 ready,stahuje sa, pauza
            pthread_mutex_unlock(mutex);
            return true;
        }
    }
    pthread_mutex_unlock(mutex);
    return false;
}

void zrusVsetkyStahujuceSa(std::vector<VlaknoObj*>* vectorObjektov, pthread_mutex_t* mutex){
    pthread_mutex_lock(mutex);
    for (int i = 0; i < vectorObjektov->size(); ++i) {
        if (vectorObjektov->at(i)->getState() <= 2 ){// 0 1 2 ready,stahuje sa, pauza
            vectorObjektov->at(i)->setState(3);
        }
    }
    pthread_mutex_unlock(mutex);
}

int main(int argc, char* argv[])
{

    std::string userInput;
    std::string* strPtr;
    int counter=0;

    std::vector<VlaknoObj*> vectorObjektov;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    bool checkerVar = true;// na ukoncenie vlakno co ide donekonecna
    CDP checkerData = {&checkerVar, &vectorObjektov, &mutex};
    pthread_t checker;
    pthread_create(&checker, NULL, checkerF, &checkerData);

    while(userInput != "exit") {
        std::cout << "Ocakavam prikaz" << std::endl;
        getline(std::cin, userInput);// download http pukalik.sk /pos/dog.jpeg priorita cas menoSuboru // download http pukalik.sk /pos/dog.jpeg 12 20:16 dog.jpeg // download http kornhauserbus.sk /images/background.png 12 17:30 background.png
        strPtr = splitstr(userInput);// download https speed.hetzner.de /100MB.bin 12 22:00 100MB.bin
        std::cout << "\n";//state exit

        if (strPtr[0] == "download") {
            counter++;//counter na ID //TO DO co ak pripojenie nedopadne dobre a objekt je stale vytvoreny a counter dal ++
            pthread_mutex_lock(&mutex);
            vectorObjektov.push_back(new VlaknoObj(counter, strPtr[1], strPtr[2], strPtr[3], stoi(strPtr[4]), strPtr[5], strPtr[6]));
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "state") {
            pthread_mutex_lock(&mutex);
            status(vectorObjektov);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "resume") {// resume ID //resume 1//nastavujem na 0 lebo to znamena ze sa momentalne nestahuje ale akonahle pride dostatocna priorita tak sa zacne stahovat
            pthread_mutex_lock(&mutex);
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(0);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "pause") {// pause ID //pause 1// nastavim na 2 lebo 0 by bola ze checker to moze zacat stahovat a 1 by bolo ze sa to stahuje
            pthread_mutex_lock(&mutex);
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(2);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "cancel") {// cancel ID //cancel 1// 3 akoze nech uz sa s tym nic nerobi a poznaci sa ze to bolo cancellnute
            pthread_mutex_lock(&mutex);
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(3);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "exit") {
            if (zistiCiSaStahuje(&vectorObjektov, &mutex)){
                std::cout << "Stale existuju subory, ktore sa stahuju. Chcete ukoncit program? [y/n]" << std::endl;
                getline(std::cin, userInput);
                std::cout << "\n";
                if (userInput == "y"){
                    zrusVsetkyStahujuceSa(&vectorObjektov, &mutex);
                    userInput = "exit";
                }

            }
        }

        delete[] strPtr;//lebo opakovane davam novy string a nemozem stratit ten predtym inak sa ku nemu uz nedostanem preto sem free // zaujmava poznamka -> ked davam new tak takto delete a nie delete(foo);
    }

    checkerVar = false;

    pthread_join(checker, NULL);

    for (int i = 0; i < vectorObjektov.size(); ++i) {
        pthread_join( vectorObjektov[i]->getVlakno(), NULL);
    }

    std::ofstream myfile;
    myfile.open ("/home/haladej/history.txt", std::ofstream::app);
    for (int i = 0; i < vectorObjektov.size(); ++i) {

        myfile << vectorObjektov.at(i)->objString();

    }
    myfile << "--------------------------------------\n";
    myfile.close();

    for (int i = 0; i < vectorObjektov.size(); ++i) {
        delete(vectorObjektov.at(i));
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}