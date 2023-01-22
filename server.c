#include <stdio.h>
#include <string.h>
#include <winsock2.h>

int main(int argc, char** argv)
{
    if (argc != 3){
        printf("Zu wenige Argumente eingegeben: \n Datei Ausgabename: %s <filename> Port: <port>\n", argv[0]);
        return 1;
    }

    //Anlegen von Variablen
    char* filename = argv[1];
    int port = atoi(argv[2]); //Portnummer kann nur 16 Bit groß sein. Also maximal 65535 Ports // Alle Ports bis <1024 sind reserviert
    int Nachrichtencounter =0;
    int Vergleichswert =1;
    int Sequenzbild =1;

    if (port < 1024)
    {
        printf("Bitte einen Port groeßer 1024 waehlen. Ports unter 1025 sind reserviert");
        return 1;
    }
    if (port > 65535)
    {
        printf("Bitte eine kleinere Porstnummer verwenden. Nur Portnummern bis 65535 sind zulaessig.");
        return 1;
    }

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
    addr.sin_port = htons(port);
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
    //Schreiben der Nachricht in eine serperate Datei
    //"a" ist der Append Modus wo immer weitergeschrieben wird statt zu überschreiben
    FILE *file = fopen(filename, "a"); 

    while (1) {
        int recvLen = recvfrom(sock, buffer, sizeof(buffer), 0, (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            printf("Empfangen von IP %s: Port %d: %s \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);
            Nachrichtencounter++; //Sorgt dafür das die Nachrichten Nacheinander eine Nummer bekommen, welche mit der Sequenznummer abgelichen werden können

            fwrite(buffer, sizeof(char), recvLen, file); //Schreiben der Daten in eine Datei

            if (Nachrichtencounter %2 == 0)
            {
                sscanf(buffer, "%i", &Sequenzbild);
                printf("Die Sezequenz die ankam %d \n", Sequenzbild);
                printf("Der Counter liegt bei %d \n", Vergleichswert);
                if(Vergleichswert == Sequenzbild){
                    printf("Die Sequenzen sind gleich. \n");

                    //Für Testzwecke kann folgende Methode verwendet werden:
                    //Sleep(6000); 

                    //Die Antwort des Servers an den Client
                    char sendeAntwort[] = "Die Sequenznummer stimmt. \n";
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

    // Schließen des Filewriters, Schließen des Sockets und Aufräumen von Winsock
    fclose(file);
    closesocket(sock);
    WSACleanup();

    return 0;
}
