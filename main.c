//
// Created by Rudolf HÖHN on 22.05.15.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

#define MAGIC_REQUEST 0x76767676
#define MAGIC_RESPONSE 0x87878787

#define CMD_READ 0x0
#define CMD_WRITE 0x1

#define PORT_NUM 2411
#define MAX_BUFFER 1024
#define MAX_REQ 1036
#define MAX_RESP 16
#define MAX_CONNECTION 1

//#define FILE_NAME "minixfs_lab1.img"

/*
* struct request_t est déclarée pour être compilée en
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
*  request_t :
 *  Le magic du message est 0x73747576
 *  Le type du message est 0x44556677
 *  Le handle du message est 0x12345678
 *  L'offset du message est 0x10121012
 *  La longueur du message est 0x400

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
    int32_t error;
    uint32_t handle;
    char payload[MAX_BUFFER];
};

struct request_t* request;
struct response_t* response;
struct sockaddr_in client;  // socket info about the machine connecting to us
struct sockaddr_in server;  // socket info about our server

char buffer[MAX_BUFFER];

int mySocket;               // socket used to listen for incoming connections
int conSocket;

socklen_t socketSize;

void initServer();
int readMinix(char* arg);
int WriteMinix(char * arg);



int main (int argc, char** argv) {
    if (argv[1] == NULL) {
        perror("No input file ! Please specify a file name (with its path).\n");
        exit(1);
    }

    int nRead;
    int errorReturn;

    initServer();

    while (1)
    {
        if ((conSocket = accept(mySocket, (struct sockaddr*)&client, &socketSize)) < 0) {
            perror("Error to accept connexion");
            exit(1);
        }
        //Read the socket and save data into the buffer
        if ((nRead = (int) recv(conSocket, buffer, MAX_REQ, 0)) < 0) {
            perror("Error to read into the socket");
            exit(1);
        }
        // Copy the content of the buffer to the request structure
        request = (struct request_t*)buffer;

        printf("handle %d\n", ntohl(request->handle));
        printf("offset %d\n", ntohl(request->offset));
        printf("length %d\n", ntohl(request->length));

        if (ntohl(request->magic == MAGIC_REQUEST)) {
            switch (ntohl(request->type)) {
                case CMD_READ :
                    if ((errorReturn = readMinix(argv[1])) < 0) {
                        response->error = -errorReturn;
                        response->error = htonl(response->error);
                        write(conSocket, response, MAX_REQ);
                    }
                    break;
                case CMD_WRITE :
                    if ((errorReturn = WriteMinix(argv[1])) < 0) {
                        response->error = -errorReturn;
                        response->error = htonl(response->error);
                        write(conSocket, response, MAX_REQ);
                    }
                    break;
                default:
                    perror("Unknow mode. Read or Write nor found");
                    break;
            }
        } else
            printf("Le magic est pourri : %d\n", ntohl(request->magic));
        close(conSocket);
    }

    close(mySocket);
    return EXIT_SUCCESS;
}

void initServer() {
    memset(&server, 0, sizeof(struct sockaddr_in)); // zero the struct before filling the fields
    server.sin_family = AF_INET;                    // set the type of connection to TCP/IP IPv4
    server.sin_addr.s_addr = htonl(INADDR_ANY);     // set our address to any interface
    server.sin_port = htons(PORT_NUM);              // set the server port number

    if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error socket");
        exit(1);
    }
    // bind server information to mySocket
    if (bind(mySocket, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Server : error to bind client address");
        exit(1);
    }

    // start listening, allowing a queue of up to 1 pending connection
    if (listen(mySocket, MAX_CONNECTION) < 0) {
        perror("Error listen");
        exit(1);
    }
    memset(&client, 0, sizeof(struct sockaddr_in));

    request = malloc(sizeof(struct request_t));
    response = malloc(sizeof(struct response_t));
}

int readMinix(char* arg) {
    int myFile;
    //ouverture du fichier
    if ((myFile = open(arg, O_RDONLY))<0) {
        perror("Error to open file_read");
        return EXIT_FAILURE;
    }

    //on se positionne a la position de l'offset avec le curseur
    lseek(myFile, (int)(ntohl(request->offset)), SEEK_SET);

    // Read the size of  bloc in the file
    read(myFile, response->payload, (int)ntohl(request->length));

    //set the values for the client response
    response->magic = ntohl(MAGIC_RESPONSE);
    response->error = 0;
    response->handle = ntohl(request->handle);

    //Set the format to send to the client
    response->magic = htonl(response->magic);
    response->error = htonl(response->error);
    response->handle = htonl(response->handle);
    //response->payload = htonl(response->payload);

    //Send the response to the client
    write(conSocket, response, MAX_REQ);

    close(myFile);

    return EXIT_SUCCESS;
}

int WriteMinix(char* arg){
    //ouverture du fichier
    int myFile = open(arg, O_WRONLY);
    if (myFile < 0) {
        perror("Error to open file_write");
        return EXIT_FAILURE;
    }

    //on se positionne a la position de l'offset avec le curseur
    lseek(myFile, (int)(ntohl(request->offset)), SEEK_SET);

    // On écris le nombre de byte voulu dans le fichier
    write(myFile,request->payload,(int)ntohl(request->length));

    //Init the magic header of the response
    response->magic = ntohl(MAGIC_RESPONSE);
    //Set error code to 0
    response->error = 0;
    //Set the handle for the response
    response->handle = ntohl(request->handle);

    //Set the format to send to the client
    response->magic = htonl(response->magic);
    response->error = htonl(response->error);
    response->handle = htonl(response->handle);

    //Send the response to the client
    write(conSocket, response, MAX_RESP);

    close(myFile);
    return EXIT_SUCCESS;
}
