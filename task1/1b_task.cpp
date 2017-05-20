#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int recv_and_send (long N, int processQuantity);

int main (int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s N \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	MPI_Init(&argc, &argv);

	char* endPtr = 0;

	long N = strtol(argv[1], &endPtr, 0);

	int myRank, childrenQuantity;

	MPI_Comm_size(MPI_COMM_WORLD, &childrenQuantity);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	int tmpMessage = 0;
	MPI_Status status;
	if (myRank == 0){
		for (long i = 0; i < N; i++)
		{

			MPI_Send(&tmpMessage, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			printf("thread 0 send message to 1\n");
			MPI_Recv(&tmpMessage, 1, MPI_INT, MPI_ANY_SOURCE,
				 0, MPI_COMM_WORLD, &status);
			printf("thread 0 get message from %d\n", status.MPI_SOURCE);
		}

		MPI_Finalize();
		return 0;
	}

	recv_and_send (N, childrenQuantity);

	MPI_Finalize();
	return 0;
}

int recv_and_send (long N, int processQuantity)
{
	MPI_Status status;
	int tmpMessage;
	for (long i = 0; i < N; i++)
	{
		MPI_Recv(&tmpMessage, 1, MPI_INT, MPI_ANY_SOURCE,
			 0, MPI_COMM_WORLD, &status);
		printf("get message from %d\n", status.MPI_SOURCE);
		MPI_Send(&tmpMessage, 1, MPI_INT, (status.MPI_SOURCE + 2)%processQuantity, \
			 0, MPI_COMM_WORLD);
		printf("send message to %d\n", (status.MPI_SOURCE + 2)%processQuantity);
	}
}
