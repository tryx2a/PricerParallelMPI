#include <iostream>

using namespace std;

#include "option.h"


Option::Option(const double T_, const int timeSteps_, const int size_){
  this->T_ = T_;
  this->TimeSteps_ = timeSteps_;
  this->size_ = size_;
  this->id = 0;
}

/*Option::Option(double T_, int TimeSteps_, int size, int id){
	this->T_ = T_;
	this->TimeSteps_ = TimeSteps_;
  	this->size_ = size;
  	this->id = id;
}*/

  	Option::Option(){
  		this->T_ = 0;
  		this->TimeSteps_ = 0;
  		this->size_ = 0;
  		this->id = 0;
  	}




Option::~Option(){}

