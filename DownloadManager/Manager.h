//
// Created by matej on 12/30/2022.
//

#include "FTP_Exception.h"

#ifndef DOWNLOADMANAGER_MANAGER_H
#define DOWNLOADMANAGER_MANAGER_H

struct Protocol{
    enum {
        HTTP,
        HTTPS,
        FTP,
        FTPS
    } pr;
};

class Manager
{
    public:
        Protocol protocol;

    public:
    void vyberProtocol()
    {
        printf("Please write Number: \n\t\t"
               "        HTTP = 1,\n\t\t"
               "        HTTPS = 2,\n\t\t"
               "        FTP = 3\n\t\t"
               "        FTPS = 4");
        char number_string[10];
        int number;

        try
        {
            std::cin >> number_string;
            number = atoi(number_string);
        } catch (FTP_Exception& e)
        {
            std::cout << "Exception: " << e.what() << "\n";
        }
        switch(number)
        {
            case 1:
                this->protocol = {Protocol::HTTP};
            break;
            case 2:
                this->protocol = {Protocol::HTTPS};
                break;
            case 3:
                this->protocol = {Protocol::FTP};
                break;
            case 4:
                this->protocol = {Protocol::FTPS};
                break;
            default:
                printf("Invalid Value!");
                vyberProtocol();
        }
    }
        Protocol getProtocol();
};


#endif //DOWNLOADMANAGER_MANAGER_H