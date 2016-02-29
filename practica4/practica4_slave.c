#include <stdio.h>
#include <string.h>
#include "mpi.h"
#define TAG_MATRIZ 0
#define TAG_VECTOR 0
#define TAG_RESULT 0

main(int argc, char* argv[]) {
   
   int result;
   int i,j,hijos;

   MPI_Comm intercom;

   MPI_Init(&argc, &argv);

   MPI_Comm_get_parent(&intercom);
   
   MPI_Comm_size(intercom, &hijos);
   int fila[hijos], vector[hijos];

   MPI_Recv(fila,hijos,MPI_INT,MPI_ANY_SOURCE,TAG_MATRIZ,intercom,MPI_STATUS_IGNORE);
   MPI_Recv(vector,hijos,MPI_INT,MPI_ANY_SOURCE,TAG_VECTOR,intercom,MPI_STATUS_IGNORE);

   //Cálculo
   result=0;
   for (i=0;i<hijos;i++)
      result += vector[i]*fila[i];

   MPI_Send(&result,1,MPI_INT,0,TAG_RESULT,intercom);

   MPI_Finalize();
}
