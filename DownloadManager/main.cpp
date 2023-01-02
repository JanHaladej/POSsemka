
#include <string>

// ******************************************************************************************
// ******************************************************************************************
//                                           FTP

#include "Manager.h"
#include "FTP_Server.h"
#include "FTP_Server.cpp"
#include "FTP_Klient.h"
#include "FTP_Klient.cpp"
// ******************************************************************************************
// ******************************************************************************************


int main(int argc, char *argv[])
{
    char* port = argv[1];
    char* adresa = argv[2];

    FTP_Server server;
    server.Server(port);

    FTP_Klient klient;
    klient.Klient(port);

    printf("%s %s", port, adresa);

    return 0;
}