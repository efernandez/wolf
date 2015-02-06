#include "correspondence_odom_2D_complex_angle.h"

CorrespondenceOdom2DComplexAngle::CorrespondenceOdom2DComplexAngle(const FeatureBaseShPtr& _ftr_ptr,  WolfScalar* _block0Ptr, WolfScalar* _block1Ptr, WolfScalar* _block2Ptr, WolfScalar* _block3Ptr) :
	CorrespondenceSparse<2,2,2,2,2>(_ftr_ptr,CORR_ODOM_2D_THETA, _block0Ptr, _block1Ptr, _block2Ptr, _block3Ptr)
{
}

CorrespondenceOdom2DComplexAngle::CorrespondenceOdom2DComplexAngle(const FeatureBaseShPtr& _ftr_ptr, const StateBaseShPtr& _state0Ptr, const StateBaseShPtr& _state1Ptr, const StateBaseShPtr& _state2Ptr, const StateBaseShPtr& _state3Ptr) :
	CorrespondenceSparse<2,2,2,2,2>(_ftr_ptr,CORR_ODOM_2D_THETA,  _state0Ptr->getPtr(), _state1Ptr->getPtr(),_state2Ptr->getPtr(), _state3Ptr->getPtr())
{
}

CorrespondenceOdom2DComplexAngle::~CorrespondenceOdom2DComplexAngle()
{
}

template <typename T>
bool CorrespondenceOdom2DComplexAngle::operator()(const T* const _p1, const T* const _o1, const T* const _p2, const T* const _o2, T* _residuals) const
{
	// Expected measurement
	T expected_range = (_p2[0]-_p1[0])*(_p2[0]-_p1[0]) + (_p2[1]-_p1[1])*(_p2[1]-_p1[1]); //square of the range
	T expected_rotation = atan2(_o2[1]*_o1[0] - _o2[0]*_o1[1], _o1[0]*_o2[0] + _o1[1]*_o2[1]);

	// Residuals
	_residuals[0] = (expected_range - T((*measurement_ptr_)(0))*T((*measurement_ptr_)(0))) / T((*measurement_covariance_ptr_)(0,0));
	_residuals[1] = (expected_rotation - T((*measurement_ptr_)(1))) / T((*measurement_covariance_ptr_)(1,1));

	return true;
}
