//
// Created by matej on 12/30/2022.
//

#include <string>

#ifndef POSSEMKA_FTP_CODE_H
#define POSSEMKA_FTP_CODE_H

// Pre FTP
#include <string>
namespace FTP_Code
{
    const std::string READY_FOR_LOGIN = "220"; //ready for login
    const std::string USER_SUCCESSFULL = "331"; //user is successfull, input password
    const std::string LOGGED_IN = "230"; //user logged in
    const std::string CLOSING_CMD = "421"; //fatal error where the command socket must be closed
    const std::string PWD = "257"; //print working directory successful
    const std::string CWD = "250"; //change directory successful
}

#endif //POSSEMKA_FTP_CODE_H
