
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include "mpi.h"
#include <windows.h>
#include <queue> 
#include <string>
#include<signal.h>
#include <iostream>
using namespace std;
#define MAX_PHIL 10
//Message Identificator Tags
#define REQUEST 1
#define RESPONSE 2
#define RELEASE 3

void sig_handler(int signo)
{
	
	printf("SIG_HANDLER CALLED");
		exit(1);
	
	
}

void tabulator(string out,int tabs) {//Generalitation of Output with Tabs
	int i = 0;
	for (i; i <= tabs;i++) {
		printf("\t");
	}
	//printf("%s\n", out);

	cout << out << endl;
	
}

void philosopher(int myRank) {//Philosopher Funct
	int buffer;
	MPI_Status stat;
	srand(time(NULL) + myRank);

	
	while (true) {//Philosopher loop

		Sleep(rand() % 10); //Think
		MPI_Send(&buffer, 1, MPI_INT, 0, REQUEST, MPI_COMM_WORLD); //Request forks
		printf("[N_%d] ", myRank);
		tabulator("## Wait for resposed ##", myRank);
		MPI_Recv(&buffer, 1, MPI_INT, 0, RESPONSE, MPI_COMM_WORLD, &stat);//wait response
		printf("[N_%d] ", myRank);
		tabulator("## Eating ##", myRank);
		Sleep(rand() % 10);//Eat
		

		MPI_Send(&buffer, 1, MPI_INT, 0, RELEASE, MPI_COMM_WORLD);//Release forks
		printf("[N_%d] ", myRank);
		tabulator("## Leave Forks ##", myRank);
	}
}



void monitor(int myrank, int nprocs) {
	
	int buffer;//Buffer used for Messaging
	int philosopherRank;
	MPI_Status stat;
	queue<int> queue;	

	int f = 0;//Iniciate all forks as avalible
	bool forks[MAX_PHIL];
	for (f; f <= nprocs; f++) {
		forks[f] = true;
	}
	
	//Monitor loop
	while (true) {

		MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); // Recive next message
		philosopherRank = stat.MPI_SOURCE; //Read source of message-Rank of philosopher


		if (stat.MPI_TAG == RELEASE) { //Philosopher wants to realease the forks
		
		//Philosopher loop
		forks[(((philosopherRank - 1) % (nprocs - 1)) + 1) % (nprocs - 1)] = true; 
		forks[philosopherRank - 1] = true;

		}else if (queue.empty()) {//The queue of fork request is empty

			if (stat.MPI_TAG == REQUEST) { //If Request for forks
				if (forks[(((philosopherRank - 1)%(nprocs - 1)) + 1) %(nprocs - 1)] == true && forks[philosopherRank - 1] == true) { //If both forks are free
					printf("[N_%d] ", philosopherRank);
					tabulator("## Picking up Forks ##", philosopherRank);
					forks[(((philosopherRank - 1) % (nprocs - 1)) + 1) % (nprocs - 1)] = false; //Set the forks as taken
					forks[philosopherRank - 1] = false;
					MPI_Send(&buffer, 1, MPI_INT, philosopherRank, RESPONSE, MPI_COMM_WORLD); // Send Fork response to the right philosopher

				}
				else {//If Not both forks are free
					printf("[N_%d] ", philosopherRank);
					tabulator("## Enter Queue ##", philosopherRank);
					queue.push(philosopherRank); //Put in wait queue
				}

			}

		}else {//There is philosophers Waiting 

				queue.push(philosopherRank); //Put in queue the newCommer
				philosopherRank = (int)(queue.front());
				printf("[N_%d] ", philosopherRank);
				tabulator("## Exiting Queue ##", philosopherRank);
				queue.pop();
				forks[(((philosopherRank - 1) % (nprocs - 1)) + 1) % (nprocs - 1)] = false; //Set the forks as taken
				forks[philosopherRank - 1] = false;
				MPI_Send(&buffer, 1, MPI_INT, philosopherRank, RESPONSE, MPI_COMM_WORLD); // Send Fork response to the right philosopher

		}

	}
}


int main(int argc, char** argv) {
	enum role { MONITOR, PHILOSOPHER};
	int myRank, comm_size;
	
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("********** Error Creating Sig_Handler **********\n");

	switch (myRank)
	{
	case MONITOR:
	{
		monitor(myRank, comm_size);
		break;
	}
	default:
	{
		philosopher(myRank);
		break;
	}
	}

	MPI_Finalize();
	return 0;

}