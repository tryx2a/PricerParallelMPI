#include "utils.h"
#include <assert.h> 
#include <iostream>
#include <mpi.h>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_mpi.h"

namespace utils {

  double computePayoff(const PnlMat *path, const PnlVect *payoffCoeff_, int TimeSteps_, double strike){
    double res = 0.0;

    int i;
    PnlVect * assetAtMaturity = pnl_vect_new();

    //On extrait la ligne 
    pnl_mat_get_row(assetAtMaturity, path, TimeSteps_); 
    
    
    assert(assetAtMaturity->size == payoffCoeff_->size);
   

    for(int i = 0; i<assetAtMaturity->size; i++){
      res += GET(assetAtMaturity,i)*GET(payoffCoeff_,i);
    }

    pnl_vect_free(&assetAtMaturity);

    res -= strike;

    if(res<=0.0){
      return 0.0;
    }

    return res;
  }


  BS* bs_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm){

    PnlVect* spot = pnl_vect_new ();
    PnlVect* sigma = pnl_vect_new ();
    double rho ;
    double r ;
    int sizeBS;
    PnlVect* trend = pnl_vect_new ();

    pnl_object_mpi_unpack (PNL_OBJECT(spot), *buf,*bufsize,pos,comm);
    pnl_object_mpi_unpack (PNL_OBJECT(sigma), *buf,*bufsize,pos,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(rho),1,MPI_DOUBLE,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(r),1,MPI_DOUBLE,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(sizeBS),1,MPI_INT,comm);
    pnl_object_mpi_unpack (PNL_OBJECT(trend), *buf,*bufsize,pos,comm);
  
    BS* bs = new BS(spot,sigma,rho,r,sizeBS,trend);

    return bs;
  }

  Option* opt_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm){

    double T;
    int Timesteps;
    int sizeOp;
    int id;

    MPI_Unpack(*buf,*bufsize,pos,&(T),1,MPI_DOUBLE,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(Timesteps),1,MPI_INT,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(sizeOp),1,MPI_INT,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(id),1,MPI_INT,comm);
    
        if(id == 1){ //Asian
          double strike;
          MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
          Option* op = new OptionAsian(T,Timesteps,sizeOp,strike);
        }else if(id == 2){// Barrier
          double strike;
          PnlVect* payoffCoeff = pnl_vect_new();
          PnlVect* upperB = pnl_vect_new();
          PnlVect* lowerB = pnl_vect_new();

          MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(upperB), *buf,*bufsize,pos,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(lowerB), *buf,*bufsize,pos,comm);

          Option* op = new OptionBarrier(T,Timesteps,sizeOp,strike,payoffCoeff,lowerB, upperB);
        }else if(id == 3){// Barrier_Low
          double strike;
          PnlVect* payoffCoeff = pnl_vect_new();
          PnlVect* lowerB = pnl_vect_new();

          MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(lowerB), *buf,*bufsize,pos,comm);

          Option* op = new OptionBarrierLow(T,Timesteps,sizeOp,strike,payoffCoeff,lowerB);
        }else if(id == 4){// Barrier_Up
          double strike;
          PnlVect* payoffCoeff = pnl_vect_new();
          PnlVect* upperB = pnl_vect_new();

          MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(upperB), *buf,*bufsize,pos,comm);

          Option* op = new OptionBarrierUp(T,Timesteps,sizeOp,strike,payoffCoeff,upperB);
        }else if(id == 5){// Basket
          double strike;
          PnlVect* payoffCoeff = pnl_vect_new();

          MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
          pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);

          Option* op = new OptionBasket(T,Timesteps,sizeOp,strike,payoffCoeff);
        
        }else if(id == 6){// Performance
          PnlVect* payoffCoeff = pnl_vect_new();
          pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);

          Option* op = new OptionPerformance(T,Timesteps,sizeOp,payoffCoeff);

        }else{
          std::cout<<"Option unknown ."<<std::endl;
          return NULL;
        }
  }

  MonteCarlo* mc_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm,BS* bs, Option* op, int rank){
    double h;
    int H;
    int samples = 50000 ;

    MPI_Unpack(*buf,*bufsize,pos,&H,1,MPI_INT,comm);
    MPI_Unpack(*buf,*bufsize,pos,&h,1,MPI_DOUBLE,comm);


    MonteCarlo* mc = new MonteCarlo( bs, op, h, H, samples, rank);

    return mc;

  }

} // utils
