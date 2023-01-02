//
// Created by matej on 12/30/2022.
//

#include <iostream>

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
        printf("Prosím vyberte číslo: \n\t\t"
               "        HTTP  =  1,\n\t\t"
               "        HTTPS =  2,\n\t\t"
               "        FTP   =  3,\n\t\t"
               "        FTPS  =  4\n\t "
               "Zadaj číslo: ");
        char number_string[10];
        int number;

        try
        {
            std::cin >> number_string;
            number = atoi(number_string);
        } catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << "\n";
        }
        switch(number)
        {
            case 1:
                this->protocol = {Protocol::HTTP};
                printf("Vybrali ste si protokol HTTP :)\n");
            break;
            case 2:
                this->protocol = {Protocol::HTTPS};
                printf("Vybrali ste si protokol HTTPS :)\n");
                break;
            case 3:
                this->protocol = {Protocol::FTP};
                printf("Vybrali ste si protokol FTP :)\n");
                break;
            case 4:
                this->protocol = {Protocol::FTPS};
                printf("Vybrali ste si protokol FTPS :)\n");
                break;
            default:
                printf("Invalidná hodnota, zadaj znova!\n");
                vyberProtocol();
        }
    }

    Protocol getProtocol() const
    {
        return this->protocol;
    };
};


#endif //DOWNLOADMANAGER_MANAGER_H
