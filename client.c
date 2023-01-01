#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>
#include <stdlib.h>

int main()
{
    //Hilfsvariable
    int antwort;

    //Init von ws2_32.lib
    WSADATA wsaData;

    // Initialisiere Winsock

    antwort = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Fehler bei WSAStartup\n");
    }

    // Socket Deskriptor
    SOCKET slisten, client; //Hier wird der Socket slisten eingerichtet

    // Socket einrichten
    slisten = socket(AF_INET, SOCK_STREAM, 0);
    if (slisten != INVALID_SOCKET)
    {
        printf("Socket erfolgreich gestartet");
    }
    else
    {
        printf("Socket einrichten fehlgeschlagen");
    }

    //Verbindungsinformationen
    struct sockaddr_in info; // In C muss hier dieses Struct vorangesetzt werden, sonst geht das definieren nicht (keine Ahnung warum, in C++ ist das anders)
    info.sin_addr.s_addr    = inet_addr("localhost"); //Die erreichbare Internet Adresse, hier könnte auch eine IP rein, ich will mich hier aber erstmal selbst anwählen
    info.sin_family         = AF_INET; //Die Family ist wie oben die AF_INET
    info.sin_port           = htons(54345); //Das ist irgendein Port mit einer festgelegten Größe
    int infolen             = sizeof(info); //Gibt die Größe von der Info zurück

    antwort = bind(slisten, (struct sockaddr*)&info, infolen); //stuct sockaddr wird hier gecastet

    if(antwort != SOCKET_ERROR)
    {
        printf("Der Bind war erfolgreich!");
    }
    else
    {
        printf("Bind fehlgeschlagen");
    }

    //Warteschlange für einkommende Verbindungen
    antwort = listen(slisten, SOMAXCONN); //Wie viele Verbindungen dürfen zugelassen werden, in diesem Beispiel sind es Max. also so viele wie gehen
    if(antwort != SOCKET_ERROR)
    {
        printf("listen() war erfolgreich!");
    }
    else
    {
        printf("listen() fehlgeschlagen");
    }

    //Hier kommen die Client-Informationen
    struct sockaddr_in clientinfo;
    int clientinfolen = sizeof(clientinfo);

    while(1) //Die Hauptschleife
    {
        client = accept(slisten, (struct sockaddr*)&clientinfo, &clientinfolen);
        if (client != SOCKET_ERROR)
        printf("client Akzeptiert");
        printf(inet_ntoa(clientinfo.sin_addr));
        printf(" : ");
        //printf(ntohs(clientinfo.sin_port));
    }

    closesocket(client);
    closesocket(slisten);
    WSACleanup();

}