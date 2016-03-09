/**
 * \file activeSearch.hpp
 *
 *  Active search detection and matching for points.
 *
 * \date 10/04/2010
 * \author jsola
 *
 * ## Add detailed description here ##
 *
 * \ingroup rtslam
 */

#ifndef ACTIVESEARCH_HPP_
#define ACTIVESEARCH_HPP_

// OLD HEADERS
/*
#include "jmath/random.hpp"
#include "jmath/jblas.hpp"
#include "image/roi.hpp"

#include "rtslam/gaussian.hpp"
#include "rtslam/rtSlam.hpp"
*/

//OpenCV includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

//Eigen includes
#include <Eigen/Dense>

//standard includes
#include <iostream>

using namespace Eigen;
using namespace std;

/**
         * Active search tesselation grid.
         *
         * \author jsola
         *
         * This class implements a tesselation grid for achieving active search
         * behavior in landmark initialization.
         *
         * The grid defines a set of cells in the image.
         * The idea is to count the number of projected landmarks per grid cell,
         * and use one randomly chosen cell that is empty
         * for feature detection and landmark initialization.
         * This guarantees that the system will automatically populate all the
         * regions of the image.
         *
         * The feature density can be controlled by
         * adjusting the grid's number of cells.
         * Typically, use grids of 5x5 to 9x9 cells.
         *
         * This class implements a few interesting features:
         * - The grid can be randomly re-positioned at each frame to avoid dead zones at the cell edges.
         * - Only the inner cells are activated for feature detection to avoid reaching the image edges.
         * - The region of interest (ROI) associated with a particular cell is shrinked with a parametrizable margin
         *   to guarantee a minimum separation between existing and new features.
         *
         * The blue and green grids in the figure below represent the grid
         * at two different offsets, corresponding to two different frames.
         *
         * 	\image html tesselationGrid.png "The tesselation grid used for active feature detection and initialization"
         *
         * This second figure shows a typical situation that we use to explain the basic mechanism.
         *
         * \image html tesselationExample.png "A typical configuration of the tesselation grid"
         *
         * Observe the figure and use the following facts as an operation guide:
         * - The grid is offset by a fraction of a cell size.
         * 		- use renew() at each frame to clear the grid and set a new offset.
         * - Projected landmarks are represented by red dots.
         * 		- After projection, use hiCell() to add a new dot to the grid.
         * - Cells with projected landmarks inside are 'occupied'.
         * - Only the inner cells (thick blue rectangle) are considered for Region of Interest (ROI) extraction.
         * - One cell is chosen randomly among those that are empty.
         * - The ROI is smaller than the cell to guarantee a minimum feature separation.
         * 		- Use the optional 'separation' parameter at construction time to control this separation.
         * 		- Use pickRoi() to obtain an empty ROI for initialization.
         * - A new feature is to be be searched inside this ROI.
         * - If there is no feature found in this ROI, call blockCell() function not to search in this area again.
         * - If you need to search more than one feature per frame, proceed like this:
         * 		- If successful detection
         *          - add the detected pixel with hiCell().
         *      - Else block the cell zith blockCell().
         * 		- Call pickRoi() again.
         * 		- Repeat these two steps for each feature to be searched.
         *
         * We include here a schematic active-search pseudo-code algorithm to illustrate its operation:
         * \code
         * // Init necessary objects
         * ActiveSearch activeSearch;
         * ActiveSearchGrid grid(640, 480, 4, 4, 10); // Construction with 10 pixels separation.
         *
         * // We start projecting everybody
         * for (obs = begin(); obs != end(); obs++)   // loop observations
         * {
         *   obs->project();
         *   if (obs->isVisible())
         *     grid.hiCell(obs->expectation.x());   // add only visible landmarks
         * }
         *
         * // Then we process the selected observations
         * activeSearch.selectObs();                  // select interesting features
         * for (activeSearch.selectedObs);            // loop selected obs
         *   obs.process();                           // process observation
         *
         * // Now we go to initialization
         * grid.pickRoi(roi);                          // roi is now region of interest
         * if (detectFeature(roi))                    // detect inside ROI
         *   initLandmark();                          // initialize only if successful detection
         * \endcode
         *
         */

class ActiveSearchGrid {

        friend std::ostream& operator <<(std::ostream & s, ActiveSearchGrid const & grid);

    private:
        Eigen::Vector2i img_size_;
        Eigen::Vector2i grid_size_;
        Eigen::Vector2i cell_size_;
        Eigen::Vector2i offset_;
        Eigen::Vector2i roi_coordinates_;
        Eigen::MatrixXi projections_count_;
        Eigen::MatrixXi empty_cells_tile_tmp_;
        int separation_;
        int margin_;

    public:
        /**
                 * Constructor.
                 * \param _img_size_h horizontal image size, in pixels.
                 * \param _img_size_v vertical image size.
                 * \param _n_cells_h horizontal number of cells per image width.
                 * \param _n_cells_v vertical number of cells per image height.
                 * \param separation minimum separation between existing and new points.
                 */
        ActiveSearchGrid(const int & _img_size_h, const int & _img_size_v, const int & _n_cells_h, const int & _n_cells_v, const int & _margin = 0,
                         const int & _separation = 0);

        /**
                 * Clear grid.
                 * Sets all cell counters to zero.
                 */
        void clear();

        /**
                 * Clear grid and position it at a new random location.
                 * Sets all cell counters to zero and sets a new random grid position.
                 */
        void renew();

        /**
                 * Add a projected pixel to the grid.
                 * \param pix the pixel to add.
                 */
        void hitCell(const Eigen::Vector2i & pix);

        /**
                 * Get ROI of a random empty cell.
                 * \param roi the resulting ROI
                 * \return true if ROI exists.
                 */
        bool pickRoi(cv::Mat & roi);

        /**
                 * Call this after pickRoi if no point was found in the roi
                 * in order to avoid searching again in it.
                 * \param roi the ROI where nothing was found
                 */
        void blockCell(const cv::Mat & roi);


    private:
        /**
                 * Get cell corresponding to pixel
                 */
        template<class Vector2i>
        Eigen::Vector2i pix2cell(const Eigen::Vector2i & pix) {
            Eigen::Vector2i cell;
            cell(0) = (pix(0) - offset_(0)) / cell_size_(0);
            cell(1) = (pix(1) - offset_(1)) / cell_size_(1);
            return cell;
        }

        /**
                 * Get cell origin (exact pixel)
                 */
        Eigen::Vector2i cellOrigin(const Eigen::Vector2i & cell);

        /**
                 * Get cell center (can be decimal if size of cell is an odd number of pixels)
                 */
        Eigen::Vector2i cellCenter(const Eigen::Vector2i & cell);

        /**
                 * Get one random empty cell
                 */
        bool pickEmptyCell(Eigen::Vector2i & cell);

        /**
                 * Get the region of interest, reduced by a margin.
                 */
        void cell2roi(const Eigen::Vector2i & cell, cv::Mat & roi);

};


//#if 0
//		/**
//		 * Class for active search algorithms.
//		 * \ingroup rtslam
//		 */
//		class ActiveSearch {
//			public:
//				vecb visibleObs;
//				vecb selectedObs;

//				/**
//				 * Project all landmarks to the sensor space.
//				 *
//				 * This function also computes visibility and information gain
//				 * for each observation.
//				 * The result is a map of visible observations,
//				 * ordered from least to most expected information gain.
//				 *
//				 * \param senPtr pointer to the sensor under consideration.
//				 * \return a map of all observations that are visible from the sensor, ordered according to the information gain.
//				 */
//				std::map<double, observation_ptr_t> projectAll(const sensor_ptr_t & senPtr, size_t & numVis);

//				/**
//				 * Predict observed appearance.
//				 * This function predicts the appearance of the perceived landmark.
//				 * It does so by computing the appearance of the landmark descriptor from the current sensor position.
//				 * The result of this operation is an updated observation.
//				 * \param obsPtr a pointer to the observation.
//				 */
//				void predictApp(const observation_ptr_t & obsPtr);

//				/**
//				 * Scan search region for match.
//				 */
//				void scanObs(const observation_ptr_t & obsPtr, const image::ConvexRoi & roi);
//		};
//#endif

#endif /* ACTIVESEARCH_HPP_ */
