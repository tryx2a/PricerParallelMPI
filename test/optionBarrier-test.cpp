#include "gtest/gtest.h"
#include "../src/optionBarrierLow.h"
#include "../src/optionBarrierUp.h"
#include "../src/optionBarrier.h"
#include <iostream>

TEST (OptionBarrierLowTest, PayoffNotNull) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 5.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5);
  PnlVect* lowerBarrier = pnl_vect_create_from_list(size_, 2.0, 2.0, 2.0);

  OptionBarrierLow ob = OptionBarrierLow(T_,timeSteps_,size_,strike_,coeffPayoff, lowerBarrier);
  double payoff = ob.payoff(path);
  
  ASSERT_EQ(payoff,10);

  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  pnl_vect_free(&lowerBarrier);
}

TEST (OptionBarrierLowTest, PayoffNull) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 5.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);
  PnlVect* lowerBarrier = pnl_vect_create_from_list(size_, 2.0, 2.0, 2.0);

  OptionBarrierLow ob = OptionBarrierLow(T_,timeSteps_,size_,strike_,coeffPayoff, lowerBarrier);
  double payoff = ob.payoff(path);
  
  ASSERT_EQ(payoff,0);

  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  pnl_vect_free(&lowerBarrier);
}

TEST (OptionBarrierUpTest, PayoffNull) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 5.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5);
  PnlVect* upperBarrier = pnl_vect_create_from_list(size_, 2.0, 2.0, 2.0);

  OptionBarrierUp ob = OptionBarrierUp(T_,timeSteps_,size_,strike_,coeffPayoff, upperBarrier);
  double payoff = ob.payoff(path);
  
  ASSERT_EQ(payoff,0);

  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  pnl_vect_free(&upperBarrier);
}

TEST (OptionBarrierUpTest, PayoffNotNull) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 5.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);
  PnlVect* upperBarrier = pnl_vect_create_from_list(size_, 2.0, 2.0, 2.0);

  OptionBarrierUp ob = OptionBarrierUp(T_,timeSteps_,size_,strike_,coeffPayoff, upperBarrier);
  double payoff = ob.payoff(path);
  
  ASSERT_EQ(payoff,1);

  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  pnl_vect_free(&upperBarrier);
}

TEST (OptionBarrierTest, PayoffNotNull) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 5.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);
  PnlVect* upperBarrier = pnl_vect_create_from_list(size_, 2.0, 2.0, 2.0);
  PnlVect* lowerBarrier = pnl_vect_create_from_list(size_, 0.0, 0.0, 0.0);

  OptionBarrier ob = OptionBarrier(T_,timeSteps_,size_,strike_,coeffPayoff, lowerBarrier, upperBarrier);
  double payoff = ob.payoff(path);
  
  ASSERT_EQ(payoff,1);

  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  pnl_vect_free(&lowerBarrier);
  pnl_vect_free(&upperBarrier);
}

TEST (OptionBarrierTest, PayoffNull) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 5.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5,2.5);
  PnlVect* lowerBarrier = pnl_vect_create_from_list(size_, 2.0, 2.0, 2.0);
  PnlVect* upperBarrier = pnl_vect_create_from_list(size_, 0.0, 0.0, 0.0);

  OptionBarrier ob = OptionBarrier(T_,timeSteps_,size_,strike_,coeffPayoff, lowerBarrier, upperBarrier);
  double payoff = ob.payoff(path);
  
  ASSERT_EQ(payoff,0);

  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  pnl_vect_free(&lowerBarrier);
  pnl_vect_free(&upperBarrier);
}
