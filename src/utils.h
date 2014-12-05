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

	/*
	 * Méthodes permettant de packer
	 */
	int bs_mpi_pack_size(int* bufsize, int* count, int* pos, MPI_Comm comm, BS* mod_);
 	int bs_mpi_pack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm, BS* mod_);

  	int opt_mpi_pack_size(int* bufsize, int* count, int* pos,MPI_Comm comm, Option* opt_);
  	int opt_mpi_pack(void **buf, int* bufsize, int* count, int* pos,MPI_Comm comm, Option *opt_);

  	int mc_mpi_pack_size(int* bufsize, int* count, int* pos,MPI_Comm comm);
  	int mc_mpi_pack(void **buf, int* bufsize, int* count, int* pos,MPI_Comm comm, MonteCarlo *mc);


  /*
	 * Méthodes permettant de unpacker
	 */
	BS* bs_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm);

	Option* opt_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm);

	MonteCarlo* mc_mpi_unpack(void **buf, int* bufsize, int* count, int* pos, MPI_Comm comm,BS* bs, Option* op, int rank);

	//Princing du master
	void price_master(double *PrixTotal, double *ICTotal, int sizeCommWorld);

} // utils


#endif // UTILS_H
