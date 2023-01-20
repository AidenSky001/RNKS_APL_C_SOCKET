#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define BUFFER_SIZE 1024

int main(int argc, char** argv)
{
    if (argc != 4){
        printf("Zu wenige Argumente eingegeben: \n Benutze Datei: %s <filename> <destination IP> <port>\n", argv[0]);
        return 1;
    }

    //Anlegen von Variablen
    char* filename = argv[1];
    char* dest_ip = argv[2];
    int port = atoi(argv[3]);

    // Initialisieren von Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Fehler bei WSAStartup\n");
        return 1;
    }
    printf("WSA Initalisiert \n");

    // Erstellen des sockets
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        printf("Fehler beim Erstellen des sockets \n");
        WSACleanup();
        return 1;
    }
    printf("Socket wurde erstellt \n");

    // Adresse des Servers
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(dest_ip);
    serverAddr.sin_port = htons(port);

    // Senden von Nachrichten an den Server
    
    //Hier noch eine While Schleife drum um den Konsole offen zu halten

     //Datei öffen
    char buffer[BUFFER_SIZE];
    //Variablen zum Arbeiten
    int sequenznummer =0;
    char Zeilen[255];

    printf("Datei wird eingelesen... \n");
    FILE* datei = fopen(filename, "r");
    if(datei == NULL){
        printf("Fehler beim öffnen der Datei! \n");
        return 1;
    }
    printf("Datei gefunden und eingelesen \n");

    //Datei senden
    while (fgets(buffer, BUFFER_SIZE, datei) != NULL){
        sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        printf("Zeilennummern: %d: %s", sequenznummer, buffer);
        sequenznummer ++;

        sprintf(Zeilen, "%d", sequenznummer);
        sendto(sock, Zeilen, strlen(Zeilen), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

        //Auf Antwort des Servers wird gewartet
    char recvBuf[1024];
    int recvLen = recv(sock, recvBuf, sizeof(recvBuf), 0);
    if (recvLen > 0)
    {
        //Daten der Antwort werden verarbeitet
        printf("Empfangene Nachricht vom Server: %s \n", recvBuf);
    }
    }
    printf("Datei gesendet \n");

    // Schließen des sockets und Aufräumen von Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
