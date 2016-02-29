#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#define TAG_MATRIZ 0
#define TAG_VECTOR 0
#define TAG_RESULT 0

main(int argc, char* argv[]) {
   int num_proc;
   MPI_Comm intercom;
   int hijos = atoi(argv[1]);
   int matriz[hijos][hijos], matriz2[hijos][hijos];
   int result_vector[hijos];
   int i,j;
   int num_esclavos = hijos;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

   // Comprobación número de procesos
   if (num_proc != 1) {
      printf("ERROR: número de procesos incorrecto\n");
      MPI_Finalize();
      exit(0);
   }

   //Generación matriz y vector
   for (i=0; i<hijos; i++) {
      for (j=0; j<hijos; j++) {
         matriz[i][j]=i+j;
	 matriz2[i][j]=10+i+j;
      }
   }
   printf("Matriz:\n");
   for (i=0; i<hijos; i++) {
      printf("  ");
      for (j=0; j<hijos; j++)
         printf("%3d",matriz[i][j]);
      printf("\n");
   }
   printf("Matriz2:\n");
   for (i=0; i<hijos; i++) {
      printf("  ");
      for (j=0; j<hijos; j++)
         printf("%3d",matriz2[i][j]);
      printf("\n");
   }

   // Creación de los procesos esclavos
   MPI_Comm_spawn("slave.out",MPI_ARGV_NULL,num_esclavos,MPI_INFO_NULL,0,MPI_COMM_WORLD,&intercom,MPI_ERRCODES_IGNORE);

   // Envío de datos
   for (i=0; i<hijos; i++) {
      MPI_Send(&matriz[i][0],hijos,MPI_INT,i,TAG_MATRIZ,intercom);
      MPI_Send(&matriz2[0][i],hijos,MPI_INT,i,TAG_VECTOR,intercom);
   }

   // Recepción de resultados
   for (i=0; i<hijos; i++)
      MPI_Recv(&result_vector[i],1,MPI_INT,MPI_ANY_SOURCE,TAG_RESULT,intercom,MPI_STATUS_IGNORE);

   printf("Resultado:\n");
   for (i=0; i<hijos; i++)
      printf("  %3d\n",result_vector[i]);

   MPI_Finalize();
}
