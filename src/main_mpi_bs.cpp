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

	utils::priceParallel(argc, argv);
	return 0;

}