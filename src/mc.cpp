#include "mc.h"
#include "bs.h"
#include <cstring>
#include <math.h>
#include <mpi.h>

#include "pnl/pnl_random.h"
#include "pnl/pnl_mpi.h"

using namespace std;


MonteCarlo::MonteCarlo(Param* P, int rank)
{
  int option_size;
  PnlVect *spot;
  PnlVect *sigma;
  PnlVect *trend;
  double rho, r;

  P->extract("option size", option_size);
  P->extract("spot", spot, option_size);
  P->extract("volatility", sigma, option_size);
  P->extract("interest rate", r);
  P->extract("correlation", rho);
  P->extract("trend",trend,option_size);
  P->extract("hedging dates number", this->H_);
  P->extract("fd step", this->h_);

  this->mod_ = new BS(spot, sigma, rho, r, option_size, trend);
  this->opt_ = MonteCarlo::createOption(P);
  

  P->extract("sample number", this->samples_);

  this->payOffOption = 0;
  this->mean_payOffOptionSquare = 0;
  this->cumulative_samples = 0;

  int search_seed = 4192;
  int myseed = 0;
  this->rng = pnl_rng_dcmt_create_id(rank, search_seed);
  pnl_rng_sseed (this->rng, myseed);
  
}

MonteCarlo::MonteCarlo(BS* bs, Option* op, double h, int H , int samples, int rank)
{
  this->mod_ = bs;
  this->opt_ = op;

  this->h_= h;
  this->H_ = H;
  this->samples_ = samples;

  this->payOffOption = 0;
  this->mean_payOffOptionSquare = 0;
  this->cumulative_samples = 0;

  int search_seed = 4192;
  int myseed = 0;
  this->rng = pnl_rng_dcmt_create_id(rank, search_seed);
  pnl_rng_sseed (this->rng, myseed);
  
}


MonteCarlo::~MonteCarlo(){
  delete (this->mod_)->spot_;
  delete (this->mod_)->sigma_;
  delete this->mod_;
  pnl_rng_free(&(this->rng));
  delete this->opt_;
}

/**
 * Cette méthode crée la bonne instance d'option
 */
Option* MonteCarlo::createOption(Param *P){
  
  double maturity, strike;
  PnlVect *payoffCoeff, *lowerBarrier, *upperBarrier;
  int time_steps,option_size;

  char* key;
  P->extract("option type", key);
  P->extract("maturity", maturity);
  P->extract("timestep number", time_steps);
  P->extract("option size", option_size);
  
  if(strcmp(key,"basket")==0){
      P->extract("strike", strike);
      P->extract("payoff coefficients", payoffCoeff, option_size);
      Option* op = new OptionBasket(maturity, time_steps, option_size, strike, payoffCoeff);
      return op;
  }
      
  else if(strcmp(key,"asian")==0){
    P->extract("strike", strike);
    Option* op = new OptionAsian(maturity, time_steps, option_size, strike);
    return op;
  }  
  
  else if(strcmp(key,"barrier_l")==0){
    P->extract("strike", strike);
    P->extract("payoff coefficients", payoffCoeff, option_size);
    P->extract("lower barrier", lowerBarrier, option_size);
    Option* op = new OptionBarrierLow(maturity, time_steps, option_size, strike, payoffCoeff,lowerBarrier);
    return op;
  }

  else if(strcmp(key,"barrier_u")==0){
    P->extract("strike", strike);
    P->extract("payoff coefficients", payoffCoeff, option_size);
    P->extract("upper barrier", upperBarrier, option_size);
    Option* op = new OptionBarrierUp(maturity, time_steps, option_size, strike, payoffCoeff,upperBarrier);
    return op;
  }

  else if(strcmp(key,"barrier")==0){
    P->extract("strike", strike);
    P->extract("payoff coefficients", payoffCoeff, option_size);
    P->extract("lower barrier", lowerBarrier, option_size);
    P->extract("upper barrier", upperBarrier, option_size);
    Option* op = new OptionBarrier(maturity, time_steps, option_size, strike, payoffCoeff,lowerBarrier,upperBarrier);
    return op;
  }
  
  else if(strcmp(key,"performance")==0){
    P->extract("payoff coefficients", payoffCoeff, option_size);
    Option* op = new OptionPerformance(maturity, time_steps, option_size, payoffCoeff);
    return op;
  }

  else{
    return NULL;
  }
}

Option* MonteCarlo::createOptionByID(int id){


  switch(id)
  {

    Option *opt;

    case 1: //Asian 
      opt = new OptionAsian();
    break;
    case 2: // Barrier
      opt = new OptionBarrier();
    break;
    case 3: // Barrier_Low
      opt = new OptionBarrierLow();
    break;
    case 4: // Barrier_Up
      opt = new OptionBarrierUp();
    break;
    case 5: // Basket
      opt = new OptionBasket();
    break;
    case 6: // Performance
      opt = new OptionPerformance();
    break;
    default:
      opt = NULL;
      cout<<"Option unknown ."<<endl;
    break;

  return opt;
  }

}


/**
 * Calcul le prix de l'option en t=0 et la largeur de son intervalle de confinace
 */
void MonteCarlo::price(double &prix, double &ic){
  double coeffActu = exp(- (mod_->r_ * opt_->T_) );
  //Matrix of assets

  //Initialize with spot
  PnlMat* path;
  path= pnl_mat_create(opt_->TimeSteps_+1,(this->mod_)->size_);

  //Calcul du payOff   
  double tmp_payOff = 0;
  double tmp_mean_payOffSquare = 0; 
  double tmp;

  for(int m=1; m<=this->samples_; m++){
    mod_->asset(path, opt_->T_, opt_->TimeSteps_, this->rng);
    tmp = opt_->payoff(path);
    payOffOption += tmp;
    mean_payOffOptionSquare += tmp*tmp;
  }

  this->cumulative_samples += samples_;
  
  tmp_payOff  = payOffOption/this->cumulative_samples;
  tmp_mean_payOffSquare = mean_payOffOptionSquare/this->cumulative_samples;

  //Calcul du prix de l'option en t=0
  prix = coeffActu * tmp_payOff;

  //Free path
  pnl_mat_free(&path);

  //Calcul de la largeur de l'intervalle de confinace
  double cst = exp(- 2 * (mod_->r_ * opt_->T_));
  
  double varEstimator = cst * (tmp_mean_payOffSquare - (tmp_payOff*tmp_payOff));
  
  
  ic = 2 * 1.96*sqrt(varEstimator)/sqrt(this->cumulative_samples);

}


/**
 * Calcul le prix de l'option en t>0 
 */
void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &ic){
  
  double coeffActu = exp(- (mod_->r_ * (opt_->T_ - t)) );
  
  //Matrix of assets
  PnlMat* path;
  path= pnl_mat_create(opt_->TimeSteps_+1,(this->mod_)->size_);
  //Calcul du payOff   
  double payOffOption=0;
  double mean_payOffSquare=0;
  double tmp;
  
  for(int m=1; m<=this->samples_; m++){
    mod_->asset(path, t, opt_->TimeSteps_, opt_->T_, this->rng, past);
    tmp = opt_->payoff(path);
    payOffOption += tmp;
    mean_payOffSquare += tmp*tmp;
  }
  pnl_mat_print(path);
  payOffOption  = payOffOption/this->samples_;
  mean_payOffSquare = mean_payOffSquare/this->samples_;
  
  //calcul du prix de l'option en t>0
  prix = coeffActu * payOffOption;

  //Free path
  pnl_mat_free(&path);

  //Calcul de la largeur de l'intervalle de confinace
  double cst = exp(- 2 * (mod_->r_ * (opt_->T_ - t) ) );
  
  double varEstimator = cst * (mean_payOffSquare - (payOffOption*payOffOption));
  
  
  ic = (1.96*sqrt(varEstimator)/sqrt(this->samples_)) - (prix - 1.96*sqrt(varEstimator)/sqrt(this->samples_));
}


void MonteCarlo::freeRiskInvestedPart(PnlVect *V, double T, double &profitLoss){
  PnlMat *simulMarketResult, *tempMarketResult;
  simulMarketResult = pnl_mat_create(this->H_+1,this->mod_->size_);
  
  //Simulate H+1 values from 0 to T (market values)
  mod_->simul_market(simulMarketResult,T,this->H_,this->rng);

  PnlVect* precDelta, *ecartDelta, *copydelta_i;
      
  //Current Time of iteration
  double tho=0.0;
  
  /* Compute V_0 */
  //Compute Price
  double refprice,refic;
  this->price(refprice,refic);
  cout<<"price :"<<refprice<<endl;

  //Compute delta_0
  PnlVect* delta,*ic;
  delta = pnl_vect_create(this->mod_->size_);
  ic = pnl_vect_create(this->mod_->size_);
  this->delta(simulMarketResult,tho,delta,ic);
  cout<<"delta "<< tho<<endl;
  pnl_vect_print(delta);

  //On récupère S_0
  PnlVect *s;
  s = pnl_vect_create(this->mod_->size_);
  pnl_mat_get_row(s,simulMarketResult,0);

  //On calcul V_0
  LET(V,0)=refprice - pnl_vect_scalar_prod(delta,s);
  precDelta = pnl_vect_copy(delta); //on sauvergarde le delta qu'on vient de calculer


  /* Compute V_i */
  for(int i=1; i<V->size; i++){
    //On incrémente tho du pas de discrétisation de la simulation de marché à savoir T/H
    tho+=T/((double) this->H_);
    cout<<"tho : "<<tho<<endl;

    //Extract the row from 0 to tho "time"
    tempMarketResult = pnl_mat_create(i+1,this->mod_->size_);
    pnl_mat_extract_subblock(tempMarketResult, simulMarketResult, 0, i+1, 0, this->mod_->size_);

    //Compute delta_i
    this->delta(tempMarketResult, tho, delta, ic);
    cout<<"delta "<< tho<<endl;
    pnl_vect_print(delta);

    copydelta_i=pnl_vect_copy(delta);
    pnl_vect_minus_vect(copydelta_i,precDelta);
    pnl_mat_get_row(s,simulMarketResult,i);
    LET(V,i)=GET(V,i-1)*exp(mod_->r_ * T / ((double) this->H_)) - pnl_vect_scalar_prod(copydelta_i,s);
    precDelta= pnl_vect_copy(delta);

    pnl_mat_free(&tempMarketResult);
  }

  profitLoss=GET(V,V->size-1)+pnl_vect_scalar_prod(precDelta,s)-this->opt_->payoff(simulMarketResult);
  pnl_vect_free(&s);
  pnl_vect_free(&delta);
  pnl_vect_free(&ic);
  pnl_vect_free(&precDelta);

  pnl_mat_free(&simulMarketResult);
}

void MonteCarlo::setSamples(int samples){
  this->samples_ = samples;
}

void MonteCarlo::resetCumulativeSamples(){
  this->cumulative_samples = 0;
}

void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *ic){
  int nbAsset = this->opt_->size_;
  PnlMat* path_shift_up = pnl_mat_create(this->opt_->TimeSteps_+1, nbAsset);
  PnlMat* path_shift_down = pnl_mat_create(this->opt_->TimeSteps_+1, nbAsset);
  PnlMat* path = pnl_mat_create(this->opt_->TimeSteps_+1, nbAsset);
  PnlVect* sum=pnl_vect_create(nbAsset);
  PnlVect* sumSquare=pnl_vect_create(nbAsset);
  PnlVect* varEstimator=pnl_vect_create(nbAsset);
  double tstep=this->opt_->T_/this->opt_->TimeSteps_;

  for (int j = 0; j < this->samples_; ++j){
      //Select the right asset method to call  
    if(t==0){
      this->mod_->asset(path, this->opt_->T_, this->opt_->TimeSteps_, this->rng);
    }else{
      this->mod_->asset(path, t, this->opt_->TimeSteps_, this->opt_->T_, this->rng, past);
    }

    for (int i = 0; i < nbAsset; ++i){    
      this->mod_->shift_asset(path_shift_up, path, i, this->h_, t, tstep);
      this->mod_->shift_asset(path_shift_down,path, i, -this->h_, t, tstep);
      LET(sum,i)=GET(sum,i)+this->opt_->payoff(path_shift_up) - this->opt_->payoff(path_shift_down);
      LET(sumSquare,i) = GET(sumSquare,i) + SQR(this->opt_->payoff(path_shift_up) - this->opt_->payoff(path_shift_down));//GET(sum,i)*GET(sum,i);
    }
  }

  double cst = exp(- 2 * (mod_->r_ * (opt_->T_ - t) ) );

  for (int i = 0; i < nbAsset; i++){
    if(t==0){
      LET(delta, i) = GET(sum,i) * exp(-this->mod_->r_ * (this->opt_->T_ - t)) / (2.0 * this->samples_ * MGET(path, 0, i) * this->h_);
      LET(sumSquare, i) = GET(sumSquare, i) * exp(-2 * this->mod_->r_ * (this->opt_->T_ - t)) / (4.0 * this->samples_ * MGET(path, 0, i) * this->h_ * this->h_);
    }else{
      LET(delta, i) = GET(sum,i) * exp(-this->mod_->r_ * (this->opt_->T_ - t)) / (2.0 * this->samples_ * MGET(past, past->m-1, i) * this->h_); 
      LET(sumSquare, i) = GET(sumSquare, i) * exp(-2 * this->mod_->r_ * (this->opt_->T_ - t)) / (4.0 * this->samples_ * MGET(past, past->m - 1, i) * this->h_ * this->h_); 
    }

    double varEstimator = cst * (GET(sumSquare, i) - (GET(delta, i) * GET(delta, i)));

    LET(ic, i) = (GET(delta, i) + 1.96 * sqrt(varEstimator) / sqrt(this->samples_)) - (GET(delta, i) - 1.96 * sqrt(varEstimator) / sqrt(this->samples_));
  }

  pnl_vect_free(&sumSquare);
  pnl_vect_free(&varEstimator);
  pnl_vect_free(&sum);
  pnl_mat_free(&path);
  pnl_mat_free(&path_shift_up);
  pnl_mat_free(&path_shift_down);
}

void MonteCarlo::price_master(double* PrixTotal, double* ICTotal, int sizeComWorld){
  *PrixTotal = *PrixTotal / (sizeComWorld);
  *ICTotal = *ICTotal / (sizeComWorld);
}

void MonteCarlo::delta_master(PnlVect *delta, PnlVect *vic, int sizeCommWorld){
  int size = delta->size;
  for(int i=0; i<size; i++){
    LET(delta,i) = GET(delta,i)/sizeCommWorld;
    LET(vic,i) = GET(vic,i)/sizeCommWorld;
  }
}


 