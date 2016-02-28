#include <stdio.h>
#include "mpi.h"
#define MAXPROC 8    /* Número mínimo de procesos */
#define LENGTH 3  /* Longitud datos a enviar */
#define Coeff 0.5

main(int argc, char* argv[])
{
   int mi_rango, num_procs;
   int i, j, numVec;
   float data[LENGTH*LENGTH];
   MPI_Win exp_win;

   float miDato, miDatoA, miDatoB, miDatoC, miDatoD, temporal;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

   if (mi_rango==0) {
      printf("Vamos a calcular: ");
      data[0] = 80.0;
      for (i=1;i<LENGTH*LENGTH;i++) {
            data[i] = 20.0;
        }
   
      printf("\n");
   }

   if (mi_rango==0) {
      MPI_Win_create(&data[0], LENGTH*LENGTH*sizeof(float),sizeof(float),MPI_INFO_NULL,MPI_COMM_WORLD,&exp_win);
   }
   else {
      MPI_Win_create(MPI_BOTTOM,0,1,MPI_INFO_NULL,MPI_COMM_WORLD,&exp_win);
   }

   MPI_Win_fence(0,exp_win);
   if (mi_rango!=0)
	numVec = 0;
      	MPI_Get(&miDato, 1, MPI_FLOAT, 0, mi_rango, 1, MPI_FLOAT, exp_win);
	MPI_Get(&miDatoA, 1, MPI_FLOAT, 0, mi_rango+1, 1, MPI_FLOAT, exp_win);
	if(miDatoA != 0.000000) numVec = numVec + 1;
	if(mi_rango-1 >= 0){
		MPI_Get(&miDatoB, 1, MPI_FLOAT, 0, mi_rango-1, 1, MPI_FLOAT, exp_win);
		if(miDatoB != 0.000000) numVec = numVec + 1;
	}
	if(mi_rango-LENGTH >= 0){
		MPI_Get(&miDatoC, 1, MPI_FLOAT, 0, mi_rango-LENGTH, 1, MPI_FLOAT, exp_win);
		if(miDatoC != 0.000000) numVec = numVec + 1;
	}
	MPI_Get(&miDatoD, 1, MPI_FLOAT, 0, mi_rango+LENGTH, 1, MPI_FLOAT, exp_win);
	if(miDatoD != 0.000000) numVec = numVec + 1;
	temporal = miDatoA + miDatoB + miDatoC + miDatoD;
	miDato = (1 - Coeff) * miDato + Coeff * (temporal / numVec);
	MPI_Put(&miDato, LENGTH*LENGTH*sizeof(float), MPI_FLOAT, mi_rango, 1, 1, MPI_FLOAT, exp_win); //HALP con este no se como hacer

   MPI_Win_fence(0,exp_win);

   if (mi_rango!=0) {
   	  printf("Hola, soy el proceso %d y leí el datosuperior %f, inferior %f, izq %f, der %f \n",mi_rango,miDatoA,miDatoB,miDatoC,miDatoD);
   }

   MPI_Win_free(&exp_win);
   MPI_Finalize();
}


