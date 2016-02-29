#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#define TAG_MATRIZ 0
#define TAG_VECTOR 0
#define TAG_RESULT 0

main(int argc, char* argv[]) {
if(argc >= 2) {
   int num_proc;
   MPI_Comm intercom, intracom;
   int hijos = atoi(argv[1]);
   int matriz[hijos][hijos], matriz2[hijos][hijos];
   int result_vector[hijos];
   int i,j;
   int num_esclavos = hijos;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
  
   MPI_Win res_ventana;

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
   MPI_Comm_spawn("a.out",MPI_ARGV_NULL,num_esclavos,MPI_INFO_NULL,0,MPI_COMM_WORLD,&intercom,MPI_ERRCODES_IGNORE);
   
   MPI_Intercomm_merge(intercom, 0, &intracom);

   MPI_Win_create(&result_vector[0], hijos*sizeof(int), 1, MPI_INFO_NULL, intracom, &res_ventana);
   
   // Envío de datos
   for (i=0; i<hijos; i++) {
      MPI_Send(&matriz[i][0],hijos,MPI_INT,i,TAG_MATRIZ,intercom);
      MPI_Send(&matriz2[0][i],hijos,MPI_INT,i,TAG_VECTOR,intercom);
   }

   // Recepción de resultados
   //for (i=0; i<hijos; i++)
   //   MPI_Recv(&result_vector[i],1,MPI_INT,MPI_ANY_SOURCE,TAG_RESULT,intercom,MPI_STATUS_IGNORE);
   MPI_Win_fence(0, res_ventana);
   printf("Resultado:\n");
   MPI_Win_fence(0, res_ventana);
   for (i=0; i<hijos; i++)
      printf("  %3d\n",result_vector[i]);
   
   MPI_Win_free(&res_ventana);
   MPI_Finalize();
} else {
   int result;
   int i,j,hijos,rango;

   MPI_Comm intercom, intracom;

   MPI_Init(&argc, &argv);

   MPI_Comm_get_parent(&intercom);

   MPI_Intercomm_merge(intercom, 0, &intracom);

   MPI_Comm_size(intercom, &hijos);
   int fila[hijos], vector[hijos];

   MPI_Win res_win;
   MPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL, intracom, &res_win);
   MPI_Comm_rank(intercom, &rango);
   MPI_Recv(fila,hijos,MPI_INT,MPI_ANY_SOURCE,TAG_MATRIZ,intercom,MPI_STATUS_IGNORE);
   MPI_Recv(vector,hijos,MPI_INT,MPI_ANY_SOURCE,TAG_VECTOR,intercom,MPI_STATUS_IGNORE);

   //Cálculo
   result=0;
   for (i=0;i<hijos;i++)
      result += vector[i]*fila[i];

   //MPI_Send(&result,1,MPI_INT,0,TAG_RESULT,intercom);
   MPI_Win_fence(0, res_win);
   MPI_Put(&result, 1, MPI_INT, 0, rango*sizeof(int), 1, MPI_INT, res_win);
   MPI_Win_fence(0, res_win);
   
   MPI_Win_free(&res_win);
   MPI_Finalize();
}
}
