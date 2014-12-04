#include "optionBarrierLow.h"
#include "utils.h"
#include <assert.h> 
#include <iostream>

using namespace std;

OptionBarrierLow::OptionBarrierLow(const double T_, const int timeSteps_, const int size_, const double strike_, const PnlVect* payoffCoeff_, const PnlVect* lowerBarrier_) : Option(T_, timeSteps_, size_)
{
	this->strike_ = strike_;
	this->payoffCoeff_ = pnl_vect_copy(payoffCoeff_);
	this->lowerBarrier_ = pnl_vect_copy(lowerBarrier_);
  this->id = 3;
}

OptionBarrierLow::OptionBarrierLow()
{
  this->strike_ = 0;
  this->payoffCoeff_ = pnl_vect_new();
  this->lowerBarrier_ = pnl_vect_new();
  this->id = 3;
}

OptionBarrierLow::~OptionBarrierLow(){
  if (&(this->payoffCoeff_) != NULL)
	  pnl_vect_free(&(this->payoffCoeff_));
  if (&(this->lowerBarrier_) != NULL)
	  pnl_vect_free(&(this->lowerBarrier_));
}

double OptionBarrierLow::payoff(const PnlMat *path)
{
  assert(lowerBarrier_->size == path->n);
	for (int i = 0; i < path->m; ++i)
	{
      for (int d = 0; d < path->n; ++d)
        {
          if (MGET(path, i, d) < GET(this->lowerBarrier_, d)){
            return 0;
          }
        }
	}

  return utils::computePayoff(path, this->payoffCoeff_, this->TimeSteps_, this->strike_);
}