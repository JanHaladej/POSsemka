#include <iostream>
#include <string>
using namespace std;

string* splitstr(string str, string deli = " ")
{
    string* polePrikazov = new string[6]; // ci treba dat delete ked je to vo funkcii lebo unmap mi pise ze to nevie najst ked dam delete
    int count = 0;
    int start = 0;
    int end = str.find(deli);
    while (end != -1 && polePrikazov[4].empty() == 1) {
        polePrikazov[count] = str.substr(start, end - start);
        count++;
        start = end + deli.size();
        end = str.find(deli, start);
    }

    polePrikazov[count] = str.substr(start, end - start);

    return polePrikazov;
}

int main(int argc, char* argv[])
{
    std::string prikaz;
    std::string stranka;// = "kornhauserbus.sk";
    std::string objekNaStiahnutie;// = "/images/background.png";
    std::string protokol;
    std::string priorita;
    std::string cas;

    string userInput;
    string* strPtr;

    while(userInput != "exit") {
        cout << "Ocakavam prikaz" << endl;
        getline(cin, userInput);
        strPtr= splitstr(userInput);
        cout << "\n";
        for(int i=0; i<6 ; i++)
        {
            cout<<strPtr[i]<<'\n';
        }
    }







    return 0;
}