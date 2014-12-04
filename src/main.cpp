#include <iostream>
#include <cstdlib>
#include <math.h>

//Libraire pnl
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_basis.h"

//Lib de parallelisation
//#include <omp.h>
#include <mpi.h>

//Lib du priver
#include "optionBarrierLow.h"
#include "bs.h"
#include "mc.h"


using namespace std;


int main(int argc, char **argv){

	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	//Temps de début
	double begin = MPI_Wtime();

	//Test sur une basket
	//char *infile = argv[1];
	//Param *P = new Parser(infile);
		

	MPI_Status status;

	//Maitre
	if(rank == 0){
		cout<<"Je suis le maitre !"<<endl;
	//int sizeVect = 3;
	//PnlVect *V = pnl_vect_create_from_scalar (sizeVect,3.14);
	
		/** --------- Type natif ---------------**/
		//const char *infile = "../data/basket.dat";
		//char *infile = argv[1];
		//Param *P = new Parser(infile);

		//Definition de ma variable:
		int maVariable = 36;
		char *buf;
		int info, count, bufsize=0, pos=0;
		info=MPI_Pack_size(1,MPI_INT,MPI_COMM_WORLD,&count);
		bufsize += count;
		buf = new char[bufsize];
		info=MPI_Pack(&(maVariable),1,MPI_INT,buf,bufsize,&pos,MPI_COMM_WORLD);


		//J'envoie
		//MPI_Send(void *buf, int count, MPI_Datatype datatype,int dest, int tag, MPI_Comm comm);
		MPI_Send(buf, bufsize, MPI_PACKED,1, 12, MPI_COMM_WORLD);
		/** ---------------------------------- **/
		/** --------- Type pnl --------------- **/
		/*char *buf;
		int info, count, bufsize=0, pos=0;
		info=MPI_Pack_size(1,MPI_INT, MPI_COMM_WORLD,&count);
		if (info) return(info);
		bufsize += count;
		info=MPI_Pack_size(V->size,MPI_DOUBLE,MPI_COMM_WORLD,&count);
		if (info) return(info);
		bufsize += count;
		buf = new char[size];
		info=MPI_Pack(&(V->size),1,MPI_INT,buf,bufsize,&pos,MPI_COMM_WORLD);
		if (info) return info;
		info=MPI_Pack(V->array,V->size,MPI_DOUBLE,buf,bufsize,&pos,MPI_COMM_WORLD);
		return info;

		MPI_Send(buf, bufsize, MPI_PACKED, 1, 12, MPI_COMM_WORLD);*/

	}

	//Esclave
	if( rank != 0){
		cout<<"Je suis l'esclave !"<<endl;
		
		/*char *buffer ;
		int sizeBuffer;
		int info,n;
		int position=0;

		info = MPI_Probe(0, 12, MPI_COMM_WORLD, &status);
		//if ( info ) return info;
		info = MPI_Get_count(&status, MPI_PACKED, &sizeBuffer);
		//if ( info ) return info

		buffer = new char[sizeBuffer];

		//Je recois
		//MPI_Recv(void *buf, int count, MPI_Datatype datatypeint source, int tag, MPI_Comm comm, MPI_Status *status);
		MPI_Recv(&buffer, 1, MPI_INT,0, 12, MPI_COMM_WORLD, &status);
	

		//J'unpack
		//MPI_Unpack(void *inbuf, int insize, int *position,void *outbuf, int outcount, MPI_Datatype datatype,MPI_Comm comm);
		MPI_Unpack(buffer, 4, &position,&n,1, MPI_INT,MPI_COMM_WORLD);
		
		cout<<n<<endl;*/

		/******************************************/

		/** --------- Type natif ---------------**/
		int n, pos;
		int info, bufsize;
		char *buf;
		//MPI_Status status;
		info = MPI_Probe(0, 12, MPI_COMM_WORLD, &status);
		if ( info ) return info;
		info = MPI_Get_count(&status, MPI_PACKED, &bufsize);
		if ( info ) return info;
		buf = new char[bufsize];
		info = MPI_Recv(buf,bufsize,MPI_PACKED,0,12,MPI_COMM_WORLD,&status);
		if (info) return info;
		
		MPI_Unpack(buf, bufsize, &pos,&n,1, MPI_INT,MPI_COMM_WORLD);

		cout<<"Résultat :"<<n<<endl;

		
		free (buf);
		/** ---------------------------------- **/

		/** --------- Type pnl ---------------**/
		/*char *buf;
		int n, info, pos=0;
		int bufsize;


		MPI_Status status;*/
		/*info = MPI_Probe(0, 12, MPI_COMM_WORLD, &status);
		if ( info ) return info;
		info = MPI_Get_count(&status, MPI_PACKED, &bufsize);
		if ( info ) return info;
		buf = new char[bufsize];
		info = MPI_Recv(buf,bufsize,MPI_PACKED,0,12,MPI_COMM_WORLD,&status);
		if (info) return info;*/

		/*PnlVect *U = pnl_vect_new ();
		info=MPI_Unpack(buf,bufsize,&pos,&n,1,MPI_INT,MPI_COMM_WORLD);
		if (info) return info;
		pnl_vect_object_resize(PNL_VECT_OBJECT(U), n);
		info=MPI_Unpack(buf,bufsize,&pos,U->array,n,MPI_DOUBLE,MPI_COMM_WORLD);
		return info;*/

		/** ---------------------------------- **/

		//J'instancie
		//MonteCarlo *mc = new MonteCarlo(P);
		//double prix;
		//double ic;
		//mc->price(prix, ic);
	}

	/*cout << "Output : " << endl<< "Nombre de tirages :"<<mc->samples_<<endl
	 << "Price : " << prix << endl << "Confidence interval : " << ic << endl;
	//Temps de fin et affichage
	double end = MPI_Wtime();
	cout<< "Temps :"<<end-begin <<endl;*/

	
		//delete opt;
		//delete P;
		//delete mc;
	


	MPI_Finalize ();

	return 0;

}