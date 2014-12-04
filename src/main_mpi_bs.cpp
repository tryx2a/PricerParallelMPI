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
#include "optionBasket.h"
#include "bs.h"
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

		const char * infile = "../data/basket.dat";
		Param *P = new Parser(infile);
		MonteCarlo* mc = new MonteCarlo(P, rank);

		/*int tailleVect = 5;
		PnlVect * V = pnl_vect_create_from_scalar(tailleVect, 3.14);
		int maVar = 120;*/

		/*PnlVect* spot = pnl_vect_create_from_scalar(40, 100);
		PnlVect* sigma = pnl_vect_create_from_scalar(40, 0.2);
		double rho = 0.0;
		double r =  0.04879;
		int sizeBS = 40;
		PnlVect* trend = pnl_vect_create_from_scalar(40, 0.03);

		//Option
		double T = 3;
		int Timesteps = 15;
		int sizeOp = 40;
		int id = 0;*/

		//Opt basket
		double strike = 100;
		PnlVect* payoff = pnl_vect_create_from_scalar(40,0.025);

		/*//Mc
		int H = 25;
		double h = 0.1;*/

		void *buf;
		int info, count, bufsize=0, pos=0;
		
		//BS
		mc->bs_mpi_pack_size(&bufsize, &count, &pos, MPI_COMM_WORLD );
		//opt
		mc->opt_mpi_pack_size(&bufsize, &count, &pos, MPI_COMM_WORLD );

		//Opt basket
		info=MPI_Pack_size(1,MPI_DOUBLE, MPI_COMM_WORLD,&count);
		if (info) return(info);
		bufsize += count;
		info = pnl_object_mpi_pack_size( PNL_OBJECT(payoff) , MPI_COMM_WORLD, &count );
		if (info) return info;
		bufsize += count;

		//mc
		/*info=MPI_Pack_size(1,MPI_INT, MPI_COMM_WORLD,&count);
		if (info) return(info);
		bufsize += count;
		info=MPI_Pack_size(1,MPI_DOUBLE, MPI_COMM_WORLD,&count);
		if (info) return(info);
		bufsize += count;*/
		mc->mc_mpi_pack_size(&bufsize, &count, &pos,MPI_COMM_WORLD);

		//Instantiation du buffer
		buf = new char[bufsize];

		//Pack des variable
		//BS
		mc->bs_mpi_pack(&buf, &bufsize, &count, &pos, MPI_COMM_WORLD );
		//opt
		mc->opt_mpi_pack(&buf, &bufsize, &count, &pos, MPI_COMM_WORLD);


		/*info=MPI_Pack(&(strike),1,MPI_DOUBLE,buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;
		info = pnl_object_mpi_pack( PNL_OBJECT(payoff), buf,bufsize,&pos,MPI_COMM_WORLD);
		if(info) return info;*/

		/*info=MPI_Pack(&(H),1,MPI_INT,buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Pack(&(h),1,MPI_DOUBLE,buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;*/
		mc->mc_mpi_pack(&buf, &bufsize, &count, &pos,MPI_COMM_WORLD);

		//Envoie du buffer
		MPI_Send(buf, bufsize, MPI_PACKED, 1, tag, MPI_COMM_WORLD);



	}else{
		cout<<"Je suis l esclave"<<endl;

		int tag =12;
		int info, count,bufsize,pos = 0;
		void *buf;
		
		//BS param
		/*PnlVect* spot = pnl_vect_new ();
		PnlVect* sigma = pnl_vect_new ();
		double rho ;
		double r ;
		int sizeBS;
		PnlVect* trend = pnl_vect_new ();*/

		//Opt
		/*double T;
		int Timesteps;
		int sizeOp;
		int id;*/

		//Opt basket
		/*double strike;
		PnlVect* payoff = pnl_vect_new ();*/

		//MC
		/*int H;
		double h;*/

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
		/*info = pnl_object_mpi_unpack (PNL_OBJECT(spot), buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;
		info = pnl_object_mpi_unpack (PNL_OBJECT(sigma), buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&rho,1,MPI_DOUBLE,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&r,1,MPI_DOUBLE,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&sizeBS,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;
		info = pnl_object_mpi_unpack (PNL_OBJECT(trend), buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;*/

		

		/*info=MPI_Unpack(buf,bufsize,&pos,&T,1,MPI_DOUBLE,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&Timesteps,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&sizeOp,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&id,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;

		info=MPI_Unpack(buf,bufsize,&pos,&strike,1,MPI_DOUBLE,MPI_COMM_WORLD);
		if (info) return info;
		info = pnl_object_mpi_unpack (PNL_OBJECT(payoff), buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;

		info=MPI_Unpack(buf,bufsize,&pos,&H,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Unpack(buf,bufsize,&pos,&h,1,MPI_DOUBLE,MPI_COMM_WORLD);
		if (info) return info;*/

		Option* op = utils::opt_mpi_unpack(&buf, &bufsize, &count, &pos,MPI_COMM_WORLD);

		//cout<< op-> <<endl;

		//On vérifit les variables
		/*cout<<"Appel de l'esclave"<<endl;
		pnl_vect_print(spot);
		pnl_vect_print(sigma);
		cout<<rho<<endl;
		cout<<r<<endl;
		pnl_vect_print(trend);*/

		//BS* bs = new BS(spot, sigma,rho,r,sizeBS,trend);
		//OptionBasket* optB = new OptionBasket(T,Timesteps,sizeOp,strike,payoff);

		MonteCarlo* mc = utils::mc_mpi_unpack( &buf, &bufsize, &count, &pos,MPI_COMM_WORLD, bs , op , rank) ;

		/*if(mc->mod_ == NULL){
			cout<<"Cest nul"<<endl;
		}*/

		double prix;
		double ic;

		mc->price(prix,ic);

		cout<<"Prix : "<<prix<<endl;
		cout<<"Intervalle de confiance :"<<ic<<endl;

	}

	MPI_Finalize ();

	return 0;

}