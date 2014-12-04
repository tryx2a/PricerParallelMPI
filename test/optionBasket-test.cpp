#include "gtest/gtest.h"
#include "../src/optionBarrierLow.h"
#include "../src/optionBasket.h"
#include <iostream>

TEST (OptionBasket1, Payoff) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 5.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);
   
  OptionBasket ob = OptionBasket(T_,timeSteps_,size_,strike_,coeffPayoff);
  double payoff = ob.payoff(path);
  
  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  ASSERT_GT(payoff,0.0);
}

TEST (OptionBasket2, Payoff) {
  double T_ = 4.5;
  int timeSteps_ = 2;
  int size_ = 3;
  double strike_ = 7.0;
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 2.0, 2.0, 2.0);
  PnlMat* path =  pnl_mat_create_from_list (timeSteps_+1, size_, 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);
   
  OptionBasket ob = OptionBasket(T_,timeSteps_,size_,strike_,coeffPayoff);
  double payoff = ob.payoff(path);
  
  pnl_vect_free(&coeffPayoff);
  pnl_mat_free(&path);
  ASSERT_EQ(payoff,0.0);
}
