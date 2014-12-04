#include <iostream>
#include <cstdlib>
#include <math.h>

//Libraire pnl
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_mpi.h"


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
		cout<<"Je suis le maitre"<<endl;
		int tag = 12;
		int tailleVect = 5;
		PnlVect * V = pnl_vect_create_from_scalar(tailleVect, 3.14);
		PnlVect * U = pnl_vect_create_from_scalar(tailleVect - 3, 6.28);
		int maVar = 120;

		void *buf;
		int info, count, bufsize=0, pos=0;
		
		//Calcul des packsize pour chaque variable
		info = pnl_object_mpi_pack_size( PNL_OBJECT(V) , MPI_COMM_WORLD, &count );
		if (info) return info;
		bufsize += count;

		info = pnl_object_mpi_pack_size( PNL_OBJECT(U) , MPI_COMM_WORLD, &count );
		if (info) return info;
		bufsize += count;

		info=MPI_Pack_size(1,MPI_INT, MPI_COMM_WORLD,&count);
		if (info) return(info);
		bufsize += count;	

		//Instantiation du buffer
		buf = new char[bufsize];

		//Pack des variable
		info = pnl_object_mpi_pack( PNL_OBJECT(V), buf,bufsize,&pos,MPI_COMM_WORLD);
		if(info) return info;
		info = pnl_object_mpi_pack( PNL_OBJECT(U), buf,bufsize,&pos,MPI_COMM_WORLD);
		if(info) return info;
		info=MPI_Pack(&(maVar),1,MPI_INT,buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;

		//Envoie du buffer
		MPI_Send(buf, bufsize, MPI_PACKED, 1, tag, MPI_COMM_WORLD);



	}else{
		cout<<"Je suis l esclave"<<endl;

		int tag =12;
		int info, bufsize,pos = 0;
		void *buf;
		int n =5 ;
		int res;
		PnlVect *V = pnl_vect_new ();
		PnlVect *U = pnl_vect_new ();
		MPI_Status status;

		//Calcul du bufsize pour dépacker
		info = MPI_Probe(0, tag, MPI_COMM_WORLD, &status);
		if ( info ) return info;
		info = MPI_Get_count(&status, MPI_PACKED, &bufsize);
		if ( info ) return info;
		buf = new char[bufsize];
		info = MPI_Recv(buf,bufsize,MPI_PACKED,0,tag,MPI_COMM_WORLD,&status);
		if (info) return info;

		//On unpack les objets
		info = pnl_object_mpi_unpack (PNL_OBJECT(V), buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;
		info = pnl_object_mpi_unpack (PNL_OBJECT(U), buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&res,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;


		//On vérifit les variables
		cout<<"Appel de l'esclave : Ai-je bien recu le vecteur - V ?"<<endl;
		pnl_vect_print(V);
		cout<<"Appel de l'esclave : Ai-je bien recu le vecteur - U ?"<<endl;
		pnl_vect_print(U);
		cout<<"Appel de l'esclave : Ai-je bien recu le vecteur - int ?"<<endl;
		cout<<res<<endl;

	}

	MPI_Finalize ();

	return 0;

}