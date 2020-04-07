
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

		//Sleep(rand() % 10); //Think
		Sleep(1);
		MPI_Send(&buffer, 1, MPI_INT, 0, REQUEST, MPI_COMM_WORLD); //Request forks
		printf("Im philo n %d ## Wait for resposed\n", myRank);
		MPI_Recv(&buffer, 1, MPI_INT, 0, RESPONSE, MPI_COMM_WORLD, &stat);//wait response
		printf("Im philo n %d ## COMO\n", myRank);
		//Sleep(rand() % 10);//Eat
		Sleep(1);

		MPI_Send(&buffer, 1, MPI_INT, 0, RELEASE, MPI_COMM_WORLD);//Release forks
		printf("Im philo n %d ## Dejo los tenedores\n", myRank);
	}
}



void monitor(int myrank, int nprocs) {
	printf("Hello from monitor %d \n", myrank);
	int buffer;
	
	int philosopherRank;
	MPI_Status stat;


	queue<int> queue;
	

	bool forks[5] = { true ,true ,true ,true , true };//SUBSTRAER 1 POR QUE EL 0 ES EL MONITOR
	

	while (true) {


		MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); // Recive next message
		philosopherRank = stat.MPI_SOURCE; //Read source of message NUMBER OF PHIL



		if (stat.MPI_TAG == RELEASE) { //If Release of forks ***************POSIBLE PROBLEMA****
	

		forks[(((philosopherRank - 1) % (nprocs - 1)) + 1) % (nprocs - 1)] = true; //Set the forks as taken
		forks[philosopherRank - 1] = true;

	
		}else if (queue.empty()) {//We take new
				//mine (rank -1)
				//next (((rank-1)mod (nProc-1))+1) mod (nProc-1)
			//forks[(((philosopherRank - 1)%(nprocs - 1)) + 1) %(nprocs - 1)] == true && forks[philosopherRank - 1] == true

				if (stat.MPI_TAG == REQUEST) { //If Request for forks
					if (forks[(((philosopherRank - 1)%(nprocs - 1)) + 1) %(nprocs - 1)] == true && forks[philosopherRank - 1] == true) { //If both forks are free
						printf("Im philo n %d ## TAKING FORKS\n", philosopherRank);
						forks[(((philosopherRank - 1) % (nprocs - 1)) + 1) % (nprocs - 1)] = false; //Set the forks as taken
						forks[philosopherRank - 1] = false;
						MPI_Send(&buffer, 1, MPI_INT, philosopherRank, RESPONSE, MPI_COMM_WORLD); // Send Fork response to the right philosopher

					}
					else {//If not both forks are free
						printf("Im philo n %d ## GOING IN TO QUEUE\n", philosopherRank);						
						queue.push(philosopherRank); //Put in wait queue
					}

				}


		}else {
				queue.push(philosopherRank); //Put in wait queue  the newCommer
				philosopherRank = (int)(queue.front());
				printf("Im philo n %d ## COMING OUT OF QUEUE\n", philosopherRank);
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