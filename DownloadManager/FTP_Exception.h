//
// Created by matej on 12/30/2022.
//

#ifndef POSSEMKA_FTP_EXCEPTION_H
#define POSSEMKA_FTP_EXCEPTION_H

#include <iostream>
#include <exception>

class FTP_Exception : public std::exception
{
    private:
        const std::string message;

    public:
        FTP_Exception(const std::string &str) : message(str)
        {
        }

        virtual const char *what() const throw()
        {
            return message.c_str();
        }

        virtual ~FTP_Exception() throw()
        {
        }
};

class unexpected_Exception : FTP_Exception{
public:
    unexpected_Exception(const std::string &message) : FTP_Exception(message){}
};


#endif //POSSEMKA_FTP_EXCEPTION_H
