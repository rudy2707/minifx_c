#include <stdio.h>
#include <arpa/inet.h>

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

   msg_aligned :
    Le magic du message est 0x73747576
    Le type du message est 0x44556677
    Le handle du message est 0x12345678
    L'offset du message est 0x10120000
    La longueur du message est 0x400

*  Explication : on accède à la même zone mémoire, pour
   struct msg_aligned, le champs offset bien que déclaré
   en short (16 bits), est affiché sur 32 bits (opérateur %x),
   avec le padding (0x0000) de 16 bits en bonus...

*/

struct __attribute__ ((__packed__)) msg { 
    int magic;
    int type;
    int handle;
    int offset;
    int length;
};

struct msg_aligned { 
    int magic;
    int type;
    int handle;
    short offset;
    int length;
};

int main(int argc, char *argv[])
{
    char buffer[1024] = {0x76,0x75,0x74,0x73,
                         0x44,0x55,0x66,0x77,
                         0x12,0x34,0x56,0x78,
                         0x10,0x12,0x10,0x12,
                         0x00,0x00,0x04,0x00};

    struct msg *m;
    m = (struct msg *)buffer;
    printf("msg :\n");
    printf("\tLe magic du message est 0x%x\n", m->magic);
    printf("\tLe type du message est 0x%x\n", ntohl(m->type));
    printf("\tLe handle du message est 0x%x\n", ntohl(m->handle));
    printf("\tL'offset du message est 0x%x\n", ntohl(m->offset));
    printf("\tLa longueur du message est 0x%x\n", ntohl(m->length));
    
    struct msg_aligned *m2;
    m2 = (struct msg_aligned *)buffer;
    printf("msg_aligned :\n");
    printf("\tLe magic du message est 0x%x\n", m2->magic);
    printf("\tLe type du message est 0x%x\n", ntohl(m2->type));
    printf("\tLe handle du message est 0x%x\n", ntohl(m2->handle));
    printf("\tL'offset du message est 0x%x\n", ntohl(m2->offset));
    printf("\tLa longueur du message est 0x%x\n", ntohl(m2->length));

}
