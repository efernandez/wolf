//std includes
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <typeinfo>
#include <ctime>

// Eigen includes
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

//Ceres includes
#include "ceres/jet.h"
#include "ceres/ceres.h"
#include "glog/logging.h"

//Wolf includes
#include "wolf.h"

/**
 * This test implements an optimization using CERES of a vehicle trajectory using odometry and GPS simulated data.
 *
 **/

using namespace Eigen;
enum costFunctionType {
	AUTO,
	NUMERIC};
enum correspondenceType {
	GPS_2D,
	ODOM_2D_COMPLEX,
	ODOM_2D_THETA};
enum parametrizationType {
	NONE,
	COMPLEX_ANGLE,
	QUATERNION,
	PO_2D};

//class CorrespondenceBase;
//class StateBase;
//
//typedef std::shared_ptr<CorrespondenceBase> CorrespondenceShPtr;
//typedef std::shared_ptr<StateBase> StateShPtr;

class StateBase
{
	protected:
		WolfScalar* state_ptr_;

	public:

		StateBase(VectorXs& _st_remote, const unsigned int _idx) :
			state_ptr_(_st_remote.data() + _idx)
		{
		}

		StateBase(WolfScalar* _st_ptr) :
			state_ptr_(_st_ptr)
		{
		}

		StateBase(StateBase& _st) :
			state_ptr_(_st.state_ptr_)
		{
		}


		virtual ~StateBase()
		{
		}

		virtual WolfScalar* getPtr()
		{
			return state_ptr_;
		}

		virtual parametrizationType getParametrizationType() const = 0;

		virtual void print() const = 0;
};

class StatePoint2D: public StateBase
{
	public:
		static const unsigned int BLOCK_SIZE = 2;

		StatePoint2D(VectorXs& _st_remote, const unsigned int _idx) :
			StateBase(_st_remote, _idx)
		{
		}

		StatePoint2D(WolfScalar* _st_ptr) :
			StateBase(_st_ptr)
		{
		}

		virtual ~StatePoint2D()
		{
		}

		virtual parametrizationType getParametrizationType() const
		{
			return NONE;
		}

		virtual void print() const
		{
			std::cout << *this->state_ptr_ << " " << *(this->state_ptr_+1) << std::endl;
		}
};

class StateThetaAngle: public StateBase
{
	public:
		static const unsigned int BLOCK_SIZE = 1;

		StateThetaAngle(VectorXs& _st_remote, const unsigned int _idx) :
			StateBase(_st_remote, _idx)
		{
		}

		StateThetaAngle(WolfScalar* _st_ptr) :
			StateBase(_st_ptr)
		{
		}

		virtual ~StateThetaAngle()
		{
		}

		virtual parametrizationType getParametrizationType() const
		{
			return NONE;
		}

		virtual void print() const
		{
			std::cout << *this->state_ptr_ << std::endl;
		}
};

class StateComplexAngle: public StateBase
{
	public:
		static const unsigned int BLOCK_SIZE = 2;

		StateComplexAngle(VectorXs& _st_remote, const unsigned int _idx) :
			StateBase(_st_remote, _idx)
		{
		}

		StateComplexAngle(WolfScalar* _st_ptr) :
			StateBase(_st_ptr)
		{
		}

		virtual ~StateComplexAngle()
		{
		}

		virtual parametrizationType getParametrizationType() const
		{
			return COMPLEX_ANGLE;
		}

		virtual void print() const
		{
			std::cout << *this->state_ptr_ << " " << *(this->state_ptr_+1) << std::endl;
		}
};

class ComplexAngleParameterization : public ceres::LocalParameterization
{
	public:
		virtual ~ComplexAngleParameterization()
		{
		}

		virtual bool Plus(const double* x_raw, const double* delta_raw, double* x_plus_delta_raw) const
		{
			x_plus_delta_raw[0] = x_raw[0] * cos(delta_raw[0]) - x_raw[1] * sin(delta_raw[0]);
			x_plus_delta_raw[1] = x_raw[1] * cos(delta_raw[0]) + x_raw[0] * sin(delta_raw[0]);

			//normalize
			//double norm = sqrt(x_plus_delta_raw[0] * x_plus_delta_raw[0] + x_plus_delta_raw[1] * x_plus_delta_raw[1]);
			//std::cout << "(before normalization) norm = " << norm << std::endl;
			//x_plus_delta_raw[0] /= norm;
			//x_plus_delta_raw[1] /= norm;

			return true;
		}

		virtual bool ComputeJacobian(const double* x, double* jacobian) const
		{
			jacobian[0] = -x[1];
			jacobian[1] =  x[0];
			return true;
		}

		virtual int GlobalSize() const
		{
			return 2;
		}

		virtual int LocalSize() const
		{
			return 1;
		}
};

class CorrespondenceBase
{
	protected:
		WolfScalar *measurement_ptr_;

    public:

        CorrespondenceBase(WolfScalar * _measurement_ptr) :
        	measurement_ptr_(_measurement_ptr)
        {
        }

        virtual ~CorrespondenceBase()
        {
        }

        virtual correspondenceType getType() const = 0;
        virtual const std::vector<WolfScalar*> getBlockPtrVector() = 0;
};

template <const unsigned int MEASUREMENT_SIZE,
				unsigned int BLOCK_0_SIZE,
				unsigned int BLOCK_1_SIZE = 0,
				unsigned int BLOCK_2_SIZE = 0,
				unsigned int BLOCK_3_SIZE = 0,
				unsigned int BLOCK_4_SIZE = 0,
				unsigned int BLOCK_5_SIZE = 0,
				unsigned int BLOCK_6_SIZE = 0,
				unsigned int BLOCK_7_SIZE = 0,
				unsigned int BLOCK_8_SIZE = 0,
				unsigned int BLOCK_9_SIZE = 0>
class CorrespondenceSparse: public CorrespondenceBase
{
    protected:
		std::vector<WolfScalar*> state_block_ptr_vector_;
		std::vector<unsigned int> block_sizes_vector_;

    public:
		static const unsigned int measurementSize = MEASUREMENT_SIZE;
		static const unsigned int block0Size = BLOCK_0_SIZE;
		static const unsigned int block1Size = BLOCK_1_SIZE;
		static const unsigned int block2Size = BLOCK_2_SIZE;
		static const unsigned int block3Size = BLOCK_3_SIZE;
		static const unsigned int block4Size = BLOCK_4_SIZE;
		static const unsigned int block5Size = BLOCK_5_SIZE;
		static const unsigned int block6Size = BLOCK_6_SIZE;
		static const unsigned int block7Size = BLOCK_7_SIZE;
		static const unsigned int block8Size = BLOCK_8_SIZE;
		static const unsigned int block9Size = BLOCK_9_SIZE;

		CorrespondenceSparse(WolfScalar* _measurementPtr, WolfScalar** _blockPtrArray) :
        	CorrespondenceBase(_measurementPtr),
			block_sizes_vector_({BLOCK_0_SIZE,
								 BLOCK_1_SIZE,
								 BLOCK_2_SIZE,
								 BLOCK_3_SIZE,
								 BLOCK_4_SIZE,
								 BLOCK_5_SIZE,
								 BLOCK_6_SIZE,
								 BLOCK_7_SIZE,
								 BLOCK_8_SIZE,
								 BLOCK_9_SIZE})
        {
			for (uint i = 0; i<block_sizes_vector_.size(); i++)
			{
				if (block_sizes_vector_.at(i) == 0)
				{
					block_sizes_vector_.resize(i);
					break;
				}
				else
					state_block_ptr_vector_.push_back(_blockPtrArray[i]);
			}
        }

		CorrespondenceSparse(WolfScalar* _measurementPtr,
							 WolfScalar* _state0Ptr,
							 WolfScalar* _state1Ptr = nullptr,
							 WolfScalar* _state2Ptr = nullptr,
							 WolfScalar* _state3Ptr = nullptr,
							 WolfScalar* _state4Ptr = nullptr,
							 WolfScalar* _state5Ptr = nullptr,
							 WolfScalar* _state6Ptr = nullptr,
							 WolfScalar* _state7Ptr = nullptr,
							 WolfScalar* _state8Ptr = nullptr,
							 WolfScalar* _state9Ptr = nullptr ) :
			CorrespondenceBase(_measurementPtr),
			state_block_ptr_vector_({_state0Ptr,
									 _state1Ptr,
									 _state2Ptr,
									 _state3Ptr,
									 _state4Ptr,
									 _state5Ptr,
									 _state6Ptr,
									 _state7Ptr,
									 _state8Ptr,
									 _state9Ptr}),
			block_sizes_vector_({BLOCK_0_SIZE,
								 BLOCK_1_SIZE,
								 BLOCK_2_SIZE,
								 BLOCK_3_SIZE,
								 BLOCK_4_SIZE,
								 BLOCK_5_SIZE,
								 BLOCK_6_SIZE,
								 BLOCK_7_SIZE,
								 BLOCK_8_SIZE,
								 BLOCK_9_SIZE})
		{
			for (uint i = 0; i<block_sizes_vector_.size(); i++)
			{
				if (block_sizes_vector_.at(i) == 0)
				{
					block_sizes_vector_.resize(i);
					state_block_ptr_vector_.resize(i);
					break;
				}
			}

			//TODO: Check if while size OK, pointers NULL
		}

        virtual ~CorrespondenceSparse()
        {
        }

        virtual correspondenceType getType() const = 0;

		virtual const std::vector<WolfScalar *> getBlockPtrVector()
		{
			return state_block_ptr_vector_;
		}
};

class CorrespondenceGPS2D : public CorrespondenceSparse<2,2>
{
	public:
		static const unsigned int N_BLOCKS = 1;
		const double stdev_ = 1;

		CorrespondenceGPS2D(WolfScalar* _measurementPtr, WolfScalar* _statePtr) :
			CorrespondenceSparse<2,2>(_measurementPtr, _statePtr)
		{
		}

		CorrespondenceGPS2D(WolfScalar* _measurementPtr, StateShPtr _statePtr) :
			CorrespondenceSparse<2,2>(_measurementPtr, _statePtr->getPtr())
		{
		}

		virtual ~CorrespondenceGPS2D()
		{
		}

		template <typename T>
		bool operator()(const T* const _x, T* _residuals) const
		{
			_residuals[0] = (T(*this->measurement_ptr_) - _x[0]) / T(stdev_);
			_residuals[1] = (T(*(this->measurement_ptr_+1)) - _x[1]) / T(stdev_);

			return true;
		}

		virtual correspondenceType getType() const
		{
			return GPS_2D;
		}
};

class Correspondence2DOdometry : public CorrespondenceSparse<2,2,2,2,2>
{
	public:
		static const unsigned int N_BLOCKS = 4;
		const double stdev_ = 0.01; //model parameters

		Correspondence2DOdometry(WolfScalar* _measurementPtr, WolfScalar** _blockPtrs) :
			CorrespondenceSparse<2,2,2,2,2>(_measurementPtr, _blockPtrs)
		{
		}

		Correspondence2DOdometry(WolfScalar* _measurementPtr, WolfScalar* _block0Ptr, WolfScalar* _block1Ptr, WolfScalar* _block2Ptr, WolfScalar* _block3Ptr) :
			CorrespondenceSparse<2,2,2,2,2>(_measurementPtr, _block0Ptr, _block1Ptr, _block2Ptr, _block3Ptr)
		{
		}

		Correspondence2DOdometry(WolfScalar* _measurementPtr, StateShPtr _state0Ptr, StateShPtr _state1Ptr, StateShPtr _state2Ptr, StateShPtr _state3Ptr) :
			CorrespondenceSparse<2,2,2,2,2>(_measurementPtr, _state0Ptr->getPtr(), _state1Ptr->getPtr(),_state2Ptr->getPtr(), _state3Ptr->getPtr())
		{
		}

		virtual ~Correspondence2DOdometry()
		{
		}

        template <typename T>
        bool operator()(const T* const _p1, const T* const _o1, const T* const _p2, const T* const _o2, T* _residuals) const
        {
			// Expected measurement
			T expected_range = (_p1[0]-_p2[0])*(_p1[0]-_p2[0]) + (_p1[1]-_p2[1])*(_p1[1]-_p2[1]); //square of the range
			T expected_rotation = atan2(-_o1[0]*_o2[1] + _o1[1]*_o2[0], _o1[0]*_o2[0] + _o1[1]*_o2[1]);

			// Residuals
			_residuals[0] = (expected_range - T((*this->measurement_ptr_)*(*this->measurement_ptr_))) / T(stdev_);
			_residuals[1] = (expected_rotation - T(*(this->measurement_ptr_+1))) / T(stdev_);

			return true;
        }

        virtual correspondenceType getType() const
        {
        	return ODOM_2D_COMPLEX;
        }
};

class Correspondence2DOdometryTheta : public CorrespondenceSparse<2,2,1,2,1>
{
	public:
		static const unsigned int N_BLOCKS = 4;
		const double stdev_ = 0.01; //model parameters

		Correspondence2DOdometryTheta(WolfScalar* _measurementPtr, WolfScalar** _blockPtrs) :
			CorrespondenceSparse<2,2,1,2,1>(_measurementPtr, _blockPtrs)
		{
		}

		Correspondence2DOdometryTheta(WolfScalar* _measurementPtr, WolfScalar* _block0Ptr, WolfScalar* _block1Ptr, WolfScalar* _block2Ptr, WolfScalar* _block3Ptr) :
			CorrespondenceSparse<2,2,1,2,1>(_measurementPtr, _block0Ptr, _block1Ptr, _block2Ptr, _block3Ptr)
		{
		}

		Correspondence2DOdometryTheta(WolfScalar* _measurementPtr, const StateShPtr& _state0Ptr, StateShPtr _state1Ptr, StateShPtr _state2Ptr, StateShPtr _state3Ptr) :
			CorrespondenceSparse<2,2,1,2,1>(_measurementPtr, _state0Ptr->getPtr(), _state1Ptr->getPtr(),_state2Ptr->getPtr(), _state3Ptr->getPtr())
		{
		}

		virtual ~Correspondence2DOdometryTheta()
		{
		}

        template <typename T>
        bool operator()(const T* const _p1, const T* const _o1, const T* const _p2, const T* const _o2, T* _residuals) const
        {
			// Expected measurement
			T expected_range = (_p2[0]-_p1[0])*(_p2[0]-_p1[0]) + (_p2[1]-_p1[1])*(_p2[1]-_p1[1]); //square of the range
			T expected_rotation = _o2[0]-_o1[0];

			// Residuals
			_residuals[0] = (expected_range - T((*this->measurement_ptr_)*(*this->measurement_ptr_))) / T(stdev_);
			_residuals[1] = (expected_rotation - T(*(this->measurement_ptr_+1))) / T(stdev_);

			return true;
        }

        virtual correspondenceType getType() const
        {
        	return ODOM_2D_THETA;
        }
};

class WolfManager
{
    protected:
        std::vector<StateShPtr> state_units_;
        std::vector<CorrespondenceShPtr> correspondences_;

    public: 
        WolfManager() :
			state_units_(0),
			correspondences_(0)
		{
		}

        virtual ~WolfManager()
        {
        }

        unsigned int getCorrespondencesSize()
        {
        	return correspondences_.size();
        }

        unsigned int addCorrespondence(const CorrespondenceShPtr& _corr_ptr)
        {
        	correspondences_.push_back(_corr_ptr);
        	return correspondences_.size()-1;
        }

        CorrespondenceShPtr getCorrespondencePrt(unsigned int i)
        {
        	return correspondences_.at(i);
        }

        unsigned int addStateUnit(const StateShPtr& _st_ptr)
        {
        	//std::cout << "Adding state unit to the wolf list..." << std::endl;
        	state_units_.push_back(_st_ptr);
        	//std::cout << "Added!" << std::endl;
        	return state_units_.size()-1;
        }

        StateShPtr getStateUnitPtr(unsigned int i)
		{
			return state_units_.at(i);
		}
};

class CeresManager
{
	protected:

		std::vector<std::pair<ceres::ResidualBlockId, CorrespondenceShPtr>> correspondence_list_;
		ceres::Problem ceres_problem_;

	public:
		CeresManager()
		{
		}

		~CeresManager()
		{
		}

		ceres::Solver::Summary solve(const ceres::Solver::Options& _ceres_options)
		{
			// create summary
			ceres::Solver::Summary ceres_summary_;

			// run Ceres Solver
			ceres::Solve(_ceres_options, &ceres_problem_, &ceres_summary_);

			//display results
			return ceres_summary_;
		}

		void addCorrespondence(const CorrespondenceShPtr& _corr_ptr)
		{
			ceres::ResidualBlockId blockIdx = ceres_problem_.AddResidualBlock(createCostFunction(_corr_ptr), NULL, _corr_ptr->getBlockPtrVector());
			correspondence_list_.push_back(std::pair<ceres::ResidualBlockId, CorrespondenceShPtr>(blockIdx,_corr_ptr));
		}

		void addStateUnit(const StateShPtr& _st_ptr)
		{
			//std::cout << "Adding a State Unit to wolf_problem... " << std::endl;
			//_st_ptr->print();

			switch (_st_ptr->getParametrizationType())
			{
				case COMPLEX_ANGLE:
				{
					//std::cout << "Adding Complex angle Local Parametrization to the List... " << std::endl;
					ceres_problem_.SetParameterization(_st_ptr->getPtr(), new ComplexAngleParameterization);
					//ceres_problem_.AddParameterBlock(_st_ptr->getPtr(), ((StateComplexAngle*)_st_ptr)->BLOCK_SIZE, new ComplexAngleParameterization);
					break;
				}
//				case QUATERNION:
//				{
//					std::cout << "Adding Quaternion Local Parametrization to the List... " << std::endl;
//					ceres_problem_.SetParameterization(_st_ptr->getPtr(), new EigenQuaternionParameterization);
//					break;
//				}
				case NONE:
				{
					//std::cout << "No Local Parametrization to be added" << std::endl;
					break;
				}
				default:
					std::cout << "Unknown  Local Parametrization type!" << std::endl;
			}
		}

		ceres::CostFunction* createCostFunction(const CorrespondenceShPtr& _corrPtr)
		{
			switch (_corrPtr->getType())
			{
				case GPS_2D:
				{
					CorrespondenceGPS2D* specific_ptr = (CorrespondenceGPS2D*)(_corrPtr.get());
					return new ceres::AutoDiffCostFunction<CorrespondenceGPS2D,
															specific_ptr->measurementSize,
															specific_ptr->block0Size,
															specific_ptr->block1Size,
															specific_ptr->block2Size,
															specific_ptr->block3Size,
															specific_ptr->block4Size,
															specific_ptr->block5Size,
															specific_ptr->block6Size,
															specific_ptr->block7Size,
															specific_ptr->block8Size,
															specific_ptr->block9Size>(specific_ptr);
					break;
				}
				case ODOM_2D_COMPLEX:
				{
					Correspondence2DOdometry* specific_ptr = (Correspondence2DOdometry*)(_corrPtr.get());
					return new ceres::AutoDiffCostFunction<Correspondence2DOdometry,
															specific_ptr->measurementSize,
															specific_ptr->block0Size,
															specific_ptr->block1Size,
															specific_ptr->block2Size,
															specific_ptr->block3Size,
															specific_ptr->block4Size,
															specific_ptr->block5Size,
															specific_ptr->block6Size,
															specific_ptr->block7Size,
															specific_ptr->block8Size,
															specific_ptr->block9Size>(specific_ptr);
					break;
				}
				case ODOM_2D_THETA:
				{
					Correspondence2DOdometryTheta* specific_ptr = (Correspondence2DOdometryTheta*)(_corrPtr.get());
					return new ceres::AutoDiffCostFunction<Correspondence2DOdometryTheta,
															specific_ptr->measurementSize,
															specific_ptr->block0Size,
															specific_ptr->block1Size,
															specific_ptr->block2Size,
															specific_ptr->block3Size,
															specific_ptr->block4Size,
															specific_ptr->block5Size,
															specific_ptr->block6Size,
															specific_ptr->block7Size,
															specific_ptr->block8Size,
															specific_ptr->block9Size>(specific_ptr);
					break;
				}
				default:
					std::cout << "Unknown correspondence type! Please add it in the CeresWrapper::createCostFunction()" << std::endl;

					return NULL;
			}
		}
};

int main(int argc, char** argv) 
{
	clock_t t1, t2;
	t1=clock();

    std::cout << " ========= 2D Robot with odometry and GPS ===========" << std::endl << std::endl;
    
    //user input
	if (argc!=4)
	{
		std::cout << "Please call me with: [./test_ceres_wrapper_states NI PRINT ORIENTATION_MODE], where:" << std::endl;
		std::cout << "     - NI is the number of iterations" << std::endl;
		std::cout << "     - PRINT = 1 for print results" << std::endl;
		std::cout << "     - ORIENTATION_MODE: 0 for theta, 1 for complex angle" << std::endl;
		std::cout << "EXIT due to bad user input" << std::endl << std::endl;
		return -1;
	}

	unsigned int n_execution = (unsigned int) atoi(argv[1]); //number of iterations of the whole execution
	bool print = (bool) atoi(argv[2]);
	bool complex_angle = (bool) atoi(argv[3]);

	//init google log
	google::InitGoogleLogging(argv[0]);

	// wolf problem
	WolfManager* wolf_problem = new WolfManager;

	//variables
	int dim = (complex_angle ? 4 : 3);
	Eigen::VectorXs odom_inc_true(n_execution*2);//invented motion
	Eigen::VectorXs pose_true(dim); //current true pose
	Eigen::VectorXs ground_truth(n_execution*dim); //all true poses
	Eigen::VectorXs pose_predicted(dim); // current predicted pose
	Eigen::VectorXs predicted_trajectory(n_execution*dim); // current predicted pose
	Eigen::VectorXs state(n_execution*dim); //running window winth solver result
	Eigen::VectorXs odom_readings(n_execution*2); // all odometry readings
	Eigen::VectorXs gps_fix_readings(n_execution*3); //all GPS fix readings
	int id_p, id_o, id_p_prev, id_o_prev;

	//init true odom and true pose
	for (unsigned int ii = 0; ii<n_execution; ii++)
	{
		if ( ii < (unsigned int)floor(n_execution/2) )
			odom_inc_true.segment(ii*2,2) << fabs(cos(ii/10.)) , fabs(sin(ii/2000.)); //invented motion increments.
		else
			odom_inc_true.segment(ii*2,2) << fabs(cos(ii/10.)) , -fabs(sin((ii-floor(n_execution/2))/2000.)); //invented motion increments.
	}
	if (complex_angle)
	{
		pose_true << 0,0,1,0;
		pose_predicted << 0,0,1,0;
	}
	else
	{
		pose_true << 0,0,0;
		pose_predicted << 0,0,0;
	}
	ground_truth.head(dim) << pose_true; //init point pushed to ground truth
	state.head(dim) << pose_predicted; //init state at origin
	predicted_trajectory.head(dim) << pose_predicted;

	//init random generators
	std::default_random_engine generator(1);
	std::normal_distribution<WolfScalar> distribution_odom(0.001,0.01); //odometry noise
	std::normal_distribution<WolfScalar> distribution_gps(0.0,1); //GPS noise

    // TODO: incorporar weights a les funcions residu (via LossFunction o directament a operador())

    // Ceres problem initialization
    ceres::Solver::Options ceres_options;
    ceres_options.minimizer_type = ceres::LINE_SEARCH;//ceres::TRUST_REGION;
    ceres_options.max_line_search_step_contraction = 1e-3;
//    ceres_options.minimizer_progress_to_stdout = false;
//    ceres_options.line_search_direction_type = ceres::LBFGS;
//    ceres_options.max_num_iterations = 2;
    CeresManager ceres_manager;
    std::ofstream log_file;  //output file

	// Start trajectory
    id_p = wolf_problem->addStateUnit(new StatePoint2D(state.data()));
	ceres_manager.addStateUnit(wolf_problem->getStateUnitPtr(id_p));
	if (complex_angle)
	{
	    id_o = wolf_problem->addStateUnit(new StateComplexAngle(state.data()+2));
		ceres_manager.addStateUnit(wolf_problem->getStateUnitPtr(id_o));
	}
	else
	{
	    id_o = wolf_problem->addStateUnit(new StateThetaAngle(state.data()+2));
		ceres_manager.addStateUnit(wolf_problem->getStateUnitPtr(id_o));
	}

	for (uint step=1; step < n_execution; step++)
	{
		// NEW STATE
		//inventing a simple motion
		if (complex_angle)
		{
			double new_pose_true_2 = pose_true(2) * cos(odom_inc_true(step*2+1)) - pose_true(3) * sin(odom_inc_true(step*2+1));
			double new_pose_true_3 = pose_true(2) * sin(odom_inc_true(step*2+1)) + pose_true(3) * cos(odom_inc_true(step*2+1));
			pose_true(0) = pose_true(0) + odom_inc_true(step*2) * new_pose_true_2;
			pose_true(1) = pose_true(1) + odom_inc_true(step*2) * new_pose_true_3;
			pose_true(2) = new_pose_true_2;
			pose_true(3) = new_pose_true_3;
		}
		else
		{
			double new_pose_true_2 = pose_true(2) + (odom_inc_true(step*2+1));
			pose_true(0) = pose_true(0) + odom_inc_true(step*2) * cos(new_pose_true_2);
			pose_true(1) = pose_true(1) + odom_inc_true(step*2) * sin(new_pose_true_2);
			pose_true(2) = new_pose_true_2;
		}

		//inventing sensor readings for odometry and GPS
		odom_readings.segment(step*2,2) << odom_inc_true(step*2)+distribution_odom(generator), odom_inc_true(step*2+1)+distribution_odom(generator); //true range and theta with noise
		gps_fix_readings.segment(step*3,3) << pose_true(0) + distribution_gps(generator), pose_true(1) + distribution_gps(generator), 0. + distribution_gps(generator);

		//setting initial guess as an odometry prediction, using noisy odometry
		if (complex_angle)
		{
			double new_pose_predicted_2 = pose_predicted(2) * cos(odom_readings(step*2+1)) - pose_predicted(3) * sin(odom_readings(step*2+1));
			double new_pose_predicted_3 = pose_predicted(2) * sin(odom_readings(step*2+1)) + pose_predicted(3) * cos(odom_readings(step*2+1));
			pose_predicted(0) = pose_predicted(0) + odom_readings(step*2) * new_pose_predicted_2;
			pose_predicted(1) = pose_predicted(1) + odom_readings(step*2) * new_pose_predicted_3;
			pose_predicted(2) = new_pose_predicted_2;
			pose_predicted(3) = new_pose_predicted_3;
		}
		else
		{
			double new_pose_predicted_2 = pose_predicted(2) + (odom_readings(step*2+1));
			pose_predicted(0) = pose_predicted(0) + odom_readings(step*2) * cos(new_pose_predicted_2);
			pose_predicted(1) = pose_predicted(1) + odom_readings(step*2) * sin(new_pose_predicted_2);
			pose_predicted(2) = new_pose_predicted_2;
		}
		predicted_trajectory.segment(step*dim,dim) = pose_predicted;

		// store
		state.segment(step*dim,dim) << pose_predicted;
		ground_truth.segment(step*dim,dim) << pose_true;

		// STATE UNITS
		id_p_prev = id_p;
		id_o_prev = id_o;
		// p
		id_p = wolf_problem->addStateUnit(StateShPtr(new StatePoint2D(state.data() + step * dim)));
		ceres_manager.addStateUnit(wolf_problem->getStateUnitPtr(id_p));
		// o
		if (complex_angle)
		{
			id_o = wolf_problem->addStateUnit(StateShPtr(new StateComplexAngle(state.data() + step * dim + 2)));
			ceres_manager.addStateUnit(wolf_problem->getStateUnitPtr(id_o));
		}
		else
		{
			id_o = wolf_problem->addStateUnit(StateShPtr(new StateThetaAngle(state.data() + step * dim + 2)));
			ceres_manager.addStateUnit(wolf_problem->getStateUnitPtr(id_o));
		}

		// CORRESPONDENCE ODOMETRY
		if (complex_angle)
		{

			// int odomIdx = wolf_manager.addOdomMeasurement(odom_readings.data() + step*2);
			// Correspondence2DOdometry* odomCorrPtr = wolf_manager.getCorrespondence(odomIdx);
			// ceres_wrapper.addCorrespondence<Correspondence2DOdometry>(odomCorrPtr, odomCorrPtr->getP1(), odomCorrPtr->getO1(), odomCorrPtr->getP2(), odomCorrPtr->getO2());

			uint id_corr = wolf_problem->addCorrespondence(new Correspondence2DOdometry(odom_readings.data() + step*2,
																					wolf_problem->getStateUnitPtr(id_p_prev),
																					wolf_problem->getStateUnitPtr(id_o_prev),
																					wolf_problem->getStateUnitPtr(id_p),
																					wolf_problem->getStateUnitPtr(id_o)));
			ceres_manager.addCorrespondence(wolf_problem->getCorrespondencePrt(id_corr));
		}
		else
		{
			uint id_corr = wolf_problem->addCorrespondence(new Correspondence2DOdometryTheta(odom_readings.data() + step*2,
																						 wolf_problem->getStateUnitPtr(id_p_prev),
																						 wolf_problem->getStateUnitPtr(id_o_prev),
																						 wolf_problem->getStateUnitPtr(id_p),
																						 wolf_problem->getStateUnitPtr(id_o)));

			ceres_manager.addCorrespondence(wolf_problem->getCorrespondencePrt(id_corr));
		}

		// CORRESPONDENCE GPS (2D)
		uint id_corr = wolf_problem->addCorrespondence(new CorrespondenceGPS2D(gps_fix_readings.data() + step*3, wolf_problem->getStateUnitPtr(id_p)));
		ceres_manager.addCorrespondence(wolf_problem->getCorrespondencePrt(id_corr));

		// SOLVE CERES PROBLEM
		//ceres::Solver::Summary summary = ceres_wrapper.solve();
		if (print)
		{
			std::cout << " ========= STEP " << step << "===========" << std::endl << std::endl;
			std::cout << "odom : " << odom_inc_true.segment(step*2,2).transpose() << std::endl << std::endl;
			std::cout << "pose_predicted : " << pose_predicted.transpose() << std::endl << std::endl;
			std::cout << "pose_true : " << pose_true.transpose() << std::endl << std::endl;
			std::cout << "gps measurement : " << gps_fix_readings.segment(step*3,3).transpose() << std::endl << std::endl;
			std::cout << "state : " << std::endl << state.head(step*dim).transpose() << std::endl;
			std::cout << "ground_truth : " << std::endl << ground_truth.head(step*dim).transpose() << std::endl;
			std::cout << "error : " << std::endl << (ground_truth.head(step*dim) - state.head(step*dim)).transpose() << std::endl << std::endl;
			//std::cout << "total time (s):" << summary.total_time_in_seconds << std::endl;
		}
	}
    
	ceres::Solver::Summary summary = ceres_manager.solve(ceres_options);
	std::cout << summary.FullReport() << std::endl;

	t2=clock();
	double seconds = ((double)t2-t1)/CLOCKS_PER_SEC;
	std::cout << "optimization seconds: " << summary.total_time_in_seconds << std::endl;
	std::cout << "total seconds: " << seconds << std::endl;

	//display/log results, by setting cout flags properly
	VectorXs state_theta(n_execution * 3);
	VectorXs ground_truth_theta(n_execution * 3);
	VectorXs predicted_trajectory_theta(n_execution * 3);
	if (complex_angle)
	{
		// change from complex angle to theta
		for (uint ii = 0; ii<n_execution; ii++)
		{
			state_theta.segment(ii*3,3) << state(ii*4), state(ii*4+1), atan2(state(ii*4+2), state(ii*4+3));
			ground_truth_theta.segment(ii*3,3) << ground_truth(ii*4), ground_truth(ii*4+1), atan2(ground_truth(ii*4+2), ground_truth(ii*4+3));
			predicted_trajectory_theta.segment(ii*3,3) << predicted_trajectory(ii*4), predicted_trajectory(ii*4+1), atan2(predicted_trajectory(ii*4+2), predicted_trajectory(ii*4+3));
		}
	}
	else
	{
		state_theta = state;
		ground_truth_theta = ground_truth;
		predicted_trajectory_theta = predicted_trajectory;
	}
	std::string homepath = getenv("HOME");
	log_file.open(homepath + "/Desktop/log_file_2.txt", std::ofstream::out); //open log file
	if (log_file.is_open())
	{
		log_file << seconds << std::endl;
		for (unsigned int ii = 0; ii<n_execution; ii++)
			log_file << state_theta.segment(ii*3,3).transpose()
					 << " " << ground_truth_theta.segment(ii*3,3).transpose()
					 << " " << (state_theta.segment(ii*3,3)-ground_truth_theta.segment(ii*3,3)).transpose()
					 << " " << gps_fix_readings.segment(ii*3,3).transpose()
					 << " " << predicted_trajectory_theta.segment(ii*3,3).transpose() << std::endl;
		log_file.close(); //close log file
		std::cout << std::endl << " Result file ~/Desktop/log_data.txt" << std::endl;
	}
	else
		std::cout << std::endl << " Failed to write the file ~/Desktop/log_data.txt" << std::endl;
    //end Wolf iteration
    std::cout << " ========= END ===========" << std::endl << std::endl;
       
    //exit
    return 0;
}

