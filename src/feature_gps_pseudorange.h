
#ifndef FEATURE_GPS_PSEUDORANGE_H_
#define FEATURE_GPS_PSEUDORANGE_H_

//Wolf includes
#include "feature_base.h"

//std includes
#include <iomanip>


namespace wolf {


// TODO manage covariance

class FeatureGPSPseudorange : public FeatureBase
{
protected:
    Eigen::Vector3s sat_position_;
    Scalar pseudorange_;

public:
    FeatureGPSPseudorange(Eigen::Vector3s& _sat_position, Scalar _pseudorange, Scalar _covariance);

    /** \brief Default destructor (not recommended)
     *
     * Default destructor (please use destruct() instead of delete for guaranteeing the wolf tree integrity)
     *
     **/
    virtual ~FeatureGPSPseudorange();

    const Eigen::Vector3s & getSatPosition() const;

    Scalar getPseudorange() const;

};

} // namespace wolf

#endif //FEATURE_GPS_PSEUDORANGE_H_
