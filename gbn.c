#include "../include/simulator.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define BUFFER_SIZE 20
unsigned short check;
char buffer[1000][21];
char window[1000][21];
int i, j, k, ACKRECV, count, sendbase, nextseqno, N, timeron, bufcount, check3;
int check1, check2, expectedseqno;
char message[20];

struct pkt packetA, ack;
//the below code is taken from https://locklessinc.com/articles/tcp_checksum/
unsigned short checksum2(const char *buf, unsigned size)
{
	unsigned long long sum = 0;
	const unsigned long long *b = (unsigned long long *) buf;

	unsigned t1, t2;
	unsigned short t3, t4;

	/* Main loop - 8 bytes at a time */
	while (size >= sizeof(unsigned long long))
	{
		unsigned long long s = *b++;
		sum += s;
		if (sum < s) sum++;
		size -= 8;
	}

	/* Handle tail less than 8-bytes long */
	buf = (const char *) b;
	if (size & 4)
	{
		unsigned s = *(unsigned *)buf;
		sum += s;
		if (sum < s) sum++;
		buf += 4;
	}

	if (size & 2)
	{
		unsigned short s = *(unsigned short *) buf;
		sum += s;
		if (sum < s) sum++;
		buf += 2;
	}

	if (size)
	{
		unsigned char s = *(unsigned char *) buf;
		sum += s;
		if (sum < s) sum++;
	}

	/* Fold down to 16 bits */
	t1 = sum;
	t2 = sum >> 32;
	t1 += t2;
	if (t1 < t2) t1++;
	t3 = t1;
	t4 = t1 >> 16;
	t3 += t4;
	if (t3 < t4) t3++;

	return ~t3;
}
//the above code is taken from https://locklessinc.com/articles/tcp_checksum/

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
	printf("Before if!");
	printf("Message Data :%s", message.data);
	if((nextseqno-sendbase)>=N)
	{
	printf("Before buffer\n");
	strncpy(buffer[i], message.data, 20);
	printf("If:  Message in Buffer: %s",buffer[i]);
	i++;
	return;
	}
	else
	{

		if(i==0)
		{
	count++;

			strncpy(window[nextseqno], message.data, 20);
				//count++;
		}
		else
		{   strncpy(buffer[i], message.data, 20);
			i++;
			printf("%d", i);
			strncpy(window[nextseqno], buffer[bufcount], 20);
			bufcount++;
			printf("%d", nextseqno);
		}
	}
		strncpy(packetA.payload, window[nextseqno], 20);
	packetA.checksum = checksum2(packetA.payload, 20);

	printf("Packet Payload: %s\n",packetA.payload);
	printf("Packet Checksum %d\n",packetA.checksum);
	packetA.seqnum = nextseqno;
	packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
	tolayer3(0, packetA);
	printf("After sending the packet to layer 3\n");
	if((sendbase == nextseqno)&&timeron)
	{
	starttimer(0, 35);
	printf("Timer started!");
	timeron=0;
	}
	nextseqno++;
//	ACKRECV =0;
	return;
	}


/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
	check3 = packet.acknum + packet.seqnum;
	printf("Check %d",check1);
	printf("Packet Checksum %d",packet.checksum);
	if(check3 == packet.checksum)
	{

	sendbase = packet.acknum+1;
	if(sendbase == nextseqno)
	{
		stoptimer(0);
		printf("Timer stopped!");
	    timeron =1;
	}

	else if(timeron)
	{
		starttimer(0, 35);
	printf("Timer started in AInput");
	}
	}
return;
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	printf("Timer Expired!\n");
	starttimer(0, 35);
	timeron = 0;
	for(j=sendbase; j<=nextseqno-1;j++)
	{
		strncpy(packetA.payload, window[j], 20);
			packetA.checksum = checksum2(packetA.payload, 20);
			printf("%s", packetA.payload);
			packetA.seqnum = j;
			printf("%d", j);
			packetA.acknum =0;
			packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
			tolayer3(0, packetA);
	}
return;
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

	k =0;
	N=getwinsize();
	printf("%d",N);
	count =0;
	timeron =1;
	sendbase =0;
	nextseqno = 0;
	bufcount=0;
	packetA.seqnum = 0;
	packetA.acknum = 0;

	for(k=0;k<1000; k++)
	{
		//char* buffer[j] = (char*) malloc(sizeof(char)*BUFFER_SIZE);
		                 memset(buffer[k], '\0', BUFFER_SIZE);
	}
	i=0;
return;
}


/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
	printf("Packet received!\n");
	strncpy(message,packet.payload, 20);
check1 = checksum2(message, 20);
printf("%d",check1);
printf("%s",message);
//packet.acknum =0;
if(expectedseqno == packet.seqnum)
{
check1 = check1+ packet.acknum + packet.seqnum;
if(check1 == packet.checksum)
{
	ack.acknum = expectedseqno;
	ack.seqnum = packet.seqnum;
	printf("%d", packet.seqnum);
	memset(ack.payload, 0, 20);
	ack.checksum = ack.acknum + ack.seqnum;
	tolayer3(1, ack);
	printf("Acknowledgment sent!\n");
	tolayer5(1, message);
	expectedseqno++;
	printf("Message sent to application layer of B!\n");
}
}
return;

}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	expectedseqno = 0;
	unsigned short check;
	check = 0;
return;
}



