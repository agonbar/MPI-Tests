#include <stdio.h>
#include <string.h>
#include "mpi.h"
main(int argc, char* argv[])
{
 int my_rank;
 int p;
 int source; int dest;
 int tag=0;
 char message[100];
 MPI_Status status;
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 MPI_Comm_size(MPI_COMM_WORLD, &p);
 if (my_rank != p-1) {
         sprintf(message,"Saludos del proceso %d",my_rank);
         dest = (my_rank+1)%p;
         MPI_Send(message,strlen(message)+1,MPI_CHAR,dest,tag,MPI_COMM_WORLD);
         //Re-envía lo que recibe
         for(source=1;source<p;source++) {
                MPI_Recv(message,100,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
                MPI_Send(message,strlen(message)+1,MPI_CHAR,dest,tag,MPI_COMM_WORLD);
         }
 }
 else {
         printf("Hola, soy el proceso %d (hay %d procesos) y recibo:\n",my_rank,p);
         for(source=1;source<p;source++) {
                 MPI_Recv(message,100,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
                 printf("%s\n",message);
         }
 }
 MPI_Finalize();
}
