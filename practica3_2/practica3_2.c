#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#define NFil 4
#define NCol 4
#define ColdTemp 20.0
#define HotTemp 80.0
#define NumIter 100
#define Coeff 0.5

int getNumVec(MPI_Comm);
void getVec(int*, MPI_Comm);

MPI_Comm commCart;

void main(int argc, char* argv[]) {
	int myRank;
	int numProcs;
	int nDims=2,
		dims[2]={NFil,NCol},
		periods[2]={0,0};
	int numVec,vec[NFil];
	int contIter;
	int i;
	MPI_Status status;
	MPI_Comm comCartesiano;

	float myTemp, recvTemp, newTemp, temporal, res[NFil*NCol];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Win res_ventana;

	// TODO: comprobación de número de procesos correcto
	if (numProcs!=NFil*NCol) {
		if (myRank == 0)  printf("Tienes que usar %d  procesos \n", NFil*NCol);
	MPI_Finalize();
	exit(0);
	}

	// TODO: creación del comunicador cartesiano
	MPI_Cart_create(MPI_COMM_WORLD, nDims, dims, periods, 0, &comCartesiano);

	numVec = getNumVec(comCartesiano);
	getVec(vec, comCartesiano);

	// Información de la topología cartesiana
	printf("Soy el nodo %d y tengo %d vecinos: ",myRank,numVec);
	for  (i =0 ; i<NFil; i++)
		if (vec[i]>=0)
			printf("%d ",vec[i]);
	printf("\n");

	// Valores iniciales de temperatura
	if (myRank==0){
		myTemp = HotTemp;
		MPI_Win_create(&res[0], numProcs*sizeof(float), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &res_ventana);
	} else {
		MPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &res_ventana);
		myTemp = ColdTemp;
	}

	contIter = 0;

	do {
		contIter++;
		temporal = 0;

		// TODO: envío de información a los vecinos
		for (i=0; i<NFil; i++) {
        		if (vec[i]>=0) {
				MPI_Send(&myTemp, 1, MPI_FLOAT, vec[i], 0, comCartesiano);
				// TODO: recepción de información de los vecinos y cálculo de la nueva temperatura en newTemp
				MPI_Recv(&recvTemp, 1, MPI_FLOAT, vec[i], 0, comCartesiano, &status); 
				temporal = temporal+recvTemp;
			}
	        }
		newTemp = (1 - Coeff) * myTemp + Coeff * (temporal / numVec);
		// Actualización temperaturas
		if (myRank != 0) {
			myTemp = newTemp;
			MPI_Win_fence(0, res_ventana);
			MPI_Put(&myTemp, 1, MPI_INT, 0, myRank*sizeof(float), 1, MPI_INT, res_ventana);
			MPI_Win_fence(0, res_ventana);
		} else {
			MPI_Win_fence(0, res_ventana);
			printf("\nResultado iteracion %d:\n", contIter);
			MPI_Win_fence(0, res_ventana);
			printf("  %f",HotTemp);
			for (i=1; i<numProcs; i++){
				if(i%NFil == 0) { printf("\n"); }
				printf("  %f",res[i]);
			}
			printf("\n");
		}


		// Muestra información 
		//if (myRank == numProcs-1)
		//	if (contIter%10 == 0)
		//		printf("Iter %d - soy el nodo %d y mi temp. es %f\n",contIter, myRank,myTemp);
	} while (contIter < NumIter);

	
	MPI_Win_free(&res_ventana);
	MPI_Finalize();
	exit(0);
}

int getNumVec(MPI_Comm comCartesiano) {

	// TODO: obtención del número de vecinos de un nodo
	int nV, src, dest;
	//Comprobamos superior e inferior
	nV = 0;
	MPI_Cart_shift(comCartesiano,0,1,&src,&dest);
	if(src >= 0) {
		nV = nV + 1;
	}
	if(dest >= 0) {
		nV = nV +1;
	}
	//printf("Encima: %d, Debajo: %d \n", src, dest);
	
	//Comprobamos los de izq y derecha
	MPI_Cart_shift(comCartesiano,1,1,&src,&dest);
	if(src >= 0) {
                nV = nV + 1;
        }
        if(dest >= 0) {
                nV = nV +1;
        }
	//printf("Izquierda: %d, Derecha: %d \n", src, dest);

	return nV;
}

void getVec(int *vec, MPI_Comm comCartesiano) {
	int src, dest;
	// TODO: obtención de la lista de vecinos de un  nodo
	MPI_Cart_shift(comCartesiano,0,1,&src,&dest);
	vec[0] = src;
	vec[1] = dest;
	MPI_Cart_shift(comCartesiano,1,1,&src,&dest);
	vec[2] = src;
	vec[3] = dest;
}

