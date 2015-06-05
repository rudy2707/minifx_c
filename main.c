//
// Created by Rudolf HÖHN on 22.05.15.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT_NUM 2400
#define MAX_BUFFER 1024

int main (int argc, char** argv) {
    char* msg = "Hello baby";

    struct sockaddr_in dest;    // socket info about the machine connecting to us
    struct sockaddr_in server;  // socket info about our server
    int mySocket;               // socket used to listen for incoming connections
    socklen_t socketSize = sizeof(struct sockaddr_in);

    memset(&server, 0, sizeof(server));         // zero the struct before filling the fields

    server.sin_family = AF_INET;                // set the type of connection to TCP/IP
    inet_aton(INADDR_ANY, &server.sin_addr.s_addr); // set our address to any interface
    server.sin_port = htons(PORT_NUM);          // set the server port number

    mySocket = socket(AF_INET, SOCK_STREAM, 0);

    // bind server information to mySocket
    bind(mySocket, (struct sockaddr *)&server, sizeof(struct sockaddr));

    // start listening, allowing a queue of up to 1 pending connection
    listen(mySocket, 1);
    int conSocket = accept(mySocket, (struct sockaddr*)&dest, &socketSize);

    while (conSocket)
    {
        printf("Icoming connection from %s - sending welcom\n", inet_ntoa(dest.sin_addr));
        send(conSocket, msg, strlen(msg), 0);
        close(conSocket);
        conSocket = accept(mySocket, (struct sockaddr *)&dest, &socketSize);
    }

    close(mySocket);

    return EXIT_SUCCESS;
}