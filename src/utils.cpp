#include "utils.h"
#include <assert.h> 
#include <iostream>
#include <mpi.h>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_mpi.h"

using namespace std;

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


  int bs_mpi_pack_size(int* bufsize, int* count, int* pos,MPI_Comm comm, BS* mod_){

    int info;

    info = pnl_object_mpi_pack_size( PNL_OBJECT(mod_->spot_), comm, count );
    if (info) return info;
    *bufsize += *count;
    info = pnl_object_mpi_pack_size( PNL_OBJECT(mod_->sigma_) , comm, count );
    if (info) return info;
    *bufsize += *count;
    info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
    if (info) return(info);
    *bufsize += *count;
    info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
    if (info) return(info);
    *bufsize += *count;
    info=MPI_Pack_size(1,MPI_INT, comm,count);
    if (info) return(info);
    *bufsize += *count;   
    /*if(mod_->trend != NULL){  
      info = pnl_object_mpi_pack_size( PNL_OBJECT(mod_->trend) , comm, count );
      if (info) return info;
      *bufsize += *count;
    }*/

    return 0;
  }

  int bs_mpi_pack(void** buf, int* bufsize, int* count, int* pos,MPI_Comm comm, BS* mod_){

    int info;

    info = pnl_object_mpi_pack( PNL_OBJECT(mod_->spot_), *buf,*bufsize,pos,comm);
    if(info) return info;
    info = pnl_object_mpi_pack( PNL_OBJECT(mod_->sigma_), *buf,*bufsize,pos,comm);
    if(info) return info;
    info=MPI_Pack(&(mod_->rho_),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
    if (info) return info;
    info=MPI_Pack(&(mod_->r_),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
    if (info) return info;
    info=MPI_Pack(&(mod_->size_),1,MPI_INT,*buf,*bufsize,pos,comm);
    if (info) return info;
    /*if(mod_->trend != NULL){
      info = pnl_object_mpi_pack( PNL_OBJECT(mod_->trend), *buf,*bufsize,pos,comm);
      if(info) return info;
    }*/

    return 0;
  }


  int opt_mpi_pack_size(int* bufsize, int* count, int* pos,MPI_Comm comm, Option* opt_){
    int info;

    info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
    if (info) return(info);
    *bufsize += *count;
    info=MPI_Pack_size(1,MPI_INT, comm,count);
    if (info) return(info);
    *bufsize += *count; 
    info=MPI_Pack_size(1,MPI_INT, comm,count);
    if (info) return(info);
    *bufsize += *count; 
    info=MPI_Pack_size(1,MPI_INT, comm,count);
    if (info) return(info);
    *bufsize += *count;

    if(opt_->id == 1){ //Asian
        info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
        if (info) return(info);
        *bufsize += *count;

    }
    else if(opt_->id == 2){ // Barrier
        info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
        if (info) return(info);
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBarrier*>(opt_)->payoffCoeff_), comm, count );
        if (info) return info;
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBarrier*>(opt_)->lowerBarrier_), comm, count );
        if (info) return info;
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBarrier*>(opt_)->upperBarrier_), comm, count );
        if (info) return info;
        *bufsize += *count;

    }  
    else if(opt_->id == 3){ // Barrier_Low
        info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
        if (info) return(info);
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBarrierLow*>(opt_)->payoffCoeff_), comm, count );
        if (info) return info;
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBarrierLow*>(opt_)->lowerBarrier_), comm, count );
        if (info) return info;
        *bufsize += *count;

    }
    else if(opt_->id == 4){ // Barrier_Up
        info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
        if (info) return(info);
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBarrierUp*>(opt_)->payoffCoeff_), comm, count );
        if (info) return info;
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBarrierUp*>(opt_)->upperBarrier_), comm, count );
        if (info) return info;
        *bufsize += *count;

    }else if(opt_->id == 5){ // Basket
        info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
        if (info) return(info);
        *bufsize += *count;
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionBasket*>(opt_)->payoffCoeff_), comm, count );
        if (info) return info;
        *bufsize += *count;

    }else if(opt_->id == 6){ // Performance
        info = pnl_object_mpi_pack_size( PNL_OBJECT(dynamic_cast<OptionPerformance*>(opt_)->payoffCoeff_), comm, count );
        if (info) return info;
        *bufsize += *count;

    }else{
        cout<<"Option unknown ."<<endl;
        return 1;
    }

    return 0;
  }

  int opt_mpi_pack(void **buf, int* bufsize, int* count, int* pos,MPI_Comm comm, Option* opt_){
    int info;

    info=MPI_Pack(&(opt_->T_),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
    if (info) return info;
    info=MPI_Pack(&(opt_->TimeSteps_),1,MPI_INT,*buf,*bufsize,pos,comm);
    if (info) return info;
    info=MPI_Pack(&(opt_->size_),1,MPI_INT,*buf,*bufsize,pos,comm);
    if (info) return info;
    info=MPI_Pack(&(opt_->id),1,MPI_INT,*buf,*bufsize,pos,comm);
    if (info) return info;

    if(opt_->id == 1){ //Asian
        info=MPI_Pack(&( dynamic_cast<OptionAsian*>(opt_)->strike_ ),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
        if (info) return info;
    }
    else if(opt_->id == 2){
        info=MPI_Pack(&( dynamic_cast<OptionBarrier*>(opt_)->strike_ ),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
        if (info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBarrier*>(opt_)->payoffCoeff_), *buf,*bufsize,pos,comm);
        if(info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBarrier*>(opt_)->lowerBarrier_), *buf,*bufsize,pos,comm);
        if(info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBarrier*>(opt_)->upperBarrier_), *buf,*bufsize,pos,comm);
        if(info) return info;
    }
    else if(opt_->id == 3){ // Barrier_Low
        info=MPI_Pack(&( dynamic_cast<OptionBarrierLow*>(opt_)->strike_ ),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
        if (info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBarrierLow*>(opt_)->payoffCoeff_), *buf,*bufsize,pos,comm);
        if(info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBarrierLow*>(opt_)->lowerBarrier_), *buf,*bufsize,pos,comm);
        if(info) return info;
    }
    else if(opt_->id == 4){ // Barrier_Up
        info=MPI_Pack(&( dynamic_cast<OptionBarrierUp*>(opt_)->strike_ ),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
        if (info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBarrierUp*>(opt_)->payoffCoeff_), *buf,*bufsize,pos,comm);
        if(info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBarrierUp*>(opt_)->upperBarrier_), *buf,*bufsize,pos,comm);
        if(info) return info;

    }else if(opt_->id == 5){ // Basket
        info=MPI_Pack(&( dynamic_cast<OptionBasket*>(opt_)->strike_ ),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
        if (info) return info;
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionBasket*>(opt_)->payoffCoeff_), *buf,*bufsize,pos,comm);
        if(info) return info;

    }else if(opt_->id == 6){ // Performance
        info = pnl_object_mpi_pack( PNL_OBJECT( dynamic_cast<OptionPerformance*>(opt_)->payoffCoeff_), *buf,*bufsize,pos,comm);
        if(info) return info;

    }else{
        cout<<"Option unknown ."<<endl;
        return 1;
    }
    return 0;
  }



  int mc_mpi_pack_size(int* bufsize, int* count, int* pos,MPI_Comm comm){

    int info;

    info=MPI_Pack_size(1,MPI_INT, comm,count);
    if (info) return(info);
    *bufsize += *count;
    info=MPI_Pack_size(1,MPI_DOUBLE, comm,count);
    if (info) return(info);
    *bufsize += *count;

    return 0;
  }

  int mc_mpi_pack(void **buf, int* bufsize, int* count, int* pos,MPI_Comm comm, MonteCarlo *mc){
    int info;

    info=MPI_Pack(&(mc->H_),1,MPI_INT,*buf,*bufsize,pos,comm);
    if (info) return info;
    info=MPI_Pack(&(mc->h_),1,MPI_DOUBLE,*buf,*bufsize,pos,comm);
    if (info) return info;

    return 0;
  }


  BS* bs_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm){

    PnlVect* spot = pnl_vect_new ();
    PnlVect* sigma = pnl_vect_new ();
    double rho ;
    double r ;
    int sizeBS;
    //PnlVect* trend = pnl_vect_new ();

    pnl_object_mpi_unpack (PNL_OBJECT(spot), *buf,*bufsize,pos,comm);
    pnl_object_mpi_unpack (PNL_OBJECT(sigma), *buf,*bufsize,pos,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(rho),1,MPI_DOUBLE,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(r),1,MPI_DOUBLE,comm);
    MPI_Unpack(*buf,*bufsize,pos,&(sizeBS),1,MPI_INT,comm);
    //pnl_object_mpi_unpack (PNL_OBJECT(trend), *buf,*bufsize,pos,comm);
    
    BS* bs = new BS(spot,sigma,rho,r,sizeBS,NULL);

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
      return new OptionAsian(T,Timesteps,sizeOp,strike);
    }
    else if(id == 2){// Barrier
      double strike;
      PnlVect* payoffCoeff = pnl_vect_new();
      PnlVect* upperB = pnl_vect_new();
      PnlVect* lowerB = pnl_vect_new();

      MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(lowerB), *buf,*bufsize,pos,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(upperB), *buf,*bufsize,pos,comm);

      return new OptionBarrier(T,Timesteps,sizeOp,strike,payoffCoeff,lowerB, upperB);
    }
    else if(id == 3){// Barrier_Low
      double strike;
      PnlVect* payoffCoeff = pnl_vect_new();
      PnlVect* lowerB = pnl_vect_new();

      MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(lowerB), *buf,*bufsize,pos,comm);

      return new OptionBarrierLow(T,Timesteps,sizeOp,strike,payoffCoeff,lowerB);
    }
    else if(id == 4){// Barrier_Up
      double strike;
      PnlVect* payoffCoeff = pnl_vect_new();
      PnlVect* upperB = pnl_vect_new();

      MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(upperB), *buf,*bufsize,pos,comm);

      return new OptionBarrierUp(T,Timesteps,sizeOp,strike,payoffCoeff,upperB);
    }
    else if(id == 5){// Basket
      double strike;
      PnlVect* payoffCoeff = pnl_vect_new();

      MPI_Unpack(*buf,*bufsize,pos,&(strike),1,MPI_DOUBLE,comm);
      pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);

      return new OptionBasket(T,Timesteps,sizeOp,strike,payoffCoeff);

    }else if(id == 6){// Performance
      PnlVect* payoffCoeff = pnl_vect_new();
      pnl_object_mpi_unpack (PNL_OBJECT(payoffCoeff), *buf,*bufsize,pos,comm);

      return new OptionPerformance(T,Timesteps,sizeOp,payoffCoeff);
    }
    else{
      std::cout<<"Option unknown ."<<std::endl;
      return NULL;
    }
  }

  MonteCarlo* mc_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm,BS* bs, Option* op, int rank){
    double h;
    int H;
    //int samples = 50000 ;
    int samples = 500 ;

    MPI_Unpack(*buf,*bufsize,pos,&H,1,MPI_INT,comm);
    MPI_Unpack(*buf,*bufsize,pos,&h,1,MPI_DOUBLE,comm);

    MonteCarlo* mc = new MonteCarlo( bs, op, h, H, samples, rank);

    return mc;

  }


  void price_master(double* fluxPrixTotal, double* fluxICTotal, MPI_Comm comm){
    cout<<"Price_master Prix:"<<endl;
    cout<<*fluxPrixTotal<<endl;

    cout<<"Price_master IC:"<<endl;
    cout<<*fluxICTotal<<endl;

  }

} // utils
