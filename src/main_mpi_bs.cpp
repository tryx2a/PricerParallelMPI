#include <iostream>
#include <cstdlib>
#include <math.h>

//Lib de parallelisation
#include <mpi.h>

//Lib du pricer
#include "mc.h"
#include "utils.h"


using namespace std;


int main(int argc, char **argv){
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	if(rank == 0){
		cout<<"Je suis le maitre"<<endl;
		int tag = 12;

		const char * infile = argv[1];
		Param *P = new Parser(infile);
		MonteCarlo* mc = new MonteCarlo(P, rank);

		void *buf;
		int info, count, bufsize=0, pos=0;
		
		utils::bs_mpi_pack_size(&bufsize, &count, &pos, MPI_COMM_WORLD, mc->mod_ );
		utils::opt_mpi_pack_size(&bufsize, &count, &pos, MPI_COMM_WORLD, mc->opt_);
		utils::mc_mpi_pack_size(&bufsize, &count, &pos,MPI_COMM_WORLD);
		
		//Instantiation du buffer
		buf = new char[bufsize];

		//Pack des variable
		utils::bs_mpi_pack(&buf, &bufsize, &count, &pos, MPI_COMM_WORLD, mc->mod_);
		utils::opt_mpi_pack(&buf, &bufsize, &count, &pos, MPI_COMM_WORLD, mc->opt_);
		utils::mc_mpi_pack(&buf, &bufsize, &count, &pos, MPI_COMM_WORLD, mc);

		//Envoie du buffer
		MPI_Send(buf, bufsize, MPI_PACKED, 1, tag, MPI_COMM_WORLD);

	}else{
		cout<<"Je suis l esclave"<<endl;

		int tag =12;
		int info, count,bufsize,pos = 0;
		void *buf;

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
		BS* bs = utils::bs_mpi_unpack(&buf, &bufsize, &count, &pos,MPI_COMM_WORLD);
		Option* op = utils::opt_mpi_unpack(&buf, &bufsize, &count, &pos,MPI_COMM_WORLD);
		MonteCarlo* mc = utils::mc_mpi_unpack( &buf, &bufsize, &count, &pos,MPI_COMM_WORLD, bs , op , rank) ;

		double prix;
		double ic;


		mc->price(prix,ic);
		cout<<"Prix : "<<prix<<endl;
		cout<<"Intervalle de confiance :"<<ic<<endl;


		cout<<"Creation du delta"<<endl;
		PnlVect* delta = pnl_vect_create(op->size_);
		//pnl_vect_print(delta);
		mc->delta(NULL, 0, delta, NULL);
		pnl_vect_print(delta);

	}

	MPI_Finalize ();

	return 0;

}