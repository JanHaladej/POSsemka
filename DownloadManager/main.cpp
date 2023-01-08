#include "VlaknoObj.h"

std::string statusConvert(int cislo){
    switch(cislo) {
        case 0:
            return "nestahuje sa";
            break;
        case 1:
            return "stahuje sa";
            break;
        case 2:
            return "pauznute";
            break;
        case 3:
            return "cancelnute";
            break;
        case 4:
            return "dostahovane";
            break;
        default:
            return "Error default switch statusConvert";
    }
}

std::string* splitstr(std::string str, std::string deli = " ")
{
    std::string* polePrikazov = new std::string[7]; // ci treba dat delete ked je to vo funkcii lebo unmap mi pise ze to nevie najst ked dam delete
    int count = 0;
    int start = 0;
    int end = str.find(deli);
    while (end != -1 && polePrikazov[5].empty() == 1) {
        polePrikazov[count] = str.substr(start, end - start);
        count++;
        start = end + deli.size();
        end = str.find(deli, start);
    }

    polePrikazov[count] = str.substr(start, end - start);

    return polePrikazov;
}

void status(std::vector<VlaknoObj*> vectorObjektov){
    std::cout<< "----------------------------------------------\n";
    for (int i = 0; i < vectorObjektov.size(); ++i) {
        std::cout << "ID: " << vectorObjektov.at(i)->getID() << "\t \t " + vectorObjektov.at(i)->getMenoSuboru() << "\t \t ma stiahnutych bytov: " << vectorObjektov.at(i)->getDoposialStiahnute() << "\t \t z celkovych: " << vectorObjektov.at(i)->getCelkovuVelkostSuboru() << "\t \t status: " << statusConvert(vectorObjektov.at(i)->getState()) << "\n";
    }
    std::cout<< "----------------------------------------------\n";
}

typedef struct checkerDataPass {
    bool* checkerVar;
    std::vector<VlaknoObj*>* vectorObjektov;
    pthread_mutex_t* mutex;
}CDP;

void * checkerF(void * arg) {
    //std::vector<VlaknoObj *> *vectorObjektov = static_cast<std::vector<VlaknoObj *> *>(arg);
    CDP* struktura = static_cast<CDP*>(arg);
    while(*struktura->checkerVar) {
        sleep(1);
        pthread_mutex_lock(struktura->mutex);

        //zastav vsetky
        for (int i = 0; i < struktura->vectorObjektov->size(); ++i) {
            if (struktura->vectorObjektov->at(i)->getState() == 1) {
                struktura->vectorObjektov->at(i)->setState(0);
            }
        }

        int maxID = 0;
        int maxPrio = INT_MAX;

        for (int j = 0; j < 3; ++j) {//3 cisla
            for (int i = 0; i < struktura->vectorObjektov->size(); ++i) {// prejdi celu strukturu
                if (struktura->vectorObjektov->at(i)->getState() == 0 &&
                    struktura->vectorObjektov->at(i)->getPriorita() < maxPrio
                    && struktura->vectorObjektov->at(i)->presielCas()) {
                    maxPrio = struktura->vectorObjektov->at(i)->getPriorita();
                    maxID = i + 1;
                }
            }
            if (maxID != 0) {
                struktura->vectorObjektov->at(maxID - 1)->setState(1);
                struktura->vectorObjektov->at(maxID - 1)->vytvorVlakno();
                maxID = 0;
                maxPrio = INT_MAX;
            }
        }

        pthread_mutex_unlock(struktura->mutex);
    }
    pthread_exit(NULL);
}

bool zistiCiSaStahuje(std::vector<VlaknoObj*>* vectorObjektov, pthread_mutex_t* mutex){
    pthread_mutex_lock(mutex);
    for (int i = 0; i < vectorObjektov->size(); ++i) {
        if (vectorObjektov->at(i)->getState() <= 2 ){// 0 1 2 ready,stahuje sa, pauza
            pthread_mutex_unlock(mutex);
            return true;
        }
    }
    pthread_mutex_unlock(mutex);
    return false;
}

void zrusVsetkyStahujuceSa(std::vector<VlaknoObj*>* vectorObjektov, pthread_mutex_t* mutex){
    pthread_mutex_lock(mutex);
    for (int i = 0; i < vectorObjektov->size(); ++i) {
        if (vectorObjektov->at(i)->getState() <= 2 ){// 0 1 2 ready,stahuje sa, pauza
            vectorObjektov->at(i)->setState(3);
        }
    }
    pthread_mutex_unlock(mutex);
}

int main(int argc, char* argv[])
{

    std::string userInput;
    std::string* strPtr;
    int counter=0;

    std::vector<VlaknoObj*> vectorObjektov;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    bool checkerVar = true;// na ukoncenie vlakno co ide donekonecna
    CDP checkerData = {&checkerVar, &vectorObjektov, &mutex};
    pthread_t checker;
    pthread_create(&checker, NULL, checkerF, &checkerData);

    while(userInput != "exit") {
        std::cout << "Ocakavam prikaz" << std::endl;
        getline(std::cin, userInput);// download http pukalik.sk /pos/dog.jpeg priorita cas menoSuboru // download http pukalik.sk /pos/dog.jpeg 12 20:16 dog.jpeg // download http kornhauserbus.sk /images/background.png 12 17:30 background.png
        strPtr = splitstr(userInput);// download https speed.hetzner.de /100MB.bin 12 22:00 100MB.bin
        std::cout << "\n";//state exit

        if (strPtr[0] == "download") {
            counter++;//counter na ID
            pthread_mutex_lock(&mutex);
            vectorObjektov.push_back(new VlaknoObj(counter, strPtr[1], strPtr[2], strPtr[3], stoi(strPtr[4]), strPtr[5], strPtr[6]));
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "state") {
            pthread_mutex_lock(&mutex);
            status(vectorObjektov);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "resume") {// resume ID //resume 1//nastavujem na 0 lebo to znamena ze sa momentalne nestahuje ale akonahle pride dostatocna priorita tak sa zacne stahovat
            pthread_mutex_lock(&mutex);
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(0);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "pause") {// pause ID //pause 1// nastavim na 2 lebo 0 by bola ze checker to moze zacat stahovat a 1 by bolo ze sa to stahuje
            pthread_mutex_lock(&mutex);
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(2);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "cancel") {// cancel ID //cancel 1// 3 akoze nech uz sa s tym nic nerobi a poznaci sa ze to bolo cancellnute
            pthread_mutex_lock(&mutex);
            vectorObjektov.at(stoi(strPtr[1]) - 1)->setState(3);
            pthread_mutex_unlock(&mutex);
        } else if (strPtr[0] == "system") {// cancel ID //cancel 1// 3 akoze nech uz sa s tym nic nerobi a poznaci sa ze to bolo cancellnute
            std::cout << system((strPtr[1] + " " + strPtr[2] + " " + strPtr[3] + " " + strPtr[4] + " " + strPtr[5] + " " + strPtr[6]).c_str()) << std::endl;
        } else if (strPtr[0] == "exit") {
            if (zistiCiSaStahuje(&vectorObjektov, &mutex)){
                std::cout << "Stale existuju subory, ktore sa stahuju. Chcete ukoncit program? [y/n]" << std::endl;
                getline(std::cin, userInput);
                std::cout << "\n";
                if (userInput == "y"){
                    zrusVsetkyStahujuceSa(&vectorObjektov, &mutex);
                    userInput = "exit";
                }

            }
        }

        delete[] strPtr;//lebo opakovane davam novy string a nemozem stratit ten predtym inak sa ku nemu uz nedostanem preto sem free // zaujmava poznamka -> ked davam new tak takto delete a nie delete(foo);
    }

    checkerVar = false;

    pthread_join(checker, NULL);

    for (int i = 0; i < vectorObjektov.size(); ++i) {
        pthread_join( vectorObjektov[i]->getVlakno(), NULL);
    }

    std::ofstream myfile;
    myfile.open ("/home/haladej/history.txt", std::ofstream::app);
    for (int i = 0; i < vectorObjektov.size(); ++i) {

        myfile << vectorObjektov.at(i)->objString();

    }
    myfile << "--------------------------------------\n";
    myfile.close();

    for (int i = 0; i < vectorObjektov.size(); ++i) {
        delete(vectorObjektov.at(i));
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}