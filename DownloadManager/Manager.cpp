//
// Created by matej on 12/30/2022.
//
#include <iostream>
#include "Manager.h"

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
}