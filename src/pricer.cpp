#include <iostream>
#include <cstdlib>
#include <math.h>

//Lib de parallelisation
#include <mpi.h>

//Lib du pricer
#include "mc.h"
#include "utils.h"
#include "pnl/pnl_mpi.h"


using namespace std;


int main(int argc, char **argv){

    //Calcul du prix de l'option
	int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    double prix = 0;
    double ic = 0;
    PnlVect *delta;
    PnlVect *vic;
    PnlVect *deltaGlobal;
    PnlVect *vicGlobal;
    double precision;
    bool optionPrecision;

    double begin = MPI_Wtime();

    if(argv[2] != NULL){
      precision = atof(argv[2]);
      optionPrecision = true;
    }else{
      precision = 1000;
      optionPrecision = false;
    }
  
    double prixGlobal = 0.0;
    double icGlobal = 0.0;
    int nbTirages = 0;

    MonteCarlo* mc;

    if(rank == 0){ //Master
    
      int tag = 12;

      const char * infile = argv[1];
      Param *P = new Parser(infile);
      mc = new MonteCarlo(P, rank);

      void *buf;
      int info, count, bufsize=0, pos=0;
      delta = pnl_vect_create(mc->mod_->size_);
      deltaGlobal = pnl_vect_create(mc->mod_->size_);
      vic = pnl_vect_create(mc->mod_->size_);
      vicGlobal = pnl_vect_create(mc->mod_->size_);

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

      int subSamples = utils::computeSubSample(mc->samples_,size);
      mc->setSamples(subSamples + (mc->samples_ % size) );

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
      mc = utils::mc_mpi_unpack( &buf, &bufsize, &count, &pos,MPI_COMM_WORLD, bs , op , rank , size) ;


      delta = pnl_vect_create(mc->mod_->size_);
      deltaGlobal = pnl_vect_create(mc->mod_->size_);
      vic = pnl_vect_create(mc->mod_->size_);
      vicGlobal = pnl_vect_create(mc->mod_->size_);

    }


  	do{ //Tant qu'on a pas la précision recquise, on continue les tours de boucle

    	//exécution de la méthode price par tous les processus
    	mc->price(prix,ic); 
    
    	//Réduction des résultats intermédiaires dans la varialble prixGlobal
    	MPI_Reduce(&prix, &prixGlobal, 1, MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);
    	MPI_Reduce(&ic, &icGlobal, 1, MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);
    	MPI_Reduce(&(mc->cumulative_samples), &nbTirages, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  	}while(optionPrecision && ic > precision); 

  	//exécution de la méthode delta par tous les processus puis réduction des résultats
  	mc->delta(NULL,0,delta,vic);
  	int info;
  	info = pnl_object_mpi_reduce (PNL_OBJECT(delta), PNL_OBJECT(deltaGlobal), MPI_SUM, 0, MPI_COMM_WORLD);
  	info = pnl_object_mpi_reduce (PNL_OBJECT(vic), PNL_OBJECT(vicGlobal), MPI_SUM, 0, MPI_COMM_WORLD);

  	if(rank == 0){
      cout<<"Nombre de tirages : "<<nbTirages<<endl;

      mc->price_master(&prixGlobal,&icGlobal,size);
      cout<<"Prix en 0 : "<<prixGlobal<<endl;
      cout<<"IC Prix : "<<icGlobal<<endl;

      double end = MPI_Wtime();
      cout<<"Temps de calcul du prix : "<<end-begin<<endl;

      mc->delta_master(deltaGlobal,vicGlobal,size);
      cout<<"Delta en 0 : "<<endl;
      pnl_vect_print(deltaGlobal);
      cout<<"IC Delta : "<<endl;
      pnl_vect_print(vicGlobal);
    }

    
    pnl_vect_free(&delta);
    pnl_vect_free(&deltaGlobal);
    pnl_vect_free(&vic);
    pnl_vect_free(&vicGlobal);

    MPI_Finalize ();
    

	return 0;

}