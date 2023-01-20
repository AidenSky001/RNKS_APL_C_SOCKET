#include <stdio.h>
#include <string.h>
#include <winsock2.h>

int main()
{
    //Anlegen der Variablen
    int Nachrichtencounter =0;
    int Vergleichswert =1;
    int Sequenzbild =1;

    // Initialisieren von Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Fehler bei WSAStartup\n");
        return 1;
    }
    printf("WSA eingerichtet \n");

    // Erstellen des sockets
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        printf("Fehler beim Erstellen des sockets\n");
        WSACleanup();
        return 1;
    }
    printf("Socket erstellt \n");

    // Binden des sockets an eine Adresse und einen Port
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(5000);
    if (bind(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Fehler beim Binden des sockets\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Socket wurde gebunden \n");

    // Empfangen von Nachrichten
    char buffer[1024];
    SOCKADDR_IN clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    while (1) {
        int recvLen = recvfrom(sock, buffer, sizeof(buffer), 0, (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            printf("Empfangen von IP %s: Port %d: %s \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);
            Nachrichtencounter++; //Sorgt dafür das die Nachrichten Nacheinander eine Nummer bekommen, welche mit der Sequenznummer abgelichen werden können
            if (Nachrichtencounter %2 == 0)
            {
                sscanf(buffer, "%i", &Sequenzbild);
                printf("Die Sezequenz die ankam %d \n", Sequenzbild);
                printf("Der Counter liegt bei %d \n", Vergleichswert);
                if(Vergleichswert == Sequenzbild){
                    printf("Die Sequenzen sind gleich. \n");

                    //Die Antwort des Servers an den Client
                    char sendeAntwort[] = "Die Sequenznummer stimmt.";
                    int sendLen = strlen(sendeAntwort) +1;
                    sendto(sock, sendeAntwort, sendLen, 0, (SOCKADDR*)&clientAddr, clientAddrLen);
                    printf("Antwort an Client gesendet");
                }
                else
                    {
                    char sendeAntwort[] = "Die Sequenznummer stimmt nicht.";
                    int sendLen = strlen(sendeAntwort) +1;
                    sendto(sock, sendeAntwort, sendLen, 0, (SOCKADDR*)&clientAddr, clientAddrLen);
                    printf("Antwort an Client gesendet");
                    }
                Vergleichswert++;
            }
        }
    }

    // Schließen des sockets und Aufräumen von Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
