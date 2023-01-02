//
// Created by matej on 1/1/2023.
//

#ifndef DOWNLOADMANAGER_FTP_SERVER_H
#define DOWNLOADMANAGER_FTP_SERVER_H


class FTP_Server
{
    public:
        FTP_Server() = default;
        ~FTP_Server() = default;

    private:
        static void vykonajPrikaz_ls(int &i, int &soketa2, struct stat &objekt, int &velkostObjektu, int &deskriptorSuboru);
        static void vykonajPrikaz_get(int &soketa2, char pamat[], char nazovSuboru[], struct stat &objekt,
                               int &deskriptorSuboru, int &velkostObjektu);
        static void vykonajPrikaz_put(int &c, int &i, char pamat[], char prikaz[], char nazovSuboru[],
                               int &soketa2, int &deskriptorSuboru, int &velkostObjektu);
        static void vykonajPrikaz_pwd(int &i, char pamat[], int &soketa2);
        static void vykonajPrikaz_cd(char pamat[], int &c, int &soketa2);
        static void vykonajPrikaz_quit_OR_bye(int &i, int &soketa2);
        static void vykonajPrikazCMD(int i, int soketa2, char pamat[], char prikaz[], char nazovSuboru[],
                              struct stat objekt, int velkostObjektu, int deskriptorSuboru, int c);
    public:
        static void Server(char* retazec);
};


#endif //DOWNLOADMANAGER_FTP_SERVER_H
