#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define MAXPROC 8    /* Número mínimo de procesos */
#define LENGTH 3    /* Longitud datos a enviar */

main(int argc, char* argv[]) {
   int i, j, numProc, miRango, rang;
   const int root = 0;     
   MPI_Datatype Column_type;
   MPI_Status status;     


   MPI_Init(&argc, &argv);               
   MPI_Comm_size(MPI_COMM_WORLD, &numProc);  
   MPI_Comm_rank(MPI_COMM_WORLD, &miRango); 

   int x[LENGTH*(numProc-1)][LENGTH];        /* Buffer envío */
   int y[LENGTH*(numProc-1)][LENGTH];        /* Buffer recepción*/
   int z[LENGTH*(numProc-1)][LENGTH];

   MPI_Type_vector(LENGTH, LENGTH, LENGTH, MPI_INT, &Column_type);

   MPI_Type_commit(&Column_type);

   if (miRango == 0) { 
      for (i=0; i<LENGTH*(numProc-1); i++) {
         for (j=0; j<LENGTH; j++) {
           x[i][j] = i+j; 
         }
      }
   }

   if (numProc>MAXPROC) {
      if (miRango == 0)  
         printf("Tienes que usar un número par de procesos (como máximo %d)\n", MAXPROC);
      MPI_Finalize();
      exit(0);
   }

   if (miRango == 0) {
      printf("El proceso %d está distribuyendo la matriz x a los %d procesos\n", miRango, numProc);
      
      for (i=0; i<LENGTH; i++) {
       printf("\n");
       for (j=0; j<LENGTH*(numProc-1); j++) {
         printf(" %d", x[j][i]);
       }
      }
      for (rang=1; rang<numProc; rang++) {
         for (i=0; i<LENGTH; i++) {
           MPI_Send(&x[LENGTH*(rang-1)][i], 1, Column_type, rang, 0, MPI_COMM_WORLD);
         }
         MPI_Recv(&z[LENGTH*(rang-1)], 1, Column_type, rang, 0, MPI_COMM_WORLD, &status);     
      }
      printf("\nEl proceso %d reunió los elemento: \n", miRango);
      for (i=0; i<LENGTH; 	i++) {
        for (j=0; j<LENGTH*(numProc-1); j++) {
          printf(" %d", z[j][i]); 
	}
      printf("\n\n");	 

      }
   } else {
      for(i=0; i<LENGTH; i++){
        MPI_Recv(&x[0][i], 1, Column_type, 0, 0, MPI_COMM_WORLD, &status);
      }
      MPI_Send(&x, 1, Column_type, 0, 0, MPI_COMM_WORLD);
      printf("\nEl proceso %d ha recibido el elemento: \n", miRango);
         for (i=0; i<LENGTH; i++) {
           for (j=0; j<LENGTH; j++) {
                printf(" %d", x[j][i]);
           }
           printf("\n\n");
         }
   }

   MPI_Finalize();
}
