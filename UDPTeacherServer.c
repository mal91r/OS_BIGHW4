#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXPENDING 5    /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int cliAddrLen;         /* Length of incoming message */
    unsigned int size;
    int cntOfStudents;
    int studentsPaseed = 0;

    if (argc != 3)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    cntOfStudents = atoi(argv[2]);

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
        
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    //printf("Server IP address = %s. Wait...\n", inet_ntoa(echoClntAddr.sin_addr));
    printf("Преподаватель готов принимать экзамен и ждёт первого студента!\n\n");
    
	char variant[3];        /* Buffer for echo string */
	char answer[100];
	int mark;
	char markString[3];
	
    for (;;) /* Run forever */
    {
	/* Set the size of the in-out parameter */
	cliAddrLen = sizeof(echoClntAddr);

	/* Block until receive message from a client */
	if ((size = recvfrom(sock, variant, 2, 0,
	    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
	    DieWithError("recvfrom() failed");
	variant[size] = '\0';
	//printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
	sleep(1);
	printf("Преподаватель предлагает студенту вытянуть билет\n");
	sleep(0.5);
	printf("Студент сообщил преподавателю, что вытянул билет %s\n", variant);

	if ((size = recvfrom(sock, answer, 100, 0,
	    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
	    DieWithError("recvfrom() failed");
	answer[size] = '\0';
	sleep(0.5);
	printf("Преподаватель получил от студента ответ на задание: \n%s\n", answer);

	srand(time(NULL));   // Initialization, should only be called once.
	mark = rand() % 10 + 1;

	sleep(0.5);
	printf("Преподаватель проверяет работу и ставит оценку: %d\n", mark);

	sprintf(markString, "%d", mark);

        /* Send received datagram back to the client */
        if (sendto(sock, markString, strlen(markString), 0,
             (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) < 0)
            DieWithError("sendto() sent a different number of bytes than expected");

        studentsPaseed++;

        printf("\n[%d/%d]\n\n", studentsPaseed, cntOfStudents);

        if (studentsPaseed == cntOfStudents) {
            printf("Все студенты завершили сдачу экзамена, преподаватель может уходить!\n\n");
            close(sock);
            exit(0);
        }
        sleep(0.5);
        printf("Преподаватель готов принимать следующего студента\n\n");
    }
    /* NOT REACHED */
}
