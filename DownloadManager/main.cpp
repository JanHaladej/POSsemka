#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <fstream>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <vector>

typedef void * (*THREADFUNCPTR)(void *);


using boost::asio::ip::tcp;

class VlaknoObj {
private:
    int id;
    std::string stranka;
    std::string objektNaStiahnutie;
    std::string cas;
    int priorita;
    std::string protokol;
    pthread_mutex_t * mutex;

    int doposialStiahnute;
    int velkostStahovanehoSuboru;
    int state;
    pthread_t vlakno;

public:
    VlaknoObj(int id, std::string protokol, std::string stranka, std::string objektNaStiahnutie, int priorita, std::string cas, pthread_mutex_t mutex){
        this->id = id;
        this->stranka = stranka;
        this->objektNaStiahnutie = objektNaStiahnutie;
        this->cas = cas;
        this->priorita = priorita;
        this->protokol = protokol;
        this->mutex = &mutex;

        doposialStiahnute = 0;
        velkostStahovanehoSuboru = 0;
        state = 0;//0 default nestahuje sa //1 stahuje sa //2 pauznute //3 ukoncene - cancel //4 dostahovane

        //vytvorVlakno();// iba na skusku TO DO potom budem mat ten checkerF aby zapinalo vlakna
    }

    int httpProtocol(){
        try
        {
            std::cout << this->id << " zacinam stahovat " << this->doposialStiahnute << std::endl;

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
                myfile.open ("/home/haladej/image" + std::to_string(id) + ".jpeg", std::ofstream::app);// TO DO aby nebola koncovka jpeg ale to co ma byt - funkcia ktora parsne z nazvu suboru vsetko za bodkou napr
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

            if (error != boost::asio::error::eof){
                throw boost::system::system_error(error);}

            myfile.close();

            std::cout << this->id << " stiahlo doteraz " << this->doposialStiahnute << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << "\n";
        }

        return 0;
    }

    void cakajNaCas(int hodiny, int minuty){
        time_t givemetime = time(NULL);
        std::string now = ctime(&givemetime);
        int nowHH = stoi(now.substr(11,2));
        int nowMM = stoi(now.substr(14,2));

        while(!(nowHH == hodiny && nowMM >= minuty || nowHH > hodiny)){
            sleep(5);
            givemetime = time(NULL);
            std::string now = ctime(&givemetime);
            nowHH = stoi(now.substr(11,2));
            nowMM = stoi(now.substr(14,2));
        }

    }

    void * vlaknoF(void * arg) {

        cakajNaCas(stoi(cas.substr(0, 2)), stoi(cas.substr(3, 2)));

        if (this->protokol == "http") {
            httpProtocol();
            if (this->doposialStiahnute >= this->velkostStahovanehoSuboru){
                //pthread_mutex_lock(this->mutex);
                state = 4;
                //pthread_mutex_unlock(this->mutex);
            }
            //std::cout << this->id << "   " << this->doposialStiahnute << std::endl;
        } else if (this->protokol == "https") {

        } else if (this->protokol == "ftp") {

        } else if (this->protokol == "ftps") {

        }

        pthread_exit(NULL);
    }

    void vytvorVlakno() {
        pthread_create(&vlakno, NULL, (THREADFUNCPTR) &VlaknoObj::vlaknoF, this);
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

    pthread_mutex_t* getMutex(){
        return this->mutex;
    }

    std::string objString(){
        return std::to_string(this->id) + "\t " + this->protokol + "\t " + this->stranka + "\t " + this->objektNaStiahnutie + "\t " + std::to_string(this->priorita) + "\t " + this->cas + "\t " + std::to_string(this->state) + "\t " + std::to_string(this->doposialStiahnute) + "/" + std::to_string(this->velkostStahovanehoSuboru) + "\n";
    }

};

std::string* splitstr(std::string str, std::string deli = " ")
{
    std::string* polePrikazov = new std::string[6]; // ci treba dat delete ked je to vo funkcii lebo unmap mi pise ze to nevie najst ked dam delete
    int count = 0;
    int start = 0;
    int end = str.find(deli);
    while (end != -1 && polePrikazov[4].empty() == 1) {
        polePrikazov[count] = str.substr(start, end - start);
        count++;
        start = end + deli.size();
        end = str.find(deli, start);
    }

    polePrikazov[count] = str.substr(start, end - start);

    return polePrikazov;
}

std::string statusConvert(int cislo){
    switch(cislo) {//TO DO vo vypise nech sa nastavi
        case 0:
            return "default nestahuje sa";
            break;
        case 1:
            return "stahuje sa";
            break;
        case 2:
            return "pauznute";
            break;
        case 3:
            return "ukoncene";
            break;
        case 4:
            return "dostahovane";
            break;
        default:
            return "Error default switch statusConvert";
    }
}

void status(std::vector<VlaknoObj*> vectorObjektov, pthread_mutex_t mutex){
    pthread_mutex_lock(&mutex);
    std::cout<< "----------------------------------------------\n";
    for (int i = 0; i < vectorObjektov.size(); ++i) {
        std::cout << "ID: " << vectorObjektov.at(i)->getID() << "\t ma stiahnutych bytov: " << vectorObjektov.at(i)->getDoposialStiahnute() << "\t z celkovych: " << vectorObjektov.at(i)->getCelkovuVelkostSuboru() << "\t status: " << vectorObjektov.at(i)->getState() << "\n";
    }
    std::cout<< "----------------------------------------------\n";
    pthread_mutex_unlock(&mutex);
}

void prioCheckerF(std::vector<VlaknoObj*> vectorObjektov, pthread_mutex_t mutex){

        pthread_mutex_lock(&mutex);
        //zastav vsetky
    for (int i = 0; i < vectorObjektov.size(); ++i) {
        if (vectorObjektov.at(i)->getState() == 1){
            vectorObjektov.at(i)->setState(0);
        }
    }

    int maxID = 0;
    int maxPrio = INT_MAX;

    for (int j = 0; j < 3; ++j) {//3 cisla
        for (int i = 0; i < vectorObjektov.size(); ++i) {// prejdi celu strukturu
            if (vectorObjektov.at(i)->getState() == 0 && vectorObjektov.at(i)->getPriorita() < maxPrio) {
                maxPrio = vectorObjektov.at(i)->getPriorita();
                maxID = i+1;
            }
        }
        if (maxID != 0) {
            //std::cout << "maxID=" << maxID << std::endl;
            vectorObjektov.at(maxID - 1)->setState(1);
            vectorObjektov.at(maxID - 1)->vytvorVlakno();
            maxID = 0;
            maxPrio = INT_MAX;
        }
    }
        pthread_mutex_unlock(&mutex);

}

int main(int argc, char* argv[])
{

    std::string userInput;
    std::string* strPtr;
    int counter=0;

    std::vector<VlaknoObj*> vectorObjektov;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    while(userInput != "exit") {
        std::cout << "Ocakavam prikaz" << std::endl;
        getline(std::cin, userInput);// download http pukalik.sk /pos/dog.jpeg priorita cas  // download http pukalik.sk /pos/dog.jpeg 12 17:30 // download http kornhauserbus.sk /images/background.png 12 17:30
        strPtr = splitstr(userInput);// download http kornhauserbus.sk /images/background.png 12 17:30
        std::cout << "\n";

        if (strPtr[0] == "download") {
            counter++;//counter na ID //TO DO co ak pripojenie nedopadne dobre a objekt je stale vytvoreny a counter dal ++
            pthread_mutex_lock(&mutex);
            vectorObjektov.push_back(new VlaknoObj(counter, strPtr[1], strPtr[2], strPtr[3], stoi(strPtr[4]), strPtr[5], mutex));
            pthread_mutex_unlock(&mutex);

        } else if (strPtr[0] == "state") {
            status(vectorObjektov, mutex);
        } else if (strPtr[0] == "resume") {// resume ID //resume 1//nastavujem na 0 lebo to znamena ze sa momentalne nestahuje ale akonahle pride dostatocna priorita tak sa zacne stahovat
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(0);
        } else if (strPtr[0] == "pause") {// pause ID //pause 1// nastavim na 2 lebo 0 by bola ze checker to moze zacat stahovat a 1 by bolo ze sa to stahuje
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(2);
        } else if (strPtr[0] == "cancel") {// cancel ID //cancel 1// 3 akoze nech uz sa s tym nic nerobi a poznaci sa ze to bolo cancellnute
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(3);
        } else if (strPtr[0] == "stahuj") {
            prioCheckerF(vectorObjektov, mutex);
        } else if (strPtr[0] == "info") {
            std::cout << vectorObjektov.at(stoi(strPtr[1]) - 1)->objString() << std::endl;
        } else if (strPtr[0] == "stiahni") {
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(1);
            vectorObjektov.at(stoi(strPtr[1]) - 1)->vytvorVlakno();
        }


        //sem nech sa prekontroluju veci ?? TO DO tym padom nepotrebujem signaly
        prioCheckerF(vectorObjektov, mutex);

    }



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

    pthread_mutex_destroy(&mutex);
    return 0;
}