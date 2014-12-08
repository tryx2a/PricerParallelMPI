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

	double prix = 0;
	double ic = 0;

	double begin = MPI_Wtime();
	
	if(rank == 0){ //Master
		
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
		for(int i = 1 ; i <= size ; i++){
			int voisin =( rank + i )% size;
			if(voisin != 0){
				MPI_Send(buf, bufsize, MPI_PACKED, i, tag, MPI_COMM_WORLD);
			}
		}

		mc->price(prix, ic);

	}

	else { //Slaves
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

		


		mc->price(prix,ic);

	}

	
	double prixGlobal = 0.0;
	MPI_Reduce(&prix, &prixGlobal, 1, MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);

	double icGlobal = 0.0;
	MPI_Reduce(&ic, &icGlobal, 1, MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0){		
		utils::price_master(&prixGlobal,&icGlobal,size);

		cout<<"Prix final : "<<prixGlobal<<endl;
		cout<<"IC final : "<<icGlobal<<endl;

		double end = MPI_Wtime();
		cout<<"Temps : "<<end-begin<<endl;
	}

	


	MPI_Finalize ();

	return 0;

}