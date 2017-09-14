#include "../include/simulator.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define BUFFER_SIZE 20
unsigned short check;
char buffer[1000][21];
int i, j, k, ACKRECV, count;
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
	if(!ACKRECV)
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
	strncpy(packetA.payload, message.data, 20);
		}
		else
		{
		printf("Count: and i: %d %d", count, i);
		strncpy(buffer[i], message.data, 20);
			printf("Else:  Message in Buffer: %s",buffer[i]);
			i++;
		strncpy(packetA.payload, buffer[count], 20);
		//printf("%s", buffer[count]);
		count++;
		}
	packetA.checksum = checksum2(packetA.payload, 20);
	packetA.acknum=0;
	printf("Packet Payload: %s\n",packetA.payload);
	printf("Packet Checksum %d\n",packetA.checksum);
	packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
	tolayer3(0, packetA);
	printf("After sending the packet to layer 3\n");
	starttimer(0, 20);
	ACKRECV =0;
	return;
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
if(packet.seqnum == packetA.seqnum)
{
	if(packet.acknum)
	{check2 = packet.acknum + packet.seqnum;
	if(check2==packet.checksum)
	{
		stoptimer(0);
		ACKRECV = 1;
		printf("Timer stopped! Success!\n");
		if(packetA.seqnum == 1)
				packetA.seqnum = 0;
			else
				packetA.seqnum = 1;
	}
	}

}
return;
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	printf("Timer Expired!\n");
	tolayer3(0, packetA);
	starttimer(0, 20);
return;
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	ACKRECV = 1;
	k =0;
	count =0;
	packetA.seqnum = 0;
	packetA.acknum = 0;
	for(j=0;j<1000; j++)
	{
		//char* buffer[j] = (char*) malloc(sizeof(char)*BUFFER_SIZE);
		                 memset(buffer[j], '\0', BUFFER_SIZE);
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
if(expectedseqno == packet.seqnum)
{
check1 = check1+ packet.acknum + packet.seqnum;
if(check1 == packet.checksum)
{
	ack.acknum = 1;
	ack.seqnum = packet.seqnum;
	printf("%d", packet.seqnum);
	memset(ack.payload, 0, 20);
	ack.checksum = ack.acknum + ack.seqnum;
	tolayer3(1, ack);
	printf("Acknowledgment sent!\n");
	tolayer5(1, message);
	if(expectedseqno == 1)
					expectedseqno = 0;
				else
					expectedseqno = 1;
	printf("Message sent to application layer of B!\n");
	printf("%lf", get_sim_time());
}
}
else
{check1 = check1+ packet.acknum + packet.seqnum;
if(check1 == packet.checksum)
{
ack.acknum = 1;
ack.seqnum = packet.seqnum;
printf("%d", packet.seqnum);
memset(ack.payload, 0, 20);
ack.checksum = ack.acknum + ack.seqnum;
tolayer3(1, ack);
printf("Acknowledgment sent!\n");
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



