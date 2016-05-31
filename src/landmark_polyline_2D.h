/**
 * \file landmark_poliyline_2D.h
 *
 *  Created on: May 26, 2016
 *      \author: jvallve
 */

#ifndef LANDMARK_POLYLINE_2D_H_
#define LANDMARK_POLYLINE_2D_H_

#include "landmark_base.h"
#include <deque>

namespace wolf
{

class LandmarkPolyline2D : public LandmarkBase
{
    protected:
        std::deque<StateBlock*> point_state_ptr_vector_; ///< polyline points state blocks
        bool first_defined_;            ///< Wether the first point is an extreme of a line or the line may continue
        bool last_defined_;             ///< Wether the last point is an extreme of a line or the line may continue

    public:
        LandmarkPolyline2D(FeaturePolyline2D* _polyline_ptr);
        LandmarkPolyline2D(const Eigen::MatrixXs& _points, const bool _first_defined, const bool _last_defined);
        virtual ~LandmarkPolyline2D();

        /** \brief Gets a const reference to the point state block pointer vector
         **/
        std::deque<StateBlock*>& getPointStatePtrDeque();

        /** \brief Gets wether the first/last points are extreme or not
         **/
        bool isFirstDefined() const;
        bool isLastDefined() const;

        /** \brief Sets the first/last extreme point
         **/
        void setFirst(const Eigen::VectorXs& _point, bool _defined);
        void setLast(const Eigen::VectorXs& _point, bool _defined);

        unsigned int getNPoints() const;

        /** \brief Adds a new point to the landmark
         * \param _point: the point to be added
         * \param _extreme: if its extreme or not
         * \param _back: if it have to be added in the back (true) or in the front (false)
         **/
        void addPoint(const Eigen::VectorXs& _point, const bool& _defined, const bool& _back);

        /** \brief Adds new points to the landmark
         * \param _points: a matrix containing points, some of them to be added
         * \param _idx: from wich position of the first point to be added
         * \param _extreme: if last point to be added is extreme or not
         * \param _back: if the points have to be added in the back (true) or in the front (false)
         **/
        void addPoints(const Eigen::MatrixXs& _points, const int& _idx, const bool& _defined, const bool& _back);
};

inline std::deque<StateBlock*>& LandmarkPolyline2D::getPointStatePtrDeque()
{
    return point_state_ptr_vector_;
}

inline bool LandmarkPolyline2D::isFirstDefined() const
{
    return first_defined_;
}

inline bool LandmarkPolyline2D::isLastDefined() const
{
    return last_defined_;
}

inline unsigned int LandmarkPolyline2D::getNPoints() const
{
    return point_state_ptr_vector_.size();
}

} /* namespace wolf */

#endif /* LANDMARK_POLYLINE_2D_H_ */