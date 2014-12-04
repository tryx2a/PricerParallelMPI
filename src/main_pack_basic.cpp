#include <iostream>
#include <cstdlib>
#include <math.h>

//Libraire pnl
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_basis.h"

//Lib de parallelisation
//#include <omp.h>
#include <mpi.h>

//Lib du priver
#include "optionBarrierLow.h"
#include "bs.h"
#include "mc.h"


using namespace std;


int main(int argc, char **argv){
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	if(rank == 0){

		int tag = 12;
		int maVar = 120;
		char *buf;
		int info, count, bufsize=0, pos=0;

		info=MPI_Pack_size(1,MPI_INT, MPI_COMM_WORLD,&count);
		if (info) return(info);
		bufsize += count;
		
		buf = new char[size];
		info=MPI_Pack(&(maVar),1,MPI_INT,buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;

		//Envoie
		MPI_Send(buf, bufsize, MPI_PACKED, 1, tag, MPI_COMM_WORLD);

	}else{

		int tag = 12;
		int info, bufsize,pos = 0;
		int n;
		char *buf;
		MPI_Status status;
		
		info = MPI_Probe(0, tag, MPI_COMM_WORLD, &status);
		if ( info ) return info;
		info = MPI_Get_count(&status, MPI_PACKED, &bufsize);
		if ( info ) return info;
		buf = new  char[bufsize];

		info = MPI_Recv(buf,bufsize,MPI_PACKED,0,tag,MPI_COMM_WORLD,&status);
		if (info) return info;
		/* lire buf pour reconstruire V */
		info=MPI_Unpack(buf,bufsize,&pos,&n,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;

		cout<<"Resultat recu :"<<n<<endl;

		free (buf);

	}

	MPI_Finalize ();

	return 0;

}