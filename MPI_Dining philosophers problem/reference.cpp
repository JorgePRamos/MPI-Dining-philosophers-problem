/*

  Compile:
	mpic++ philosophers.cpp -o philosophers

  Use:
	mpiexec -np 6 philosophers


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include "mpi.h"
#include <windows.h>

void philosopher(int);
void table(int, int);
#define FORK_REQUEST 1
#define FORK_RESPONSE 2
#define FORK_RELEASE 3
#define DEBUG 1
/*
int main(int argc, char** argv) {
	int myrank, nprocs;
	//init MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//Depending on rand, Philosopher or Table
	if (myrank == 0) table(myrank, nprocs);
	else philosopher(myrank);

	MPI_Finalize();
	return 0;
}

void philosopher(int myrank) {
	if (DEBUG) printf("Hello from philosopher %d \n", myrank);
	int in_buffer[1];
	int out_buffer[1];
	MPI_Status stat;
	out_buffer[0];
	srand(time(NULL) + myrank);

	//Philosopher main loop
	while (true) {

		if (DEBUG) printf("Philosopher %d is sleeping \n", myrank);
		Sleep(rand() % 10); //Sleep
		if (DEBUG) printf("Philosopher %d is whaiting to eat \n", myrank);

		MPI_Send(out_buffer, 1, MPI_INT, 0, FORK_REQUEST, MPI_COMM_WORLD); //Request forks
		MPI_Recv(in_buffer, 1, MPI_INT, 0, FORK_RESPONSE, MPI_COMM_WORLD, &stat); //Whait for response
		if (DEBUG) printf("Philosopher %d is eating \n", myrank);
		Sleep(rand() % 10); //Eat
		if (DEBUG) printf("Philosopher %d is done eating \n", myrank);
		MPI_Send(out_buffer, 1, MPI_INT, 0, FORK_RELEASE, MPI_COMM_WORLD); //Release forks

	}
}

void table(int myrank, int nprocs) {
	printf("Hello from table %d \n", myrank);
	int in_buffer[1];
	int out_buffer[1];
	int philosopher;
	MPI_Status stat;

	std::list<int> queue;

	bool fork[nprocs - 1];
	for (int i = 0; i < nprocs - 1; i++) fork[i] = true; //Init all forks as free

	//Table main loop
	while (true) {
		MPI_Recv(in_buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); // Recive next message
		philosopher = stat.MPI_SOURCE; //Read source of message

		if (stat.MPI_TAG == FORK_REQUEST) { //If Request for forks
			if (DEBUG) printf("Table got philosopher %d fork request\n", philosopher);
			if (fork[philosopher % (nprocs - 1)] == true && fork[philosopher - 1] == true) { //If both forks are free
				fork[philosopher % (nprocs - 1)] = false; //Set the forks as taken
				fork[philosopher - 1] = false;
				MPI_Send(out_buffer, 1, MPI_REAL, philosopher, FORK_RESPONSE, MPI_COMM_WORLD); // Send Fork response to the right philosopher
				if (DEBUG) printf("Table sent philosopher %d the forks\n", philosopher);
			}
			else //If not both forks are free
				queue.push_back(philosopher); //Put in wait queue
		}
		if (stat.MPI_TAG == FORK_RELEASE) { //If Release of forks
			fork[philosopher % (nprocs - 1)] = true; //Set forks to free again
			fork[philosopher - 1] = true;
			if (DEBUG) printf("Table got philosopher %d fork release\n", philosopher);

			if (!queue.empty()) { //If philosopher whaiting for forks
				for (std::list<int>::iterator it = queue.begin(); it != queue.end(); it++) { //Go through whole list of whaiting philosophers
					philosopher = *it;
					if (fork[philosopher % (nprocs - 1)] == true && fork[philosopher - 1] == true) { //If one of them can get both forks
						fork[philosopher % (nprocs - 1)] = false;
						fork[philosopher - 1] = false;
						MPI_Send(out_buffer, 1, MPI_INT, philosopher, FORK_RESPONSE, MPI_COMM_WORLD); // send Fork response
						if (DEBUG) printf("Table sent philosopher %d the forks\n", philosopher);
						it = queue.erase(it); //Remove from wait list
					}
				}
			}
		}
	}
}


*/