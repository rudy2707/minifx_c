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

#define PORT_NUM 2401
#define MAX_BUFFER 1024
#define MAX_CONNECTION 1

/*
* struct msg est déclarée pour être compilée en
*  comme non alignée en mémoire : pas de padding pour
*  optimisé l'accès mémoire de la structure
*  struct msg_aligned est implicitement déclarée avec
*  du padding : le compilateur aligne les données sur
*  la taille des registres du processeur cible de la
*  compilation : un champs short, bien que ayant une
*  taille de 16 bits, prendra en réalité 32 bits en
*  mémoire.
*
*  Exécution :
*
*  msg :
    Le magic du message est 0x73747576
    Le type du message est 0x44556677
    Le handle du message est 0x12345678
    L'offset du message est 0x10121012
    La longueur du message est 0x400

*  Explication : on accède à la même zone mémoire, pour
   struct msg_aligned, le champs offset bien que déclaré
   en short (16 bits), est affiché sur 32 bits (opérateur %x),
   avec le padding (0x0000) de 16 bits en bonus...
*/

struct __attribute__ ((__packed__)) request_t {
    uint32_t magic;
    uint32_t type;
    uint32_t handle;
    uint32_t offset;
    int length;
    char payload[MAX_BUFFER];
};

struct __attribute__ ((__packed__)) response_t {
    uint32_t magic;
    int8_t error;
    uint32_t handle;
    char payload[MAX_BUFFER];
};

int mySocket;               // socket used to listen for incoming connections
int conSocket;

void initServer();

int readBuffer(char* buffer, struct request_t* request);
int readMinix(struct request_t* request, char* bufferRead, char* arg);
int WriteMinix(struct request_t* request, char * arg);
int requestResult(struct request_t* request, struct response_t* response, char* bufferRead);

/*
struct msg_aligned {
    int magic;
    int type;
    int handle;
    short offset;
    int length;
};
*/
int main (int argc, char** argv) {
    struct request_t* requete;
    struct response_t* reponse;

    char bufferRead[MAX_BUFFER];
    char buffer[MAX_BUFFER];

    int nRead;
    int errorReturn;

    initServer();

    while (1)
    {
        if ((nRead = recv(conSocket, buffer, MAX_BUFFER, 0)) < 0) {
            perror("Error to read");
        }

        readBuffer(buffer, requete);

        switch (requete->type) {
            case 0x0 :
                if ((errorReturn = readMinix(requete, bufferRead, argv[1])) != 0)
                    reponse->error = -errorReturn;
                reponse->error = 0;
                requestResult(requete, reponse, bufferRead);
                break;
            case 0x1 :
                if ((errorReturn = WriteMinix(requete, argv[1])) != 0)
                    reponse->error = -errorReturn;
                reponse->error = 0;
                requestResult(requete, reponse, NULL);
                break;
                break;
            default:
                perror("Unknow mode. Read or Write nor found");
                break;
        }

        if ((write(conSocket, reponse, sizeof(struct response_t))) < 0) {
            perror("Error to write into the socket");
            exit(1);
        }
        close(conSocket);
    }

    close(mySocket);

    return EXIT_SUCCESS;
}

void initServer() {
    struct sockaddr_in client;  // socket info about the machine connecting to us
    struct sockaddr_in server;  // socket info about our server

    socklen_t socketSize;

    memset(&server, 0, sizeof(struct sockaddr_in));         // zero the struct before filling the fields
    server.sin_family = AF_INET;                // set the type of connection to TCP/IP IPv4
    server.sin_addr.s_addr = htonl(INADDR_ANY); // set our address to any interface
    server.sin_port = htons(PORT_NUM);          // set the server port number

    if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("Error socket");
        exit(1);
    }
    // bind server information to mySocket
    if (bind(mySocket, (struct sockaddr *)&server, sizeof(server)) < 0)
        perror("Server : error to bind client address");

    // start listening, allowing a queue of up to 1 pending connection
    if (listen(mySocket, MAX_CONNECTION) < 0)
        perror("Error listen");

    memset(&client, 0, sizeof(struct sockaddr_in));
    if ((conSocket = accept(mySocket, (struct sockaddr*)&client, &socketSize)) < 0) {
        perror("Error to accept connexion");
        exit(1);
    }

}

int readBuffer(char* buffer, struct request_t* request) {
    return EXIT_SUCCESS;
}

int readMinix(struct request_t* request, char* bufferRead, char* arg) {
    return EXIT_SUCCESS;
}

int WriteMinix(struct request_t* request, char * arg){
    return EXIT_SUCCESS;
}

int requestResult(struct request_t* request, struct response_t* response, char* bufferRead) {
    return EXIT_SUCCESS;
}

