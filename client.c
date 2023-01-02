#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>

#define BUFFER_SIZE 1024
#define PORT 54321



int main(int argc, char** argv)
{
    if (argc != 4){
        printf("Benutze Datei: %s <filename> <destination IP> <port>\n", argv[0]);
        return 1;
    }

    //Anlegen der Variablen die vom Nutzer als Eingangsargumente eingegeben werden
    char* filename = argv[1];
    char* dest_ip = argv[2];
    int port = atoi(argv[3]);

    //Hilfsvariable

    struct sockaddr_in server; 

    //Init von ws2_32.lib
    WSADATA wsaData;

    // Initialisiere Winsock
    printf("Initalisiere Winsock ... \n");
    WSAStartup(MAKEWORD(2,2), &wsaData);

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("Fehler bei WSAStartup. Fehlercode: %d \n", WSAGetLastError()); //Gibt bei Fehlern auch den Fehlercode aus, lässt sich leichter debbugen
        return 1;
    }

    // Socket Deskriptor
    SOCKET rnkssocket; //Hier wird der Socket rnkssocket eingerichtet

    // Socket einrichten
    printf("Erstelle den Socket ... \n");
    rnkssocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (rnkssocket != INVALID_SOCKET)
    {
        printf("Socket erfolgreich erstellt \n");
    }
    else
    {
        printf("Socket einrichten fehlgeschlagen \n");
    }

    //Verbindungsinformationen
    struct sockaddr_in info; // In C muss hier dieses Struct vorangesetzt werden, sonst geht das definieren nicht (keine Ahnung warum, in C++ ist das anders)
    info.sin_addr.s_addr    = inet_addr(dest_ip); //Die erreichbare Internet Adresse, hier könnte auch eine IP rein, ich will mich hier aber erstmal selbst anwählen
    info.sin_family         = AF_INET; //Die Family ist wie oben die AF_INET
    info.sin_port           = htons(PORT); //Das ist irgendein Port mit einer festgelegten Größe

    //Datei öffen
    char buffer[BUFFER_SIZE];
    printf("Datei wird eingelesen... \n");
    FILE* fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Fehler beim öffnen der Datei! \n");
        return 1;
    }

    //Datei senden 
    printf("Sende Datei ... \n");
    // Empfange Nachricht
struct sockaddr_in sender;
int sender_len = sizeof(sender);
char recv_buffer[BUFFER_SIZE];
int recv_len = recvfrom(rnkssocket, recv_buffer, BUFFER_SIZE, 0, (struct sockaddr*)&sender, &sender_len);
if (recv_len > 0) {
    recv_buffer[recv_len] = '\0';
    printf("Nachricht empfangen: %s\n", recv_buffer);
}

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL){
        if (sendto(rnkssocket, buffer, strlen(buffer), 0, (struct sockaddr*)&server, sizeof(server)) < 0){
            printf("Senden fehlgeschlagen! \n");
        }
    }

    fclose(fp);
    closesocket(rnkssocket);
    WSACleanup();


    printf("Goodbye. Stay healthy and wash your hands. 3301");
    return 0;
}