//
// Created by matej on 1/1/2023.
//
#include <boost/lexical_cast.hpp>

/*FTP server*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include<sys/stat.h>            /*pre zistenie velkosti suboru pouzijem stat()*/
#include<sys/sendfile.h>        /*pre poslanie suboru*/
#include<fcntl.h>               /*pre O_RDONLY*/

void FTP_Server::vykonajPrikaz_ls(int &i, int &soketa2, struct stat &objekt, int &velkostObjektu, int &deskriptorSuboru)
{
    system("ls >textovySubor.txt");
    i = 0;

    // získa stavové informácie o špecifikovanom súbore a umiestni ich do oblasti pamäte, na ktorú poukazuje argument buf.
    // Ak je pomenovaný súbor symbolickým odkazom, funkcia stat() vyrieši symbolický odkaz.
    // Tiež vráti informácie o výslednom súbore.
    stat("textovySubor.txt",&objekt);
    velkostObjektu = objekt.st_size;

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa2, &velkostObjektu, sizeof(int),0);

    // vytvorí a vráti nový deskriptor súboru pre súbor pomenovaný podľa názvu súboru.
    // Na začiatku je indikátor polohy súboru na začiatku súboru.
    deskriptorSuboru = open("textovySubor.txt", O_RDONLY);

    // kopíruje dáta medzi jedným deskriptorom súboru a druhým.
    // Pretože toto kopírovanie prebieha v jadre, sendfile() je efektívnejšie
    // ako kombinácia read() a write(), čo by vyžadovalo prenos dát do až užívateľského priestoru.
    sendfile(soketa2,deskriptorSuboru,nullptr,velkostObjektu);
}

void FTP_Server::vykonajPrikaz_get(int &soketa2, char pamat[], char nazovSuboru[], struct stat &objekt,
                       int &deskriptorSuboru, int &velkostObjektu)
{
    // Číta naformátované údajé.
    // Funguje to podobne ako scanf(), ale údaje sa čítajú z reťazca namiesto z konzoly.
    sscanf(pamat, "%s%s", nazovSuboru, nazovSuboru);

    // získa stavové informácie o špecifikovanom súbore a umiestni ich do oblasti pamäte, na ktorú poukazuje argument buf.
    // Ak je pomenovaný súbor symbolickým odkazom, funkcia stat() vyrieši symbolický odkaz.
    // Tiež vráti informácie o výslednom súbore.
    stat(nazovSuboru, &objekt);

    // vytvorí a vráti nový deskriptor súboru pre súbor pomenovaný podľa názvu súboru.
    // Na začiatku je indikátor polohy súboru na začiatku súboru.
    deskriptorSuboru = open(nazovSuboru, O_RDONLY);
    velkostObjektu = objekt.st_size;

    if(deskriptorSuboru == -1)
        velkostObjektu = 0;

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa2, &velkostObjektu, sizeof(int), 0);

    if(velkostObjektu)
        sendfile(soketa2, deskriptorSuboru, nullptr, velkostObjektu);
}

void FTP_Server::vykonajPrikaz_put(int &c, int &i, char pamat[], char prikaz[], char nazovSuboru[],
                       int &soketa2, int &deskriptorSuboru, int &velkostObjektu)
{
    c = 0;
    char* subor;

    // Číta naformátované údajé.
    // Funguje to podobne ako scanf(), ale údaje sa čítajú z reťazca namiesto z konzoly.
    sscanf(pamat+strlen(prikaz), "%s", nazovSuboru);

    // Číta prichádzajúce údaje o soketách na pripojenie alebo bez pripojenia.
    // Pri použití protokolu na pripojenie musia byť sokety pripojené pred volaním recv.
    // Pri použití protokolu bez pripojenia musia byť sokety zviazané pred volaním recv.
    recv(soketa2, &velkostObjektu, sizeof(int), 0);
    i = 1;

    while(true)
    {
        deskriptorSuboru = open(nazovSuboru, O_CREAT | O_EXCL | O_WRONLY, 0666);
        if(deskriptorSuboru == -1)
        {
            // Ak je úspešný, vráti celkový počet zapísaných znakov bez pridaného nulového znaku v reťazci,
            // v prípade zlyhania sa vráti záporné číslo. sprintf znamená „String print“.
            sprintf(nazovSuboru + strlen(nazovSuboru), "%d", i);
        }
        else
            break;
    }
    subor = static_cast<char *>(malloc(velkostObjektu));
    recv(soketa2, subor, velkostObjektu, 0);

    // Sa pokúsi zapísať nbyte bajtov z vyrovnávacej pamäte, na ktorú ukazuje buf,
    // do súboru spojeného s otvoreným deskriptorom súboru.
    // Pred vykonaním akejkoľvek akcie popísanej nižšie a ak je nbyte nula a súbor je bežný súbor,
    // funkcia write() môže zistiť a vrátiť chyby.
    c = write(deskriptorSuboru, subor, velkostObjektu);

    // Uvoľní deskriptor súboru označený fildes.
    // Delokovať znamená sprístupniť deskriptor súboru na vrátenie následnými volaniami funkcie open() alebo iných funkcií,
    // ktoré prideľujú deskriptory súborov.
    close(deskriptorSuboru);

    // odosiela údaje na soket so soket deskriptora.
    // Volanie send() sa vzťahuje na všetky pripojené sokety.
    // Deskriptor sokety. Ukazovateľ na vyrovnávaciu pamäť obsahujúcu správu na prenos.
    send(soketa2, &c, sizeof(int), 0);
}

void FTP_Server::vykonajPrikaz_pwd(int &i, char pamat[], int &soketa2)
{
    system("pwd>textovySubor.txt");
    i = 0;
    FILE* f = fopen("textovySubor.txt","r");
    while(!feof(f))
        pamat[i++] = fgetc(f);

    pamat[i-1] = '\0';
    fclose(f);
    send(soketa2, pamat, 100, 0);
}

void FTP_Server::vykonajPrikaz_cd(char pamat[], int &c, int &soketa2)
{
    if(chdir(pamat+3) == 0)
        c = 1;
    else
        c = 0;
    send(soketa2, &c, sizeof(int), 0);
}

void FTP_Server::vykonajPrikaz_quit_OR_bye(int &i, int &soketa2)
{
    printf("FTP server končí...\n");
    i = 1;
    send(soketa2, &i, sizeof(int), 0);
    exit(0);
}

void FTP_Server::vykonajPrikazCMD(int i, int soketa2, char *pamat, char *prikaz, char *nazovSuboru, struct stat objekt,
                                  int velkostObjektu, int deskriptorSuboru, int c)
{
    while(true)
    {
        // Číta prichádzajúce údaje o soketách na pripojenie alebo bez pripojenia.
        // Pri použití protokolu na pripojenie musia byť sokety pripojené pred volaním recv.
        // Pri použití protokolu bez pripojenia musia byť sokety zviazané pred volaním recv.
        recv(soketa2, pamat, 100, 0);

        // Číta naformátované údajé.
        // Funguje to podobne ako scanf(), ale údaje sa čítajú z reťazca namiesto z konzoly.
        sscanf(pamat, "%s", prikaz);
        if(!strcmp(prikaz, "ls"))
        {
            vykonajPrikaz_ls(i, soketa2, objekt, velkostObjektu, deskriptorSuboru);
        }
        else if(!strcmp(prikaz,"get"))
        {
            vykonajPrikaz_get(soketa2, pamat, nazovSuboru, objekt, deskriptorSuboru, velkostObjektu);
        }
        else if(!strcmp(prikaz, "put"))
        {
            vykonajPrikaz_put(c, i, pamat, prikaz, nazovSuboru, soketa2, deskriptorSuboru, velkostObjektu);
        }
        else if(!strcmp(prikaz, "pwd"))
        {
            vykonajPrikaz_pwd(i, pamat, soketa2);
        }
        else if(!strcmp(prikaz, "cd"))
        {
            vykonajPrikaz_cd(pamat, c, soketa2);
        }
        else if(!strcmp(prikaz, "bye")
                || !strcmp(prikaz, "quit"))
        {
            vykonajPrikaz_quit_OR_bye(i, soketa2);
        }
    }
}

void FTP_Server::Server(char* retazec)
{
    struct sockaddr_in server, klient;           // adresa soketu servera a adresa soketu klienta
    struct stat objekt;
    int soketa1, soketa2;
    char pamat[100];
    char prikaz[5];
    char nazovSuboru[20];
    int vysledokOperacieADDR;    // priradenie adresy lokálneho soketu
    int i = 1;                   // vykonavanie prikazov, v cykle zivotnost, nie pocet vykonania
    int c = 0;
    int velkostObjektu = 0;
    int dlzkaKlienta = 0;
    int deskriptorSuboru = 0;

    // vytvorim novu socketu typu SOCK_STREAM v domene AF_INET pomocu protokolu.
    // Ak je protokol 0 vyberie sa jedna socketa automaticky
    // vrati suborovy deskriptor pre novy socket
    // alebo vrati -1, co znamena chybu
    soketa1 = socket(AF_INET, SOCK_STREAM, 0);

    if(soketa1 == -1)
    {
        printf("Vytvorenie sokety zlyhalo!\n");
        exit(1);
    }

    // cislo portu
    server.sin_port = atoi(retazec);
    // internetova adressa
    server.sin_addr.s_addr = 0;


    // Funkcia bind() priradí adresu adresy lokálneho soketu, soketu identifikovanému soketom deskriptora,
    // ktorý nemá priradenú adresu lokálneho soketu.
    // Sokety vytvorené pomocou funkcie socket() sú spočiatku nepomenované;
    // identifikujú sa iba podľa rodiny adries.
    // Po úspešnom dokončení funkcia bind() vráti 0; inak sa vráti -1 a nastaví sa errno na označenie chyby.
    vysledokOperacieADDR = bind(soketa1,(struct sockaddr*)&server,sizeof(server));

    if(vysledokOperacieADDR == -1)
    {
        printf("\nOperácia bind() bola neúspešná! Nepodarilo sa výtvoriť lokalnú adresú soketu servera.\n");
        exit(1);
    }

    // Funkcia listen() sa vzťahuje len na streamové sokety.
    // Indikuje pripravenost akceptovat poziadavky klienta na pripojenie a vytvara front ziadosti o pripojenie
    // Ak sa front preplni, ziadosti o pripojenie budu odmietnute.
    // Ak je funkcia listen() úspešná, vráti 0 ak nie je vrati -1
    // Nasledne nastaví errno na jednu z nasledujúcich hodnôt: Kód chyby. Popis.
    vysledokOperacieADDR = listen(soketa1,1);
    if(vysledokOperacieADDR == -1)
    {
        printf("Operácia listen() bola neúspešná!\n");
        exit(1);
    }

    dlzkaKlienta = sizeof(klient);

    // Extrahuje prvú požiadavku na pripojenie z frontu čakajúcich pripojení pre počúvajúci soket, sockfd,
    // vytvorí nový pripojený soket
    // a vráti nový deskriptor súboru odkazujúci na tento soket
    soketa2 = accept(soketa1, (struct sockaddr*)&klient, reinterpret_cast<socklen_t *>(&dlzkaKlienta));

    vykonajPrikazCMD(i, soketa2, pamat, prikaz, nazovSuboru, objekt, velkostObjektu, deskriptorSuboru, c);
}


