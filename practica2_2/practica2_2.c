#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define MAXPROC 2    /* Número mínimo de procesos */
#define LENGTH 3  /* Longitud datos a enviar */
#define NC 2 /* Numero de comunicadores*/

main(int argc, char* argv[]) {
   int i, j, numProc, miRango, rang;
   const int root = 0;     
   MPI_Datatype Column_type;
   MPI_Status status;     


   MPI_Init(&argc, &argv);               
   MPI_Comm_size(MPI_COMM_WORLD, &numProc);  
   MPI_Comm_rank(MPI_COMM_WORLD, &miRango); 

   int x[LENGTH*(numProc-1)][LENGTH];        /* Buffer envío */
   int y[1][LENGTH];                 /* Buffer recepción*/
   int z[numProc-1][LENGTH];   
   int a[NC][LENGTH];

   MPI_Type_vector(LENGTH, LENGTH, LENGTH, MPI_INT, &Column_type);

   MPI_Type_commit(&Column_type);

   if (numProc!=NC*MAXPROC) {
      if (miRango == 0)
         printf("Tienes que usar un núero de proceso igual a %d\n",NC*MAXPROC);
      MPI_Finalize();
      exit(0);
   }

   MPI_Comm rowComm;
   MPI_Comm_split(MPI_COMM_WORLD, miRango*NC/numProc, miRango, &rowComm);

   int rowRank, rowSize;
   MPI_Comm_rank(rowComm, &rowRank);
   MPI_Comm_size(rowComm, &rowSize);

   //printf("WORLD RANK/SIZE: %d/%d ROW RANK/SIZE: %d/%d\n", miRango, numProc, rowRank, rowSize);

   MPI_Group world_group;
   MPI_Comm_group(MPI_COMM_WORLD, &world_group);
   int n = 2;
   int ranks[2] = {0, 2};
   MPI_Group prime_group;
   MPI_Group_incl(world_group, 2, ranks, &prime_group);
   MPI_Comm prime_comm;
   MPI_Comm_create(MPI_COMM_WORLD, prime_group, &prime_comm);


   if (rowRank == 0) {
      int prime_rank = -1, prime_size = -1;
      MPI_Comm_rank(prime_comm, &prime_rank);
      MPI_Comm_size(prime_comm, &prime_size);   

      //printf("WORLD RANK/SIZE: %d/%d \t PRIME RANK/SIZE: %d/%d\n", miRango, numProc, prime_rank, prime_size);

   
      for (i=0; i<LENGTH*(numProc-1); i++) {
         for (j=0; j<LENGTH; j++) {
           x[i][j] = i+j; 
         }
      }

      printf("El proceso %d está distribuyendo la matriz x a los %d procesos\n",rowRank, rowSize-1);
      
      for (i=0; i<LENGTH; i++) {
       printf("\n");
       for (j=0; j<LENGTH*(rowSize-1); j++) {
          printf(" %d", x[j][i]);
       }
      }
      for (rang=1; rang<rowSize; rang++) {
        for (i=0; i<LENGTH; i++) {
           MPI_Send(&x[LENGTH*(rang-1)][i], 1, Column_type, rang, 0, rowComm);
        }
	MPI_Recv(&y[rang-1], 1, Column_type, rang, 0, rowComm, &status);     
      }
      printf("\nEl proceso %d reunió los elemento: \n",rowRank);
      for (i=0; i<LENGTH; i++) {
        y[0][i] = 0;
        for (j=0; j<rowSize-1; j++) {
	  y[0][i] = z[j][i] + y[0][i];          
	}
        printf(" %d", y[0][i]);
        printf("\n\n");	 
      }
      if(prime_rank == 0) {
        for (rang=1; rang<prime_size; rang++) {
	    MPI_Recv(&a[rang], 1, Column_type, rang, 0, rowComm, &status);
        }
        printf("\nEl proceso %d reunió los elemento: \n",rowRank);
        for (i=0; i<LENGTH; i++) {
           for (j=1; j<prime_size; j++) {
               y[0][i] = a[j][i] + y[0][i];
           }
           printf(" %d", y[0][i]);
           printf("\n\n");
        }
      } else {
        MPI_Send(&z, 1, Column_type, 0, 0, prime_comm);
      }
   } else {
      for(i=0; i<LENGTH; i++){
        y[0][i] = 0;
        MPI_Recv(&x[0][i], 1, Column_type, 0, 0, rowComm, &status);
        for(j=0; j<LENGTH; j++){
           y[0][i] = x[j][i] + y[0][i];
        }
      }
      MPI_Send(&y, 1, Column_type, 0, 0, rowComm);
   }

   MPI_Finalize();
}
