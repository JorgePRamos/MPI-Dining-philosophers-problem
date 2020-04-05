
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include "mpi.h"
#include <windows.h>
#include <queue> 
using namespace std;
//Message Tags
#define REQUEST 1
#define RESPONSE 2
#define RELEASE 3


void philosopher(int myRank) {
	int buffer;
	MPI_Status stat;
	srand(time(NULL) + myRank);

	//Philosopher main loop
	while (true) {

		Sleep(rand() % 10); //Think
		MPI_Send(&buffer, 1, MPI_INT, 0, REQUEST, MPI_COMM_WORLD); //Request forks
		MPI_Recv(&buffer, 1, MPI_INT, 0, RESPONSE, MPI_COMM_WORLD, &stat);//wait response
		Sleep(rand() % 10);//Eat
		MPI_Send(&buffer, 1, MPI_INT, 0, RELEASE, MPI_COMM_WORLD);//Release forks

	}
}



void table(int myrank, int nprocs) {
	printf("Hello from table %d \n", myrank);
	int buffer;

	int philosopher;
	MPI_Status stat;

	//std::list<int> queue;
	queue<int> queue;
	

	bool forks[5] = { true ,true ,true ,true , true };//SUBSTRAER 1 POR QUE EL 0 ES EL MONITOR
	for (int i = 0; i < nprocs - 1; i++) forks[i] = true; //Init all forks as free

	while (true) {


		MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); // Recive next message
		philosopher = stat.MPI_SOURCE; //Read source of message NUMBER OF PHIL



		if (stat.MPI_TAG == RELEASE) { //If Release of forks ***************POSIBLE PROBLEMA****
		forks[philosopher % (nprocs - 1)] = true; //Set forks to free again
		forks[philosopher - 1] = true;

	
		}else if (queue.empty()) {//We take new


				if (stat.MPI_TAG == REQUEST) { //If Request for forks
					if (forks[philosopher % (nprocs - 1)] == true && forks[philosopher - 1] == true) { //If both forks are free
						forks[philosopher % (nprocs - 1)] = false; //Set the forks as taken
						forks[philosopher - 1] = false;
						MPI_Send(&buffer, 1, MPI_INT, philosopher, RESPONSE, MPI_COMM_WORLD); // Send Fork response to the right philosopher

					}
					else {//If not both forks are free
						queue.push(philosopher); //Put in wait queue
					}

				}


		}else {
				queue.push(philosopher); //Put in wait queue  the newCommer
				philosopher = (int)(queue.front());
				queue.pop();
				forks[philosopher % (nprocs - 1)] = false; //Set the forks as taken
				forks[philosopher - 1] = false;
				MPI_Send(&buffer, 1, MPI_INT, philosopher, RESPONSE, MPI_COMM_WORLD); // Send Fork response to the right philosopher
		}

	}
}

int main(int argc, char** argv) {
	enum role { MONITOR, PHILOSOPHER};
	int myRank, nprocs;
	//init MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	printf("say HI.  %d", argv);

	switch (myRank)
	{
	case MONITOR:
	{
		//ble(myRank, nprocs);
		break;
	}
	case PHILOSOPHER:
	{
		philosopher(myRank);
		break;
	}
	}
	//Depending on rand, Philosopher or Table
	//if (myRank == 0) table(myRank, nprocs);
//	else philosopher(myRank);

	MPI_Finalize();
	return 0;

}