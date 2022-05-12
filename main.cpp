#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "mpi.h"
#include <string>
#include <stdint.h>

static void PrintAnswer(uint32_t* arr, uint32_t N) {
	for (int i = 0; i < N; i++) {
		printf("%08jx ", (uintmax_t)arr[N - i - 1]);
	}
	printf("\n");
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int ProcNum, ProcRank;
	MPI_Status Status;
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	uint32_t** Nums;
	uint32_t RealSize = 2;
	uint32_t NumsSize;
	NumsSize = 8; //2, 4, 8, 16 и т.д. ≈сли необходимо другое значение, то брать большее и остальное заполн€ть 1. => 2,3,5,9... procNum 

	MPI_Datatype LongInt;
	uint32_t LongIntSize = RealSize * NumsSize; 
	MPI_Type_contiguous(LongIntSize, MPI_UINT32_T, &LongInt);
	MPI_Type_commit(&LongInt);
	
	if (ProcRank == 0) {
		
		Nums = new uint32_t * [NumsSize]
		{
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				new uint32_t[LongIntSize]{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		};
	}
	else {
		Nums = new uint32_t * [1]{ new uint32_t[0] };
	}

	uint32_t Step;
	uint32_t MaxStep;
	uint32_t MaxNums;
	Step = 1;
	MaxNums = ProcNum - 1;
	MaxStep = log2(2 * MaxNums);

	uint32_t* num1 = new uint32_t[LongIntSize];
	uint32_t* num2 = new uint32_t[LongIntSize];

	while (Step <= MaxStep) {

		if (ProcRank == 0) {
			for (int i = 0; i < MaxNums / Step; i++) {
				uint32_t* sendNum = Nums[2 * i];
				MPI_Send(sendNum, 1, LongInt, i + 1, 1, MPI_COMM_WORLD);
				sendNum = Nums[2 * i + 1];
				MPI_Send(sendNum, 1, LongInt, i + 1, 2, MPI_COMM_WORLD);
			}
		}
		if (ProcRank < MaxNums / Step + 1 && ProcRank != 0)
		{
			MPI_Recv(num1, 1, LongInt, 0, 1, MPI_COMM_WORLD, &Status);
			MPI_Recv(num2, 1, LongInt, 0, 2, MPI_COMM_WORLD, &Status);

			uint32_t* multiply = new uint32_t[LongIntSize]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			for (int i = 0; i < LongIntSize; i++) {
				for (int j = 0; j < LongIntSize; j++) {
					uint64_t mltpl = (uint64_t)num1[i] * (uint64_t)num2[j];
					uint32_t n1 = (uint32_t)mltpl;
					uint32_t n2 = (uint32_t)(mltpl >> 32);
					if (i + j < LongIntSize) {
						multiply[i + j] += n1;
					}
					if (i + j + 1 < LongIntSize) {
						multiply[i + j + 1] += n2;
					}
				}
			}

			MPI_Send(multiply, 1, LongInt, 0, 3, MPI_COMM_WORLD);
		}
		if (ProcRank == 0) {
			for (int i = 0; i < MaxNums / Step; i++) {
				uint32_t* recvNum = new uint32_t[LongIntSize];
				MPI_Recv(recvNum, 1, LongInt, i + 1, 3, MPI_COMM_WORLD, &Status);
				Nums[i] = recvNum;
			}
		}
		Step++;
	}

	if (ProcRank == 0) {
		PrintAnswer(Nums[0], LongIntSize);
	}
	int MPI_Type_free(LongInt);
	MPI_Finalize();
	return 0;
}