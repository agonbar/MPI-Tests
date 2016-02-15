#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define MAXPROC 8    /* Número mínimo de procesos */
#define LENGTH 24    /* Longitud datos a enviar */

main(int argc, char* argv[]) {
   int i, j, numProc, miRango;
   const int root = 0;     
   MPI_Status status;     

   int x[LENGTH];        /* Buffer envío */
   int y[LENGTH];        /* Buffer recepción*/
   int z[LENGTH];

   MPI_Init(&argc, &argv);               
   MPI_Comm_size(MPI_COMM_WORLD, &numProc);  
   MPI_Comm_rank(MPI_COMM_WORLD, &miRango); 

   if (miRango == 0) { 
      for (i=0; i<LENGTH; i++) 
         x[i] = i; 
   }

   if (numProc>MAXPROC || numProc%2 != 0) {
      if (miRango == 0)  
         printf("Tienes que usar un número par de procesos (como máximo %d)\n", MAXPROC);
      MPI_Finalize();
      exit(0);
   }

   if (miRango == 0) 
      printf("El proceso %d está distribuyendo la matriz x a los %d procesos\n", miRango, numProc);

   MPI_Scatter(&x, LENGTH/numProc, MPI_INT, &y, LENGTH/numProc, MPI_INT, root, MPI_COMM_WORLD);

   printf("El proceso %d tiene los siguientes elementos: ", miRango);
   for (i=0; i<LENGTH/numProc; i++) 
      printf(" %d", y[i]); 
   printf("\n");

   /* Reúne el array x y lo almacena en y */
   MPI_Gather(&y, LENGTH/numProc, MPI_INT, &z, LENGTH/numProc, MPI_INT, root, MPI_COMM_WORLD);

   if (miRango == 0) { /* Muestra el array que ha reunido */
	printf("El proceso %d reunió los elementos:", miRango);
	for (i=0; i<LENGTH; i++) 
		printf(" %d", z[i]); 
	printf("\n\n");
   }


   MPI_Finalize();
}
