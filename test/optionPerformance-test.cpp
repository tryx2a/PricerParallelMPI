#include "gtest/gtest.h"
#include "../src/optionPerformance.h"
#include <iostream>

TEST (OptionPerformance, PayoffNotNull) {
  double T_ = 3.0;
  int timeSteps_ = 1;
  int size_ = 3;
  
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 1.0, 2.0, 3.0);
  PnlMat* path =  pnl_mat_create_from_list (T_, size_, 1.0,1.5,2.0,0.5,1.0,0.5,2.0,1.5,0.0);
  
  OptionPerformance oP = OptionPerformance(T_,timeSteps_,size_,coeffPayoff);
  double payoff = oP.payoff(path);
  
  ASSERT_EQ(payoff,1.1);
    
  pnl_mat_free(&path);
}

TEST (OptionPerformance2, PayoffNotNull) {
  double T_ = 3.0;
  int timeSteps_ = 1;
  int size_ = 3;
  
  PnlVect* coeffPayoff = pnl_vect_create_from_list (size_, 1.0, 1.0, 1.0);
  PnlMat* path =  pnl_mat_create_from_list (T_, size_, 1.0,1.0,1.0,100.0,100.0,100.0,200.0,200.0,200.0);
  
  OptionPerformance oP = OptionPerformance(T_,timeSteps_,size_,coeffPayoff);
  double payoff = oP.payoff(path);

  ASSERT_NEAR(payoff, 1.005, 0.01);
  
  pnl_mat_free(&path);
}
