#include <stdio.h>
#include <string.h>
#include "mpi.h"
#define TAG_MATRIZ 0
#define TAG_VECTOR 0
#define TAG_RESULT 0

main(int argc, char* argv[]) {

   int fila[6], vector[6];
   int result;
   int i,j;

   MPI_Comm intercom;

   MPI_Init(&argc, &argv);

   MPI_Comm_get_parent(&intercom);

   MPI_Recv(fila,6,MPI_INT,MPI_ANY_SOURCE,TAG_MATRIZ,intercom,MPI_STATUS_IGNORE);
   MPI_Recv(vector,6,MPI_INT,MPI_ANY_SOURCE,TAG_VECTOR,intercom,MPI_STATUS_IGNORE);

   //Cálculo
   result=0;
   for (i=0;i<6;i++)
      result += vector[i]*fila[i];

   MPI_Send(&result,1,MPI_INT,0,TAG_RESULT,intercom);

   MPI_Finalize();
}
