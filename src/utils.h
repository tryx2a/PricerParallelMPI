#ifndef UTILS_H
#define UTILS_H

#include "option.h"
#include "optionBasket.h"
#include "optionAsian.h"
#include "optionBarrierLow.h"
#include "optionBarrierUp.h"
#include "optionBarrier.h"
#include "optionPerformance.h"
#include "bs.h"
#include "mc.h"


#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

namespace utils {
	double computePayoff(const PnlMat *path, const PnlVect *payoffCoeff_, int TimeSteps_, double strike);	

	BS* bs_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm);
	Option* opt_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm);
	MonteCarlo* mc_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm,BS* bs, Option* op, int rank);
} // utils


#endif // UTILS_H
