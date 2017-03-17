#include "LinearResistor.h"

using namespace DPsim;

LinearResistor::LinearResistor(std::string name, int src, int dest, double resistance) : BaseComponent(name, src, dest) {
	this->resistance = resistance;
	this->conductance = 1.0 / resistance;
}	
		
void LinearResistor::applySystemMatrixStamp(DPSMatrix& g, int compOffset, double om, double dt) {

	// Set diagonal entries
	if (mNode1 >= 0) {
		g(mNode1,mNode1) = g(mNode1,mNode1) + conductance;
		g(compOffset+mNode1,compOffset+mNode1) = g(compOffset+mNode1,compOffset+mNode1) + conductance;
	}
	
	if (mNode2 >= 0) {
		g(mNode2,mNode2) = g(mNode2,mNode2) + conductance;
		g(compOffset+mNode2,compOffset+mNode2) = g(compOffset+mNode2,compOffset+mNode2) + conductance;
	}

	// Set off diagonal entries
	if (mNode1 >= 0 && mNode2 >= 0) {
		g(mNode1, mNode2) = g(mNode1, mNode2) - conductance;
		g(compOffset + mNode1, compOffset + mNode2) = g(compOffset + mNode1, compOffset + mNode2) - conductance;

		g(mNode2, mNode1) = g(mNode2, mNode1) - conductance;
		g(compOffset + mNode2, compOffset + mNode1) = g(compOffset + mNode2, compOffset + mNode1) - conductance;
	}
}

//void applySystemMatrixStamp2(Simulation& sim) {

//}
	

