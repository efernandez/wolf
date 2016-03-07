#include "capture_base.h"
#include "frame_base.h"
#include "sensor_base.h"
#include "feature_base.h"

CaptureBase::CaptureBase(const TimeStamp& _ts, SensorBase* _sensor_ptr) :
        NodeLinked(MID, "CAPTURE"),
        time_stamp_(_ts),
        sensor_ptr_(_sensor_ptr),
	sensor_p_ptr_(sensor_ptr_->getPPtr()),
	sensor_o_ptr_(sensor_ptr_->getOPtr())
{
    //
}


CaptureBase::~CaptureBase()
{
	//std::cout << "deleting CaptureBase " << nodeId() << std::endl;
}

void CaptureBase::getConstraintList(ConstraintBaseList & _ctr_list)
{
	for(auto f_it = getFeatureListPtr()->begin(); f_it != getFeatureListPtr()->end(); ++f_it)
		(*f_it)->getConstraintFromList(_ctr_list);
}

void CaptureBase::process()
{
    // Call all processors assigned to the sensor that captured this data
    for (auto processor_iter = sensor_ptr_->getProcessorListPtr()->begin(); processor_iter != sensor_ptr_->getProcessorListPtr()->end(); ++processor_iter)
    {
        (*processor_iter)->process(this);
    }
}


StateBlock* CaptureBase::getSensorPPtr() const {
	if (getSensorPtr()->isExtrinsicDynamic())
		return sensor_p_ptr_;
	else
		return getSensorPtr()->getPPtr();
}

StateBlock* CaptureBase::getSensorOPtr() const {
	if (getSensorPtr()->isExtrinsicDynamic())
		return sensor_o_ptr_;
	else
		return getSensorPtr()->getOPtr();
}

