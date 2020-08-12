
#include <iostream>
#include <time.h>
#include "mpi.h"
using namespace std;

int main(int argc, char** argv)
{
	int Row_Size = atoi(argv[1]), Column_Size = atoi(argv[2]), Part_Size = 0, Last_Part = 0, Current_Rank, Proc_N, Summ_Value = 0, Counter = 0;;
	double WTime_Start = 0, WTime_End = 0, Single_Time = 0, Parallel_Time = 0;	
	
	srand(time(NULL));								
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &Current_Rank);
	MPI_Comm_size(MPI_COMM_WORLD, &Proc_N);
	if (Row_Size == 0 || Column_Size == 0)
	{
		if (Current_Rank == 0)
		{
			cout << "INCORRECT INPUT" << endl;
			MPI_Finalize();
			return 0;
		}
	}

	int** Matrice = new int* [Row_Size];					// create dynamic matrix
	int* Result_Sum = new int[Column_Size];					// result summ massive of columns matrix
	int* Matrice_1_M = new int[Column_Size * Row_Size];		// matrix in one massive for normal sending

	for (int i = 0; i < Row_Size; ++i)
	{
		Matrice[i] = new int[Column_Size];
	}

	if (Column_Size < Proc_N)
	{
		Part_Size = 1;
	}
	else
	{
		Part_Size = Column_Size / Proc_N;
		if (Column_Size % Proc_N != 0)
		{
			Last_Part = Column_Size % Proc_N;
		}
	}
	int* Matrice_2_M = new int[Part_Size * Row_Size];	// matrice in one massive for normal sending	
	if (Current_Rank == 0)
	{
		cout << "\n\t\tSumm values of columns matrix.\n\t\tTimofeev E.V. 381708-2.\n\n";
		cout << "\n\tAmount of process = " << Proc_N << "\n\tSize of matrix = " << Row_Size << "x" << Column_Size << "\n\tAverage sent package size (columns to 1 process) = " << Part_Size << "\n\tAdditional pack = " << Last_Part << "\n\n";
		for (int i = 0; i < Column_Size; i++)		// initialize massive of rows values summ
		{
			Result_Sum[i] = 0;
		}

		for (int i = 0; i < Row_Size; i++)
		{			
			for (int c = 0; c < Column_Size; c++)	// full matrice with limit (max value = quantity of rows)
			{
				Matrice[i][c] = rand() % Row_Size;
			}			
		}

		WTime_Start = MPI_Wtime();
		for (int i = 0; i < Row_Size; i++)			// summ values pf columns
		{
			for (int c = 0; c < Column_Size; c++)
			{				
				Result_Sum[c] += Matrice[i][c];		
			}
		}
		WTime_End = MPI_Wtime();
		Single_Time = WTime_End - WTime_Start;
		for (int i = 0; i < Row_Size; i++)
		{
			for (int c = 0; c < Column_Size; c++)	// show matrice
			{
				cout << Matrice[i][c] << "|";
			}
			cout << endl;
		}
		cout << endl;

		for (int i = 0; i < Column_Size; i++)
		{
			cout<< Result_Sum[i] << "|";
		}
		cout << "<- Summ of columns values" <<  endl;

		int Addr = 0;
		for (int i = 0; i < Column_Size; i++)			// transform matrice to a one massive for sending
		{
			for (int r = 0; r < Row_Size; r++)	
			{
				Matrice_1_M[Addr] = Matrice[r][i];
				Addr++;
			}
		}
		WTime_Start = MPI_Wtime();
	}
	
	MPI_Scatter(Matrice_1_M, Part_Size * Row_Size,  MPI_INT, Matrice_2_M, Part_Size * Row_Size, MPI_INT, 0, MPI_COMM_WORLD);

	if (Current_Rank < Column_Size)
	{
		cout << "\n\nFrom " << Current_Rank << " process:\n";
		Counter = 0;
		Summ_Value = 0;
		for (int i = 0; i < Part_Size * Row_Size; i++)
		{
			Counter++;
			if (Counter > Row_Size)
			{
				cout << "\nSumm = " << Summ_Value << "\n\n";
				Summ_Value = 0;
				Counter = 0;
			}
			Summ_Value += Matrice_2_M[i];
			cout << Matrice_2_M[i] << "|";
		}
		cout << "\nSumm = " << Summ_Value << "\n\n";
	}
	
	if (Current_Rank == 0)
	{
		WTime_End = MPI_Wtime();
		if (Last_Part != 0)
		{
			cout << "\n\nAdditional pack from process " << Current_Rank << " process:\n";
			int Counter = 0;
			Summ_Value = 0;
			for (int i = (Column_Size * Row_Size) - (Last_Part * Row_Size); i < Column_Size * Row_Size; i++)
			{
				Counter++;
				if (Counter > Row_Size)
				{
					cout << "\nSumm = " << Summ_Value << "\n\n";
					Summ_Value = 0;
					Counter = 0;
				}
				Summ_Value += Matrice_1_M[i];
				cout << Matrice_1_M[i] << "|";
			}
			cout << "\nSumm = " << Summ_Value << "\n\n";
		}	
		Parallel_Time = WTime_End - WTime_Start;
		cout << "\n\nTime in single: " << Single_Time << " seconds\nTime in parallel: " << Parallel_Time << " seconds\n\n";
	}
	MPI_Finalize();
}
