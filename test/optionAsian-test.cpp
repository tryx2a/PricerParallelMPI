#include "gtest/gtest.h"
#include "../src/optionAsian.h"
#include <iostream>

TEST (OptionAsian1, Payoff) {
  
  double strike=3.0;
  double T=10.0;
  int TimeSteps=5;
  int size=10;
  PnlMat *mat=  pnl_mat_create_from_list(6,1,3.0,3.0,3.0,3.0,3.0,3.0);
  OptionAsian O=OptionAsian(T,TimeSteps,size,strike);
  double payoff = O.payoff(mat);
  pnl_mat_free(&mat);
 
  
  ASSERT_GT(payoff,0.0);
  
}

TEST (OptionAsian2, Payoff) {
  
  double strike=3.0;
  double T=10.0;
  int TimeSteps=5;
  int size=10;
  PnlMat *mat=  pnl_mat_create_from_list(6,1,1.0,1.0,1.0,1.0,1.0,1.0);
  OptionAsian O=OptionAsian(T,TimeSteps,size,strike);
  double payoff = O.payoff(mat);
  pnl_mat_free(&mat);
  

  ASSERT_EQ(payoff,0.0);
}
