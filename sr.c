#include "../include/simulator.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define BUFFER_SIZE 20
#define ACKSIZE 5000
unsigned short check;
char buffer[1000][21];
char window[5000][21];
char receiverwindow[10000][21];
int i, j, k, ACKRECV, count, sendbase, nextseqno, N, timeron, bufcount, lastseqno, remtime,n;
int check1, check2, check3, expectedseqno, ackno, t1, t2, t3, position;
char message[20];
void* r;
int acknowledgment[5000];
struct time1 {

int simtime;
int timerid;
struct time1* next;

};//same as node t is as head.. use append(**t, int,int,int) //geeks ka ref

//the below part is taken from http://quiz.geeksforgeeks.org/linked-list-set-2-inserting-a-node/
void append(struct time1** t, int simtimet, int timeridt)
{
    /* 1. allocate node */
    struct time1* new_node = (struct time1*) malloc(sizeof(struct time1));

    struct time1 *last = *t;  /* used in step 5*/

    /* 2. put in the data  */
    new_node->simtime  = simtimet;

    new_node->timerid  = timeridt;
    /* 3. This new node is going to be the last node, so make next of
          it as NULL*/
    new_node->next = NULL;

    /* 4. If the Linked List is empty, then make the new node as head */
    if (*t == NULL)
    {
       *t = new_node;
       return;
    }

    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;

    /* 6. Change the next of last node */
    last->next = new_node;
    return;
}
//the above part is taken from http://quiz.geeksforgeeks.org/linked-list-set-2-inserting-a-node/

struct time1 *t  = NULL;
struct time1 *temp, *prev = NULL;
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
//the below code is taken from http://quiz.geeksforgeeks.org/delete-a-linked-list-node-at-a-given-position/
void deleteNode(struct time1 **head_ref, int position)
{
   // If linked list is empty
   if (*head_ref == NULL)
      return;

   // Store head node
   struct time1* temp1 = *head_ref;

    // If head needs to be removed
    if (position == 0)
    {
        *head_ref = temp1->next;   // Change head
        free(temp1);               // free old head
        return;
    }

    // Find previous node of the node to be deleted
    for (int i=0; temp1!=NULL && i<position-1; i++)
         temp1 = temp1->next;

    // If position is more than number of ndoes
    if (temp1 == NULL || temp1->next == NULL)
         return;

    // Node temp->next is the node to be deleted
    // Store pointer to the next of node to be deleted
    struct time1 *next = temp1->next->next;

    // Unlink the node from linked list
    free(temp1->next);  // Free memory

    temp1->next = next;  // Unlink the deleted node from list
}
//the above code is taken from http://quiz.geeksforgeeks.org/delete-a-linked-list-node-at-a-given-position/

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
	struct time1 *a  = NULL;
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
			strncpy(window[nextseqno], message.data, 20);
		}
		else
		{   strncpy(buffer[i], message.data, 20);
			i++;
			//printf("%d", i);
			strncpy(window[nextseqno], buffer[bufcount], 20);
			bufcount++;
			//count++;
			//printf("%d", nextseqno);
		}
	strncpy(packetA.payload, window[nextseqno], 20);
	packetA.checksum = checksum2(packetA.payload, 20);

	printf("Packet Payload: %s\n",packetA.payload);
	printf("Packet Checksum %d\n",packetA.checksum);
	packetA.seqnum = nextseqno;
	printf("%d", packetA.seqnum);
	packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
	tolayer3(0, packetA);
	printf("After sending the packet to layer 3\n");

//	r = malloc( sizeof(struct time1));
//	if(!r)
//		{
//		printf("NULL");
//		}
//
//	printf("NULL1");
//	fflush(stdout);
	//a = (struct time *) malloc( sizeof(struct time) );
	a = NULL;
	a = (struct time1 *) malloc( sizeof(struct time1));
	printf("After allocation");
if(nextseqno==0)
{
	starttimer(0, 20);
	timeron =1;
	temp = t;
//	while(temp!=NULL)
//		                	{temp = temp->next;
//		                	}
	temp= a;
		temp->simtime = get_sim_time();
		//printf("Simulation time for packet 0 :%lf", temp->simtime);
		temp->timerid = nextseqno;
		temp->next = NULL;
		t =temp;
		//free (a);

}
else
{ append(&t, get_sim_time(), nextseqno);
//printf("Simulation time for packet %d :%l",nextseqno, get_sim_time());
//	temp = t;
//printf("Inside else");
//	                 while(temp->next!=NULL)
//	                	{temp = temp->next;
//	                	}
//	                 temp =temp->next;
//	                 temp= a;
//	temp->simtime = get_sim_time();
//	temp->timerid = nextseqno;
//	temp->next = NULL;
	//free (a);
	//temp=temp->next;

}
//	if(sendbase == nextseqno)
//	{
//	//starttimer(0, 50);
//	//printf("Timer started!");
//	//timeron=0;
//	}
	nextseqno++;
//	ACKRECV =0;
	return;
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{//check1 = checksum2(message, 20);

printf("%s",message);
check3 = packet.acknum + packet.seqnum;
printf("Check %d",check1);
printf("Packet Checksum %d",packet.checksum);
if(check3 == packet.checksum)
{

	sendbase = packet.acknum;
	printf("SENDBASE: %d", sendbase);
	fflush(stdout);

	if((sendbase == ackno)&&timeron)
	{
		stoptimer(0);
		printf("Timer stopped! Success");
		fflush(stdout);
		timeron =0;
		ackno++;
		acknowledgment[sendbase] = 1;
		temp = t;
		prev = t;
		position =1;
		while(temp->next!=NULL)
			{
			printf("Inside tenp NULL loop!");
			fflush(stdout);
			if(temp->timerid == sendbase)
			{
				printf("Inside tenp NULL SENDBASE\n");
							fflush(stdout);
				//deleteNode(&t, position);
				printf("Inside tenp NULL SENDBASE\n");
											fflush(stdout);
//				printf("Timer ID :%d",temp->timerid);
//				prev = temp;
//				prev=temp->next;
//				temp->next = prev->next;
//				//temp =temp->next;
			}
			position++;
			temp = temp->next;
			printf("Inside tenp NULL SENDBASE\n");
										fflush(stdout);
				}
		t = temp;
		printf("After acknowledgment sendbase!");
		fflush(stdout);

//		temp=t;
//		while(temp->next!=NULL)
//		{
//
//		}
			printf("After entering in temp NULL loop!");
			fflush(stdout);

			//t=t->next;
			temp=t;
			while(temp->next!=NULL)
			{
				t1= temp->simtime;
				t3 = get_sim_time();
				sendbase++;
				ackno++;
				if(temp->timerid == ackno)
				{
				if(acknowledgment[ackno]==1)
				{
					;
				}
				else
				{
					remtime = temp->simtime+20-t3;
					if((remtime>0)&&timeron==0)
					{
						starttimer(0, remtime);
						printf("Timer started for %d", remtime);
						timeron =1;
					}
					else
					{
						strncpy(packetA.payload, window[sendbase], 20);
							packetA.checksum = checksum2(packetA.payload, 20);
							packetA.seqnum = sendbase;
							packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
							tolayer3(0, packetA);
					}

				}
				}
				temp=temp->next;
			}


	}
//	else if(ackno<sendbase)
//	{
//		printf("Inside If ackno<sendbase\n");
//		fflush(stdout);
//		temp=t;
//					while(temp->next!=NULL)
//					{
//						printf("Inside Linked list\n");
//						fflush(stdout);
//						if(temp->timerid == ackno)
//						{
//							printf("Inside If timerid = ackno\n");
//						t1= temp->simtime;
//						t3 = get_sim_time();
//						//sendbase++;
//						remtime = temp->simtime+20-t3;
//							if((remtime>0)&&timeron==0)
//							{
//								starttimer(0, remtime);
//								printf("Timer started for %d", remtime);
//								timeron =1;
//							}
//							else
//							{
//								strncpy(packetA.payload, window[ackno], 20);
//									packetA.checksum = checksum2(packetA.payload, 20);
//									packetA.seqnum = ackno;
//									packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
//									tolayer3(0, packetA);
//							}
//
//
//
//						}
//						temp=temp->next;
//					}
//
//	}
	else
	{
		printf("Inside else Ainput\n");
		fflush(stdout);
		acknowledgment[sendbase]=1;
		temp = t;
		prev = t;

//		while(temp->next!=NULL)
//		{
//		if(sendbase==temp->timerid)
//		{
//			strncpy(packetA.payload, window[sendbase], 20);
//										packetA.checksum = checksum2(packetA.payload, 20);
//										packetA.seqnum = sendbase;
//										packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
//										tolayer3(0, packetA);
//										if(!timeron)
//										{
//											starttimer(0, 20);
//										}
//										else
//										{
//											temp->simtime = get_sim_time();
//										}
////			printf("%d",sendbase);
////
////					prev = temp;
////					temp=temp->next;
////					prev->next = temp->next;
//		}
//		temp = temp->next;
//		}
	}
//	else
//	{
//		temp = t;
//		prev = t;
//		for(n =0; n<sendbase-1;n++)
//		{
//			prev = temp;
//			temp=temp->next;
//		}
//		prev->next = temp->next;
//	}

//	else if(timeron)
//	{
//		starttimer(0, 50);
//	printf("Timer started in AInput");
//	}
}
return;
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	printf("Timer Expired!\n");
	strncpy(packetA.payload, window[sendbase], 20);
	packetA.checksum = checksum2(packetA.payload, 20);
	packetA.seqnum = sendbase;
	tolayer3(0, packetA);
	if(!timeron)
	{
		starttimer(0, 20);
		timeron =1;
	}
	else
	{
		temp->simtime = get_sim_time();
	}
//	starttimer(0, 50);
//	timeron = 0;
//	for(j=sendbase; j<=nextseqno-1;j++)
//	{
//		strncpy(packetA.payload, window[j], 20);
//			packetA.checksum = checksum2(packetA.payload, 20);
//			printf("%s", packetA.payload);
//			packetA.seqnum = j;
//			printf("%d", j);
//			packetA.acknum =0;
//			packetA.checksum = packetA.checksum + packetA.acknum +packetA.seqnum;
//			tolayer3(0, packetA);
//	}
return;
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

	k =0;
	check3 =0;
	struct time *t = NULL;
	N=getwinsize();
	ackno=0;
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
	  memset(buffer[k], 0, BUFFER_SIZE);
	}
	int acknowledgment[5000] ={0};
//	for(k=0;k<5000; k++)
//		{
//		acknowledgment[k]=0;
//		}
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
printf("Check1 in Binput %d",check1);
printf("Message received :%s",message);
check1 = check1+ packet.acknum + packet.seqnum;
if(check1 == packet.checksum)
{
strncpy(receiverwindow[packet.seqnum],message, 20);

ack.acknum = packet.seqnum;
printf("Packet Sequence No :%d", packet.seqnum);
ack.seqnum = packet.seqnum;
memset(ack.payload, 0, 20);
ack.checksum = ack.acknum + ack.seqnum;
printf("Acknowledgment Checksum: %d", ack.checksum);

tolayer3(1, ack);

if(lastseqno== packet.seqnum)
{
	printf("Inside if loop");
	tolayer5(1, receiverwindow[lastseqno]);
	printf("After send 1st packet");
	lastseqno++;
	printf("Before while");
	while(strcmp(receiverwindow[lastseqno], "0"))
	{
		printf("Inside while");
		tolayer5(1, receiverwindow[lastseqno]);
		lastseqno++;
	}
}
}
return;

}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	lastseqno =0;
	expectedseqno = 0;
	unsigned short check;
	check = 0;
	for(k=0;k<10000; k++)
		{
		strcpy(receiverwindow[k], "0");
			//char* buffer[j] = (char*) malloc(sizeof(char)*BUFFER_SIZE);
		//memset(receiverwindow[k], 1 , BUFFER_SIZE);
		}
return;
}



