//
// Created by matej on 1/1/2023.
//
#include <boost/lexical_cast.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>

#include "FTP_Klient.h"


bool FTP_Klient::volba_get(int &i, char nazovSuboru[], char pamat[], int &soketa, int &velkostObjektu,
                           char* &subor, int &deskriptorSuboru)
{
    printf("Vlož nazovSubora do get: ");
    scanf("%s", nazovSuboru);
    strcpy(pamat, "get ");
    strcat(pamat, nazovSuboru);

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa, pamat, 100, 0);

    // Číta prichádzajúce údaje o soketách na pripojenie alebo bez pripojenia.
    // Pri použití protokolu na pripojenie musia byť sokety pripojené pred volaním recv.
    // Pri použití protokolu bez pripojenia musia byť sokety zviazané pred volaním recv.
    recv(soketa, &velkostObjektu, sizeof(int), 0);
    if(!velkostObjektu)
    {
        printf("Neexistuje taký súbor v directory!\n\n");
        return true;
    }
    subor = static_cast<char *>(malloc(velkostObjektu));
    recv(soketa, subor, velkostObjektu, 0);
    while(true)
    {
        // vytvorí a vráti nový deskriptor súboru pre súbor pomenovaný podľa názvu súboru.
        // Na začiatku je indikátor polohy súboru na začiatku súboru.
        deskriptorSuboru = open(nazovSuboru, O_CREAT | O_EXCL | O_WRONLY, 0666);
        if(deskriptorSuboru == -1)
        {
            // Ak je úspešný, vráti celkový počet zapísaných znakov bez pridaného nulového znaku v reťazci,
            // v prípade zlyhania sa vráti záporné číslo. sprintf znamená „String print“.
            sprintf(nazovSuboru + strlen(nazovSuboru), "%d", i); // je potrebne ak to iste direcotory je pouzite pre oba servre a klienta
        }
        else break;
    }
    // zapise data do suboru
    fwrite(&deskriptorSuboru, *subor, velkostObjektu, nullptr);
    // zatvori subor
    close(deskriptorSuboru);
    strcpy(pamat, "cat ");
    strcat(pamat, nazovSuboru);
    system(pamat);
    return true;
}

bool FTP_Klient::volba_put(char nazovSuboru[], int &deskriptorSuboru, char pamat[], int &soketa, struct stat &objekt,
        int &velkostObjektu, int &status)
{
    printf("Vloz nazovSubora do put do server: ");
    scanf("%s", nazovSuboru);

    // vytvorí a vráti nový deskriptor súboru pre súbor pomenovaný podľa názvu súboru.
    // Na začiatku je indikátor polohy súboru na začiatku súboru.
    deskriptorSuboru = open(nazovSuboru, O_RDONLY);
    if(deskriptorSuboru == -1)
    {
        printf("Neexistuje taký súbor v directory!\n\n");
        return true;
    }
    strcpy(pamat, "put ");
    strcat(pamat, nazovSuboru);

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa, pamat, 100, 0);

    // Získa stavové informácie o špecifikovanom súbore a umiestni ich do oblasti pamäte,
    // na ktorú poukazuje argument buf.
    // Ak je pomenovaný súbor symbolickým odkazom, funkcia stat() vyrieši symbolický odkaz.
    // Tiež vráti informácie o výslednom súbore.
    stat(nazovSuboru, &objekt);

    velkostObjektu = objekt.st_size;
    send(soketa, &velkostObjektu, sizeof(int), 0);
    sendfile(soketa, deskriptorSuboru, nullptr, velkostObjektu);

    // Číta prichádzajúce údaje o soketách na pripojenie alebo bez pripojenia.
    // Pri použití protokolu na pripojenie musia byť sokety pripojené pred volaním recv.
    // Pri použití protokolu bez pripojenia musia byť sokety zviazané pred volaním recv.
    recv(soketa, &status, sizeof(int), 0);
    if(status)
        printf("Subor bol úspešne uložený.\n");
    else
        printf("Subor bol neúspešne uložený.\n");
    return true;
}

bool FTP_Klient::volba_pwd(char pamat[], int &soketa)
{
    strcpy(pamat, "pwd");

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa, pamat, 100, 0);

    // Číta prichádzajúce údaje o soketách na pripojenie alebo bez pripojenia.
    // Pri použití protokolu na pripojenie musia byť sokety pripojené pred volaním recv.
    // Pri použití protokolu bez pripojenia musia byť sokety zviazané pred volaním recv.
    recv(soketa, pamat, 100, 0);

    printf("Cesta directory je: %s\n", pamat);
    return true;
}

bool FTP_Klient::volba_ls(char pamat[], int &soketa, int &velkostObjektu, char* &subor, int &deskriptorSuboru)
{
    strcpy(pamat, "ls");

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa, pamat, 100, 0);

    // Číta prichádzajúce údaje o soketách na pripojenie alebo bez pripojenia.
    // Pri použití protokolu na pripojenie musia byť sokety pripojené pred volaním recv.
    // Pri použití protokolu bez pripojenia musia byť sokety zviazané pred volaním recv.
    recv(soketa, &velkostObjektu, sizeof(int), 0);

    subor = static_cast<char *>(malloc(velkostObjektu));
    recv(soketa, subor, velkostObjektu, 0);
    deskriptorSuboru = creat("textovySubor.txt", O_WRONLY);
    fwrite(&deskriptorSuboru, *subor, velkostObjektu, nullptr);
    close(deskriptorSuboru);
    printf("Musíte napísať:\n");
    system("cat textovySubor.txt");
    return true;
}

bool FTP_Klient::volba_cd(char pamat[], int &soketa, int &status)
{
    strcpy(pamat, "cd ");
    printf("Vlož cestu aby ste zmenili directory: ");
    scanf("%s", pamat + 3);

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa, pamat, 100, 0);

    // Číta prichádzajúce údaje o soketách na pripojenie alebo bez pripojenia.
    // Pri použití protokolu na pripojenie musia byť sokety pripojené pred volaním recv.
    // Pri použití protokolu bez pripojenia musia byť sokety zviazané pred volaním recv.
    recv(soketa, &status, sizeof(int), 0);

    if(status)
        printf("Remote directory bolo uspešné zmenené changed.\n");
    else
        printf("Remote directory nebolo uspešné zmenené changed.\n");
    return true;
}

bool FTP_Klient::volba_quit(char pamat[], int &soketa, int &status)
{
    strcpy(pamat, "quit");
    send(soketa, pamat, 100, 0);
    recv(soketa, &status, 100, 0);
    if(status)
    {
        printf("Server sa zatvára.\nQuitting...\n");
        exit(0);
    }
    printf("Zhyhalo zatvorenie spojenia.\n");
    return true;
}

void FTP_Klient::vybrataVolba(int &i, int &vybranyPrikaz, char nazovSuboru[], char pamat[], int &soketa, int &velkostObjektu,
                              char* subor, int &deskriptorSuboru, struct stat &objekt, int &status)
{
    while(true)
    {
        printf("Výber príkaz:\n1- get\n2- put\n3- pwd\n4- ls\n5- cd\n6- quit\n");
        scanf("%d", &vybranyPrikaz);
        switch(vybranyPrikaz)
        {
            case 1:
                if (volba_get(i, nazovSuboru, pamat, soketa, velkostObjektu,
                    subor, deskriptorSuboru))
                    break;
            case 2:
                if (volba_put(nazovSuboru, deskriptorSuboru, pamat, soketa, objekt, velkostObjektu, status))
                    break;
            case 3:
                if (volba_pwd(pamat, soketa))
                    break;
            case 4:
                if (volba_ls(pamat, soketa, velkostObjektu, subor, deskriptorSuboru))
                    break;
            case 5:
                if (volba_cd(pamat, soketa, status))
                    break;
            case 6:
                if (volba_quit(pamat, soketa, status))
                    break;
            default:
                printf("CHYBA!!! Je iné číslo ako 1, 2, 3, 4, 5, 6");
        }
    }
}

void FTP_Klient::Klient(char* retazec)
{
    struct sockaddr_in server;                     // adresa soketu servera a adresa soketu klienta
    struct stat objekt;
    int soketa;
    int vybranyPrikaz;
    char pamat[100];
    char nazovSuboru[20];
    char* subor = new char();
    int vysledokOperacie, velkostObjektu, status;
    int deskriptorSuboru;

    // vytvorim novu socketu typu SOCK_STREAM v domene AF_INET pomocu protokolu.
    // Ak je protokol 0 vyberie sa jedna socketa automaticky
    // vrati suborovy deskriptor pre novy socket
    // alebo vrati -1, co znamena chybu
    soketa = socket(AF_INET, SOCK_STREAM, 0);
    if(soketa == -1)
    {
        printf("Vytvorenie sokety zlyhalo!\n");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = atoi(retazec);
    server.sin_addr.s_addr = 0;

    // Vytvára spojenie soketu medzi klientom a serverom.
    // Táto funkcia spája soket, ktorého deskriptor je sockfd,
    // so serverom a ktorého adresa je uložená v štruktúre serv_addr, ktorá má dĺžku addrlen.
    vysledokOperacie = connect(soketa,(struct sockaddr*)&server, sizeof(server));
    if(vysledokOperacie == -1)
    {
        printf("Pripojenie zlyhalo!");
        exit(1);
    }

    int i = 1;
    vybrataVolba(i, vybranyPrikaz, nazovSuboru, pamat, soketa, velkostObjektu, subor,
                 deskriptorSuboru, objekt, status);
    delete subor;
}













