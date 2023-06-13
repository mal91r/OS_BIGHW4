#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[]) {
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    unsigned int size;
    char *servIP;                    /* Server IP address (dotted quad) */
    int variant;
    char variantString[3];
    char answer[100];
    char mark[3];

    if (argc != 3)    /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server IP>\n",
                argv[0]);
        exit(1);
    }

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

    printf("Студент вошел в аудиторию\n");

    sleep(0.5);
    srand(time(NULL));   // Initialization, should only be called once.
    variant = rand() % 30 + 1;
    printf("Студент вытянул билет %d\n", variant);

    sprintf(variantString, "%d", variant);

    /* Send the string to the server */
    if (sendto(sock, variantString, strlen(variantString), 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != strlen(variantString))
        DieWithError("sendto() sent a different number of bytes than expected");

    printf("Студент приступил к решению билета\n");
    printf("Введите ответ студента: ");
    fgets(answer, 100, stdin);
    if ((strlen(answer) > 0) && (answer[strlen (answer) - 1] == '\n'))
        answer[strlen (answer) - 1] = '\0';
    sleep(0.5);
    printf("Студент передал ответ преподавателю\n");

    /* Send the string to the server */
    if (sendto(sock, answer, strlen(answer), 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != strlen(answer))
        DieWithError("sendto() sent a different number of bytes than expected");
    sleep(0.5);
    printf("Студент ждёт, пока преподаватель поставит оценку\n");

    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((size = recvfrom(sock, mark, 2, 0,
         (struct sockaddr *) &fromAddr, &fromSize)) < 0)
        DieWithError("recvfrom() failed");
    mark[size] = '\0';


    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }
    
    sleep(0.5);
    printf("Студент узнает, что преподаватель поставил ему оценку %s\n", mark);
    sleep(0.5);
    printf("Студент покидает аудиторию!\n");
    printf("\n");    /* Print a final linefeed */
    sleep(1);
    close(sock);
    exit(0);
}
