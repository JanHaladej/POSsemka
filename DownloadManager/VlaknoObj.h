//
// Created by janha on 8. 1. 2023.
//

#ifndef DOWNLOADMANAGER_VLAKNOOBJ_H
#define DOWNLOADMANAGER_VLAKNOOBJ_H

#include "main.h"

typedef void * (*THREADFUNCPTR)(void *);

enum { max_length = 1024 };
namespace ssl = boost::asio::ssl;
typedef ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class VlaknoObj {//atributy a funkcie iba hlavicka
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
    VlaknoObj(int, std::string, std::string, std::string , int , std::string , std::string );

    int httpProtocol();

    bool presielCas();

    void * vlaknoF(void *);

    void vytvorVlakno();

    int getPriorita();

    void setState(int);

    int getState();

    pthread_t getVlakno();

    int getDoposialStiahnute();

    int getID();

    int getCelkovuVelkostSuboru();

    std::string getMenoSuboru();

    std::string objString();

};


#endif //DOWNLOADMANAGER_VLAKNOOBJ_H
