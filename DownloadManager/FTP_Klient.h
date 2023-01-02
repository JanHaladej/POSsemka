//
// Created by matej on 1/1/2023.
//

#ifndef DOWNLOADMANAGER_FTP_KLIENT_H
#define DOWNLOADMANAGER_FTP_KLIENT_H


class FTP_Klient
{
public:
    FTP_Klient() = default;
    ~FTP_Klient() = default;

private:
    //printf("Vyber prikaz:\n1- get\n2- put\n3- pwd\n4- ls\n5- cd\n6- quit\n");
        static bool volba_get(int &i, char nazovSuboru[], char pamat[], int &soketa, int &velkostObjektu,
                       char* &subor, int &deskriptorSuboru);
        static bool volba_put(char nazovSuboru[], int &deskriptorSuboru, char pamat[], int &soketa, struct stat &objekt,
                       int &velkostObjektu, int &status);
        static bool volba_pwd(char pamat[], int &soketa);
        static bool volba_ls(char pamat[], int &soketa, int &elkostObjektu, char* &subor, int &deskriptorSuboru);
        static bool volba_cd(char pamat[], int &soketa, int &status);
        static bool volba_quit(char pamat[], int &soketa, int &status);
        static void vybrataVolba(int &i, int &vybranyPrikaz, char nazovSuboru[], char pamat[], int &soketa, int &velkostObjektu,
                          char* subor, int &deskriptorSuboru, struct stat &objekt, int &status);

public:
    void Klient(char* retazec);
};


#endif //DOWNLOADMANAGER_FTP_KLIENT_H
