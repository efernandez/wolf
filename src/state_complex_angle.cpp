
#include "state_complex_angle.h"

StateComplexAngle::StateComplexAngle(Eigen::VectorXs& _st_remote, const unsigned int _idx) :
	StateBase(_st_remote, _idx)
{
	//
}

StateComplexAngle::StateComplexAngle(WolfScalar* _st_ptr) :
	StateBase(_st_ptr)
{
	//
}

StateComplexAngle::~StateComplexAngle()
{
	//
}

stateType StateComplexAngle::getStateType() const
{
	return ST_COMPLEX_ANGLE;
}

void StateComplexAngle::printSelf(unsigned int _ntabs, std::ostream& _ost) const
{
	printTabs(_ntabs);
	_ost << *this->state_ptr_<< std::endl;
	printTabs(_ntabs);
	_ost << *(this->state_ptr_+1) << std::endl;
}
