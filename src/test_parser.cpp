#include <iostream>
#include "parser.h"

using namespace std;

int main(int argc, char **argv)
{
  double T, r, strike;
  PnlVect *spot, *sigma, *divid;
  int size;

  char *infile = argv[1];
  Param *P = new Parser(infile);

  P->extract("maturity", T);
  P->extract("option size", size);
  P->extract("spot", spot, size);
  P->extract("volatility", sigma, size);
  P->extract("interest rate", r);
  P->extract("dividend rate", divid, size);
  if (divid == NULL)
    {
      divid = pnl_vect_create_from_zero(size);
    }

  P->extract("strike", strike);

  P->print();
  cout << "maturity " << T << endl;
  cout << "strike " << strike << endl;
  cout << "option size " << size << endl;
  cout << "interest rate " << r << endl;
  cout << "dividend rate "; pnl_vect_print_asrow(divid);
  cout << "spot "; pnl_vect_print_asrow(spot);
  cout << "volatility "; pnl_vect_print_asrow(sigma);


  exit(0);
}
