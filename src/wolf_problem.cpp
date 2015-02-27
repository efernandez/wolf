#include "wolf_problem.h"

WolfProblem::WolfProblem(unsigned int _size) :
        NodeBase("WOLF_PROBLEM"), //
		state_(_size),
        location_(TOP)
{
}

WolfProblem::WolfProblem(const TrajectoryBaseShPtr _trajectory_ptr, const MapBaseShPtr _map_ptr, unsigned int _size) :
        NodeBase("WOLF_PROBLEM"), //
		state_(_size),
		state_idx_last_(0),
        location_(TOP),
		map_ptr_(_map_ptr),
		trajectory_ptr_(_trajectory_ptr)
{
}

WolfProblem::~WolfProblem()
{
}

void WolfProblem::addState(const StateBaseShPtr _new_state, const Eigen::VectorXs& _new_state_values)
{
	// Check if resize should be done
	if (state_idx_last_+_new_state->getStateSize() > state_.size())
	{
		std::cout << "\nState size: " << state_.size() << " last idx: " << state_idx_last_ << " last idx + new state size: " << state_idx_last_+_new_state->getStateSize() << std::endl;
		std::cout << "Resizing state and remapping al state units..." << std::endl;
		WolfScalar* old_first_pointer = state_.data();
		state_.resize(state_.size()*2);
		for (auto state_units_it = state_list_.begin(); state_units_it != state_list_.end(); state_units_it++)
			(*state_units_it)->setPtr(state_.data() + ( (*state_units_it)->getPtr() - old_first_pointer) );
		std::cout << "New state size: " << state_.size() << "last idx: " << state_idx_last_ << std::endl;
	}
	// copy the values of the new state
	assert(_new_state_values.size() == _new_state->getStateSize() && "Different state unit and vector sizes");
	state_.segment(state_idx_last_,_new_state->getStateSize()) = _new_state_values;
	// add the state unit to the list
	state_list_.push_back(_new_state);
	// update the last state index
	state_idx_last_ += _new_state->getStateSize();
}

WolfScalar* WolfProblem::getStatePtr()
{
	return state_.data();
}

WolfScalar* WolfProblem::getNewStatePtr()
{
	return state_.data()+state_idx_last_+1;
}

const unsigned int WolfProblem::getStateSize() const
{
	return state_idx_last_;
}

//void WolfProblem::setStateIdx(unsigned int _idx)
//{
//	state_idx_last_ = _idx;
//}

void WolfProblem::addMap(const MapBaseShPtr _map_ptr)
{
	map_ptr_ = _map_ptr;
}

void WolfProblem::addTrajectory(const TrajectoryBaseShPtr _trajectory_ptr)
{
	trajectory_ptr_ = _trajectory_ptr;
}

MapBasePtr WolfProblem::getMapPtr()
{
	return map_ptr_.get();
}

TrajectoryBasePtr WolfProblem::getTrajectoryPtr()
{
	return trajectory_ptr_.get();
}

StateBaseList* WolfProblem::getStateListPtr()
{
	return &state_list_;
}

void WolfProblem::print(unsigned int _ntabs, std::ostream& _ost) const
{
    printSelf(_ntabs, _ost); //one line
    printLower(_ntabs, _ost);
}

void WolfProblem::printSelf(unsigned int _ntabs, std::ostream& _ost) const
{
    printTabs(_ntabs);
    _ost << nodeLabel() << " " << nodeId() << " : ";
    _ost << "TOP" << std::endl;
}

const Eigen::VectorXs WolfProblem::getState() const
{
	return state_;
}

WolfProblem* WolfProblem::getTop()
{
	return this;
}

void WolfProblem::printLower(unsigned int _ntabs, std::ostream& _ost) const
{
    printTabs(_ntabs);
    _ost << "\tLower Nodes  ==> [ ";
    _ost << map_ptr_->nodeId() << " ";
    _ost << trajectory_ptr_->nodeId() << " ]" << std::endl;
    _ntabs++;
	map_ptr_->print(_ntabs, _ost);
	trajectory_ptr_->print(_ntabs, _ost);

}
