#include "capture_laser_2D.h"

unsigned int CaptureLaser2D::segment_window_size = 8;//window size to extract segments
double CaptureLaser2D::theta_min = 0.4; //minimum theta between consecutive segments to detect corner. PI/8=0.39
double CaptureLaser2D::theta_max_parallel = 0.1; //maximum theta between consecutive segments to fuse them in a single line.
double CaptureLaser2D::k_sigmas = 3.;//How many std_dev are tolerated to count that a point is supporting a line
unsigned int CaptureLaser2D::max_beam_distance = 5;//max number of beams of distance between lines to consider corner or concatenation
double CaptureLaser2D::max_distance = 0.5;//max distance between line ends to consider corner or concatenation

CaptureLaser2D::CaptureLaser2D(const TimeStamp & _ts, const SensorLaser2DPtr & _sensor_ptr, const Eigen::VectorXs& _ranges):
	CaptureBase(_ts, _sensor_ptr, _ranges),
	ranges_(data_.data(), _ranges.size()),
	intensities_(data_.data(), 0)
{
    // 
}

CaptureLaser2D::CaptureLaser2D(const TimeStamp & _ts, const SensorLaser2DPtr & _sensor_ptr, const Eigen::VectorXs& _ranges, const Eigen::VectorXs& _intensities):
		CaptureBase(_ts, _sensor_ptr, _ranges),
		ranges_(data_.data(), _ranges.size()),
		intensities_(data_.data(), _intensities.size())
{
	//
}

CaptureLaser2D::~CaptureLaser2D()
{
    //
}

void CaptureLaser2D::processCapture()
{
    //variables
    std::list<Eigen::Vector2s> corners;
    
    //extract corners from range data
    extractCorners(corners);
    
    //generate a feature for each corner
    createFeatures(corners);
    
    //Establish constraints for each feature
    //TODO
    
}

unsigned int CaptureLaser2D::extractCorners(std::list<Eigen::Vector2s> & _corner_list) const
{

    //local variables
    Eigen::MatrixXs points(3,data_.size());
    double azimuth, cos_theta, theta;
    Eigen::Vector3s corner;
    Line line;
    std::list<Line> line_list;
    std::list<Line>::iterator line_it1, line_it2;
    std::queue<unsigned int> jumps;

    
    //cast to specialized sensor. TODO: Could be done once at the constructor ?
    SensorLaser2DPtr laser_ptr = (const SensorLaser2DPtr)sensor_ptr_;
    
    //init from sensor this->sensor_ptr_
    //double aperture = laser_ptr->getAngleMax() - laser_ptr->getAngleMin();
    double azimuth_step = laser_ptr->getAngleIncrement();
    double range_std_dev = laser_ptr->getRangeStdDev();

    std::default_random_engine generator(1);
    std::uniform_int_distribution<int> rand_window_overlapping(1,segment_window_size);
    
    //convert range polar data to cartesian points.
    azimuth = laser_ptr->getAngleMin();
    for (unsigned int ii = 0; ii<ranges_.size(); ii++)
	{
		points.col(ii) << ranges_(ii)*cos(azimuth), ranges_(ii)*sin(azimuth), 1; //points.row0-> x coordinate, points.row1->y coordinate
		azimuth += azimuth_step;
		if (ii > 0 && fabs(ranges_(ii)-ranges_(ii-1)) > max_distance)
		{
			// store jumps
			jumps.push(ii);
			// consider jumps as a corners
			if (ranges_(ii) < ranges_(ii-1))
				_corner_list.push_back( Eigen::Vector2s(points(0,ii),points(1,ii)) );
			else
				_corner_list.push_back( Eigen::Vector2s(points(0,ii-1),points(1,ii-1)) );
		}
	}

    //find line segments running over the scan
    for (unsigned int ii = segment_window_size-1; ii<ranges_.size(); ii=ii+rand_window_overlapping(generator) )
    {
    	unsigned int i_from = ii - segment_window_size + 1;

    	// update the jump to be checked
    	while (!jumps.empty() && i_from > jumps.front())
    		jumps.pop();

    	// check if there is a jump inside the window (not fitting if it is the case)
    	if (jumps.front() > i_from && jumps.front() <= ii)
    	{
    	    //std::cout << "jump: " << jumps.front() << " inside the window: " << i_from << "-"<<ii << std::endl;
    		continue;
    	}
		//Found the best fitting line over points within the window [ii - segment_window_size + 1, ii]
		fitLine(i_from, ii, points, line);

		//if error below stdev, add line to result set
		if ( line.error < range_std_dev*k_sigmas )
			line_list.push_back(line);
    }

    std::cout << "Lines fitted: " << line_list.size() << std::endl;

    // concatenating and corners only if more than 1 line
    if ( line_list.size() > 1 )
	{
    	// concatenate lines
    	line_it1 = line_list.begin();
		line_it2 = line_it1;
		line_it2 ++;
		while ( line_it1 != line_list.end() )
		{
			// last of current line and first of next line too far
			if (line_it2->first > line_it1->last + max_beam_distance)
			{
				//std::cout << "lines too far:\nlast of current: " << line_it1->last << " first of next: " << line_it2->first << std::endl;
				line_it1 ++;
				line_it2 = line_it1;
				line_it2 ++;
			}
			else
			{
				//compute angle between lines 1 and 2
				cos_theta = (line_it1->vector).dot(line_it2->vector) / ( (line_it1->vector).norm()*(line_it2->vector).norm() );
				theta = acos (cos_theta);
	//            std::cout << std::endl << "cos_theta: " << cos_theta << std::endl <<
	//                                      "theta: " << theta << std::endl <<
	//                                      "*index_it1: " << *index_it1 << std::endl <<
	//                                      "*index_it2: " << *index_it2 << std::endl;
	//                                       "   (*line_it1).dot(*line_it2): " << (*line_it1).dot(*line_it2) << std::endl <<
	//                                       "   (*line_it1).norm()*(*line_it2).norm(): " << (*line_it1).norm()*(*line_it2).norm() << std::endl;


				//Check angle threshold and consecutiveness of lines in the scan
				if ( fabs(theta) < theta_max_parallel )
				{
					Line new_line;
					fitLine(line_it1->first, line_it2->last, points, new_line);
					if ( new_line.error < range_std_dev*k_sigmas )
					{
						*line_it1 = new_line;
						line_it2 = line_list.erase(line_it2);

//						std::cout << "lines concatenated" << std::endl;
//						std::cout << "line 1: " << std::endl << line_it1->first << std::endl <<
//							 line_it1->last << std::endl <<
//							 line_it1->vector.transpose() << std::endl <<
//							 line_it1->error << std::endl;
//						std::cout << "line 2: " << std::endl << line_it2->first << std::endl <<
//							 line_it2->last << std::endl <<
//							 line_it2->vector.transpose() << std::endl <<
//							 line_it2->error << std::endl;
//						std::cout << "line resultant: "<< std::endl << new_line.first << std::endl <<
//							new_line.last << std::endl <<
//							new_line.vector.transpose() << std::endl <<
//							new_line.error << std::endl;
					}
					else
					{
						//update iterators
						line_it1 ++;
						line_it2 = line_it1;
						line_it2 ++;
					}
				}
				else
				{
					//update iterators
					line_it1 ++;
					line_it2 = line_it1;
					line_it2 ++;
				}
			}
		}

		std::cout << "Lines after concatenation: " << line_list.size() << std::endl;

		// find corners over the line list
        line_it1 = line_list.begin();
        line_it2 = line_it1;
        line_it2 ++;
        while ( line_it1 != line_list.end() )
        {   
        	// last of current line and first of next line too far
        	if (line_it2->first > line_it1->last + max_beam_distance)
			{
        		//std::cout << "lines too far:\nlast of current: " << line_it1->last << " first of next: " << line_it2->first << std::endl;
        		line_it1 ++;
                line_it2 = line_it1;
        		line_it2 ++;
			}
        	else
        	{
				//compute angle between lines 1 and 2
				cos_theta = (line_it1->vector).dot(line_it2->vector) / ( (line_it1->vector).norm()*(line_it2->vector).norm() );
				theta = acos (cos_theta);
//	            std::cout << std::endl << "cos_theta: " << cos_theta << std::endl <<
//	                                      "theta: " << theta << std::endl <<
//	                                      "line 1: " << line_it1->first << "-" << line_it1->last << std::endl <<
//	                                      "line 2: " << line_it2->first << "-" << line_it2->last << std::endl <<
//	                                      "   (*line_it1).dot(*line_it2): " << (line_it1->vector).dot(line_it2->vector) << std::endl <<
//	                                      "   (*line_it1).norm()*(*line_it2).norm(): " << (line_it1->vector).norm()*(line_it2->vector).norm() << std::endl;


				//Check angle threshold and consecutiveness of lines in the scan
				if ( fabs(theta) > theta_min )
				{
					corner = (line_it1->vector).cross(line_it2->vector); //cross product between lines is the intersection
					corner = corner / corner(2); //normalize point to set last component to 1
//					std::cout << "Corner candidate:" << std::endl << "theta: " << theta << std::endl << "corner: " << corner.transpose() << std::endl <<
//							 "last point first line: " << points.col(line_it1->last).transpose() << std::endl <<
//							 "first point second line: " << points.col(line_it2->first).transpose() << std::endl;
					// Check if the corner is close to both lines ends
					if ( (points.col(line_it1->last) - corner).head(2).norm() < max_distance && (points.col(line_it2->first) - corner).head(2).norm() < max_distance)
					{
						//std::cout << "Corner detected!" << std::endl;
						_corner_list.push_back( Eigen::Vector2s(corner(0),corner(1)) );
					}
				}

				//update iterators
				if (line_it2 != line_list.end() )
					line_it2 ++;
				else
				{
					line_it1 ++;
					line_it2 = line_it1;
					line_it2 ++;
				}
        	}
        }
    }

    std::cout << "Corners extracted: " << _corner_list.size() << std::endl;

    // Erase duplicated corners
	if ( _corner_list.size() > 1 )
	{
		// concatenate lines
		std::list<Eigen::Vector2s>::iterator corner_it1 = _corner_list.begin();
		std::list<Eigen::Vector2s>::iterator corner_it2 = corner_it1;
		corner_it2 ++;
		while ( corner_it1 != _corner_list.end() )
		{
			// Check if two corners are close enough
			if ( (*corner_it1 - *corner_it2).head(2).norm() < max_distance )
			{
				// TODO: keep the one with larger lines
				// compute the mean
				*corner_it1 = (*corner_it1 + *corner_it2) / 2;
				corner_it2 = _corner_list.erase(corner_it2);
			}
			else
			{
				corner_it1 ++;
				corner_it2 = corner_it1;
				corner_it2 ++;
			}
		}
	}

	std::cout << "Corners after removing duplicates: " << _corner_list.size() << std::endl;

    return _corner_list.size();
}

void CaptureLaser2D::fitLine(unsigned int _idx_from, unsigned int _idx_to, const Eigen::MatrixXs& _points, Line& line_) const
{
	line_.first = _idx_from;
	line_.last = _idx_to;

	//Found the best fitting line over points within the window. Build the system: A*line=[0 0 1]'. Matrix A = a_ij
	Eigen::Matrix3s AA = _points.block(0, _idx_from, 3, _idx_to-_idx_from+1) * _points.block(0, _idx_from, 3, _idx_to-_idx_from+1).transpose();
	AA.row(2) << 0,0,1;

	//solve for line
	line_.vector = AA.inverse().col(2);

	// compute fitting error
	line_.error = 0;
    for(unsigned int jj = _idx_from; jj<=_idx_to; jj++)
    	line_.error += fabs( line_.vector(0)*_points(0,jj) + line_.vector(1)*_points(1,jj) + line_.vector(2)) / sqrt( line_.vector(0)*line_.vector(0) + line_.vector(1)*line_.vector(1) );
    line_.error /= (_idx_to-_idx_from+1);
}

void CaptureLaser2D::createFeatures(std::list<Eigen::Vector2s> & _corner_list)
{
    std::list<Eigen::Vector2s>::iterator corner_it;
    Eigen::Matrix2s cov_mat;
    
    //init constant cov
    cov_mat << 0.01,0,0,0.01;
    
    //for each corner in the list create a feature
    for (corner_it = _corner_list.begin(); corner_it != _corner_list.end(); corner_it ++)
    {
        std::shared_ptr<FeatureCorner2D> ft_shptr( new FeatureCorner2D( (*corner_it), cov_mat ) );
        this->addFeature( (FeatureBaseShPtr&)ft_shptr );
    }
}

Eigen::VectorXs CaptureLaser2D::computePrior() const
{
    return Eigen::Vector3s(1,2,3);
}

void CaptureLaser2D::findCorrespondences()
{
        //
}

