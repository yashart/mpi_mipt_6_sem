#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>


long factorial_part (long minNum, long n);
int recv_and_calc ();

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

	long sizeForCalc = N / childrenQuantity + 1;

	if (myRank == 0)
	{
		long globalResult = 1;
		long localResult;
		long start, end;
		for (long i = 1; i < childrenQuantity; i++)
		{
			start = (i - 1)*sizeForCalc + 1;
			end = i*sizeForCalc;
			if (i == childrenQuantity - 1)
			{
				MPI_Send(&start, 1,\
						 MPI_LONG, i, 0, MPI_COMM_WORLD);
				MPI_Send(&N, 1, \
						 MPI_LONG, i, 0, MPI_COMM_WORLD);
				break;
			}
			MPI_Send(&start, 1,\
					MPI_LONG, i, 0, MPI_COMM_WORLD);
			MPI_Send(&end, 1, \
					MPI_LONG, i, 0, MPI_COMM_WORLD);
		}

		MPI_Status status;
		for (long i = 1; i < childrenQuantity; i++)
		{
			MPI_Recv(&localResult, 1, MPI_LONG, i,
						 0, MPI_COMM_WORLD, &status);

			printf("local result: %ld\n", localResult);
			globalResult *= localResult;
		}
		printf("global result: %ld\n", globalResult);

		long closeThread[2];
		closeThread[0] = 0;
		closeThread[1] = -1;
		for(int i = 1; i < childrenQuantity; i++)
		{
			MPI_Send(&closeThread[0], 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
			MPI_Send(&closeThread[1], 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
		}

		MPI_Finalize();
		return 0;
	}

	recv_and_calc();
	MPI_Finalize();
	return 0;
}

int recv_and_calc ()
{
	long start, end, result;
	while(true)
	{
		MPI_Recv(&start, 1, MPI_LONG, 0,
				 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&end, 1, MPI_LONG, 0,
				 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if (start > end)
			break;

		result = factorial_part(start, end);

		MPI_Send(&result, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
	}

	return 0;
}

long factorial_part (long minNum, long n){
	if ((n == minNum) && (n == 0))
		return 1;
	if (n == minNum)
		return minNum;
	return n * factorial_part(minNum, n - 1);
}
