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
#include <sys/fcntl.h>

#define MAGIC_REQUEST 0x76767676
#define MAGIC_RESPONSE 0x87878787

#define CMD_READ 0x0
#define CMD_WRITE 0x1

#define PORT_NUM 2404
#define MAX_BUFFER 1024
#define DATA_MAX_REQ 1036
#define DATA_MAX_REP 16
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
    uint32_t length;
    char payload[MAX_BUFFER];
};

struct __attribute__ ((__packed__)) response_t {
    uint32_t magic;
    int8_t error;
    uint32_t handle;
    char payload[MAX_BUFFER];
};

struct request_t* request;
struct response_t* response;
char bufferRead[MAX_BUFFER];
char buffer[MAX_BUFFER];

int mySocket;               // socket used to listen for incoming connections
int conSocket;

void initServer();

int readBuffer(char* buffer);
int readMinix(char* arg);
int WriteMinix(char * arg);
int requestResult(char* bufferRead);

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
    int nRead;
    int errorReturn;

    initServer();

    while (1)
    {
        if ((nRead = recv(conSocket, buffer, MAX_BUFFER, 0)) < 0) {
            perror("Error to read");
        }

        //readBuffer(buffer, request);
        printf("message recu %s", buffer);

        switch (request->type) {
            case CMD_READ :
                if ((errorReturn = readMinix(argv[1])) != 0)
                    response->error = -errorReturn;
                response->error = 0;
                //readMinix(argv[1]);
                requestResult(bufferRead);
                break;
            case CMD_WRITE :
                if ((errorReturn = WriteMinix(argv[1])) != 0)
                    response->error = -errorReturn;
                response->error = 0;
                requestResult(NULL);
                break;
            default:
                perror("Unknow mode. Read or Write nor found");
                break;
        }

        if ((write(conSocket, response, sizeof(struct response_t))) < 0) {
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

    request = malloc(sizeof(struct reponse_t));
    response = malloc(sizeof(struct response_t));

}

int readBuffer(char* buffer) {
    //request = (struct request_t *)buffer;

    return EXIT_SUCCESS;
}

int readMinix(char* arg) {
    //request = (struct request_t *)buffer;

    int i;
    //on aloue de la memoire pour la reponse on sait de combien on va lire
    //reponse = malloc(sizeof(struct reponse_t));
    //aloue la memoire pour la tableau structure
    //reponse->payload = malloc( sizeof(char)*length);

    //ouverture du fichier
    int fichier = open(arg, O_RDONLY);

    //on se positionne a la position de l'offset avec le curseur
    lseek(fichier,request->offset,SEEK_SET);
    // On lis le nombre de byte voulu du fichier
    read(fichier,response->payload,request->length);

    //Initialisation des champs
    response->magic = MAGIC_RESPONSE;
    response->error=0;
    response->handle = request->handle;

    return EXIT_SUCCESS;
}

int WriteMinix(char * arg){
    //on aloue de la memoire pour la reponse on sait de combien on va lire
    //reponse = malloc(sizeof(struct reponse_t));

    //ouverture du fichier
    int fichier = open(arg, O_WRONLY);
    //on se positionne a la position de l'offset avec le curseur
    lseek(fichier,request->offset,SEEK_SET);
    // On écris le nombre de byte voulu dans le fichier
    write(fichier,request->payload,request->length);
    //initialisation des champs
    response->magic = MAGIC_RESPONSE;
    response->error=0;
    response->handle = request->handle;

    return EXIT_SUCCESS;
}

int requestResult(char* bufferRead) {
    return EXIT_SUCCESS;
}

