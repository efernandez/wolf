
#include "landmark_base.h"

LandmarkBase::LandmarkBase(const LandmarkType & _tp, StateBase* _p_ptr, StateOrientation* _o_ptr, StateBase* _v_ptr, StateBase* _w_ptr) :
            NodeLinked(MID, "LANDMARK"),
            type_(_tp),
            status_(LANDMARK_CANDIDATE),
            hit_count_(1),
			p_ptr_(_p_ptr),
			o_ptr_(_o_ptr),
			v_ptr_(_v_ptr),
			w_ptr_(_w_ptr)
{
    //
}

//LandmarkBase::LandmarkBase(const LandmarkType & _tp, const StateBaseList& _st_list) :
//            NodeLinked(MID, "LANDMARK"),
//            type_(_tp),
//            status_(LANDMARK_CANDIDATE),
//            hit_count_(1),
//			st_list_(_st_list)
//{
//    //
//}
                
LandmarkBase::~LandmarkBase()
{
	//std::cout << "deleting LandmarkBase " << nodeId() << std::endl;

	// Remove Landmark State Units
	if (p_ptr_ != nullptr)
		getTop()->removeState(p_ptr_);
	if (o_ptr_ != nullptr)
		getTop()->removeState(o_ptr_);
	if (v_ptr_ != nullptr)
		getTop()->removeState(v_ptr_);
	if (w_ptr_ != nullptr)
		getTop()->removeState(w_ptr_);
}

void LandmarkBase::setStatus(LandmarkStatus _st)
{
    status_ = _st;
}

void LandmarkBase::hit()
{
    hit_count_ ++;
}

void LandmarkBase::unhit()
{
    hit_count_ --;
}

void LandmarkBase::fix()
{
	//std::cout << "Fixing frame " << nodeId() << std::endl;
	status_ = LANDMARK_FIXED;

	// Frame State Units
	if (p_ptr_!=nullptr)
		p_ptr_->setStateStatus(ST_FIXED);
	if (o_ptr_!=nullptr)
		o_ptr_->setStateStatus(ST_FIXED);
	if (v_ptr_!=nullptr)
		v_ptr_->setStateStatus(ST_FIXED);
	if (w_ptr_!=nullptr)
		w_ptr_->setStateStatus(ST_FIXED);
}

void LandmarkBase::unfix()
{
	//std::cout << "Unfixing frame " << nodeId() << std::endl;
	status_ = LANDMARK_ESTIMATED;

	// Frame State Units
	if (p_ptr_!=nullptr)
		p_ptr_->setStateStatus(ST_ESTIMATED);
	if (o_ptr_!=nullptr)
		o_ptr_->setStateStatus(ST_ESTIMATED);
	if (v_ptr_!=nullptr)
		v_ptr_->setStateStatus(ST_ESTIMATED);
	if (w_ptr_!=nullptr)
		w_ptr_->setStateStatus(ST_ESTIMATED);
}

unsigned int LandmarkBase::getHits() const
{
    return hit_count_;
}

StateBase* LandmarkBase::getPPtr() const
{
	return p_ptr_;
}

StateOrientation* LandmarkBase::getOPtr() const
{
	return o_ptr_;
}

StateBase* LandmarkBase::getVPtr() const
{
	return v_ptr_;
}

StateBase* LandmarkBase::getWPtr() const
{
	return w_ptr_;
}

void LandmarkBase::setDescriptor(const Eigen::VectorXs& _descriptor)
{
	descriptor_ = _descriptor;
}

const Eigen::VectorXs& LandmarkBase::getDescriptor() const
{
	return descriptor_;
}

WolfScalar LandmarkBase::getDescriptor(unsigned int _ii) const
{
    return descriptor_(_ii);
}

const LandmarkType LandmarkBase::getType() const
{
    return type_;
}

//const StateBasePtr LandmarkBase::getStatePtr() const
//{
//	return st_list_.front();
//}

//const StateBasePtrList* LandmarkBase::getStateListPtr() const
//{
//	return &st_list_;
//}

//void LandmarkBase::printSelf(unsigned int _ntabs, std::ostream& _ost) const
//{
//    NodeLinked::printSelf(_ntabs, _ost);
//    if (p_ptr_.get() != nullptr)
//    {
//    	_ost << "\tPosition : \n";
//    	p_ptr_->printSelf(_ntabs,_ost);
//    }
//    if (o_ptr_.get() != nullptr)
//    {
//		_ost << "\tOrientation : \n";
//		o_ptr_->printSelf(_ntabs,_ost);
//    }
//    if (v_ptr_.get() != nullptr)
//    {
//    	_ost << "\tVelocity : \n";
//    	v_ptr_->printSelf(_ntabs,_ost);
//    }
//    if (w_ptr_.get() != nullptr)
//    {
//    	_ost << "\tAngular velocity : \n";
//    	v_ptr_->printSelf(_ntabs,_ost);
//    }
//}
