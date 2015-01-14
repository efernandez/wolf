// Testing a full wolf tree avoiding template classes for NodeLinked derived classes

//std includes
#include <iostream>
#include <vector>
#include <random>
// #include <memory>
// #include <typeinfo>

//ceres
#include "ceres/ceres.h"

//Wolf includes
#include "wolf.h"
#include "node_terminus.h"
#include "node_linked.h"

//forward declarations
class TrajectoryBaseX;
class FrameBaseX;
class CaptureBaseX;
class FeatureBaseX;
class CorrespondenceBaseX;

//class TrajectoryBaseX
class TrajectoryBaseX : public NodeLinked<NodeTerminus,FrameBaseX>
{
    protected:
        unsigned int length_; //just something to play
        
    public:
        TrajectoryBaseX(const unsigned int _len) :
            NodeLinked(TOP, "TRAJECTORY"),
            length_(_len)
        {
            //
        };
        
        virtual ~TrajectoryBaseX()
        {
            
        };
};

//class FrameBaseX
class FrameBaseX : public NodeLinked<TrajectoryBaseX,CaptureBaseX>
{
    protected:
        double time_stamp_; //frame ts
        
    public:
        FrameBaseX(double _ts) :
            NodeLinked(MID, "FRAME"),
            time_stamp_(_ts)
        {
            //
        };
        
        virtual ~FrameBaseX()
        {
            
        };
};

//class CaptureBaseX
class CaptureBaseX : public NodeLinked<FrameBaseX,FeatureBaseX>
{
    protected:
        double time_stamp_; //capture ts
        
    public:
        CaptureBaseX(double _ts) :
            NodeLinked(MID, "CAPTURE"),
            time_stamp_(_ts)
        {
            //
        };
        
        virtual ~CaptureBaseX()
        {
            
        };
};

//class FeatureBaseX
class FeatureBaseX : public NodeLinked<CaptureBaseX,CorrespondenceBaseX>
{
    protected:
        
    public:
        FeatureBaseX() :
            NodeLinked(MID, "FEATURE")
        {
            //
        };
        
        virtual ~FeatureBaseX()
        {
            //
        };
};

//class CorrespondenceBaseX
class CorrespondenceBaseX : public NodeLinked<FeatureBaseX,NodeTerminus>
{
    protected:
        unsigned int nblocks_; //number of state blocks in which the correspondence depends on.
        std::vector<unsigned int> block_indexes_; //state vector indexes indicating start of each state block. This vector has nblocks_ size. 
        std::vector<unsigned int> block_sizes_; //sizes of each state block. This vector has nblocks_ size. 
        ceres::CostFunction* cost_function_ptr_;
        
    public:
        CorrespondenceBaseX(const unsigned int _nb, const std::vector<unsigned int> & _bindexes, const std::vector<unsigned int> & _bsizes) :
            NodeLinked(BOTTOM, "CORRESPONDENCE"),
            nblocks_(_nb),
            block_indexes_(_bindexes),
            block_sizes_(_bsizes)
        {
            assert(block_sizes_.size() == nblocks_);
        };
        
        virtual ~CorrespondenceBaseX()
        {
            //
        };
        
        ceres::CostFunction * getCostFunctionPtr()
        {
            return cost_function_ptr_;
        };
        	
        virtual void display() const
        {
                unsigned int ii; 
                std::cout << "number of blocks: " << nblocks_ << std::endl;
                std::cout << "block indexes: ";
                for (ii=0; ii<block_indexes_.size(); ii++) std::cout << block_indexes_.at(ii) << " ";
                std::cout << std::endl;
                std::cout << "block sizes: ";
                for (ii=0; ii<block_sizes_.size(); ii++) std::cout << block_sizes_.at(ii) << " ";
                std::cout << std::endl;
        };
};

class Odom2DFunctor
{
    protected:
        Eigen::Vector2s odom_inc_; //incremental odometry measurements (range, theta). Could be a map to data hold by capture or feature
    
    public:
        //constructor
        Odom2DFunctor(const Eigen::Vector2s & _odom):
            odom_inc_(_odom) 
        {
            //
        };
        
        //destructor
        virtual ~Odom2DFunctor()
        {
            //
        };
        
        //cost function
        template <typename T>
        bool operator()(const T* const _x0, const T* const _x1, T* _residual) const
        {
            T dr2, dth;
            
            //expected range and theta increments, given the state points
            dr2 = (_x0[0]-_x1[0])*(_x0[0]-_x1[0]) + (_x0[1]-_x1[1])*(_x0[1]-_x1[1]); //square of the range
            dth = _x1[2] - _x0[2]; //
            
            //residuals in range and theta components 
            _residual[0] = T(dr2) - T(odom_inc_(0)*odom_inc_(0));
            _residual[1] = T(dth) - T(odom_inc_(1));
            
            //return 
            return true;
        };
};

//Specialized correspondence class for 2D odometry
class CorrespondenceOdom2D : public CorrespondenceBaseX
{
    protected:
        Eigen::Map<Eigen::Vector3s> pose_previous_;
        Eigen::Map<Eigen::Vector3s> pose_current_; 
        Eigen::Map<const Eigen::Vector2s> odom_inc_; 
        
    public:
        CorrespondenceOdom2D(WolfScalar * _st, const Eigen::Vector2s & _odom) :
            CorrespondenceBaseX(2,{0,3},{3,3}),
            pose_previous_(_st + block_indexes_.at(0), block_sizes_.at(0)),
            pose_current_(_st + block_indexes_.at(1), block_sizes_.at(1)),
            odom_inc_(_odom.data())
        {
            cost_function_ptr_ = new ceres::AutoDiffCostFunction<Odom2DFunctor,2,3,3>(new Odom2DFunctor(_odom));
        };
        
        virtual ~CorrespondenceOdom2D()
        {
            //delete cost_function_ptr_;
        };
                
        double * getPosePreviousPtr()
        {
            return pose_previous_.data();
        };
        
        double * getPoseCurrentPtr()
        {
            return pose_current_.data();
        };        
        
        virtual void display() const
        {
            std::cout << "---- Odom Correspondence ----" << std::endl;
            CorrespondenceBaseX::display();
            std::cout << "pose_previous_: " << pose_previous_.transpose() << std::endl;
            std::cout << "pose_current_: " << pose_current_.transpose() << std::endl;
            std::cout << "odom_inc_: " << odom_inc_.transpose() << std::endl;
        };      
};

class GPSFixFunctor
{
    protected:
        Eigen::Vector3s gps_fix_; //GPS fix XYZ. Could be a map to data hold by capture or feature
    
    public:
        //constructor
        GPSFixFunctor(const Eigen::Vector3s & _gps_fix):
            gps_fix_(_gps_fix)
        {
            //
        };
        
        //destructor
        virtual ~GPSFixFunctor()
        {
            //
        };
        
        //cost function
        template <typename T>
        bool operator()(const T* const _x0, T* _residual) const
        {
            _residual[0] = T( gps_fix_(0) ) - _x0[0];
            _residual[1] = T( gps_fix_(1) ) - _x0[1];
            _residual[2] = T(0.); //T( gps_fix_(2) ) - _x0[2]; //in this case, third component of the state is heading, not Z, so it is ignored
            return true;
        };
};

//Specialized correspondence class for GPS Fix data
class CorrespondenceGPSFix : public CorrespondenceBaseX
{
    protected:
        Eigen::Map<Eigen::Vector3s> location_;
        Eigen::Map<const Eigen::Vector3s> gps_fix_;

    public:
        CorrespondenceGPSFix(WolfScalar * _st, const Eigen::Vector3s & _gps_fix) :
            CorrespondenceBaseX(1,{0},{3}), 
            location_(_st + block_indexes_.at(0) , block_sizes_.at(0)),
            gps_fix_(_gps_fix.data())
        {
            cost_function_ptr_ = new ceres::AutoDiffCostFunction<GPSFixFunctor,3,3>(new GPSFixFunctor(_gps_fix));
        };
        
        virtual ~CorrespondenceGPSFix()
        {
            //delete cost_function_ptr_;
        };
        
        double * getLocation()
        {
            return location_.data();
        }
                
        virtual void display() const
        {
            std::cout << "---- GPS Fix Correspondence ----" << std::endl;
            CorrespondenceBaseX::display();
            std::cout << "location_: " << location_.transpose() << std::endl;
            std::cout << "gps_fix_: " << gps_fix_.transpose() << std::endl;            
        };        
};


int main(int argc, char** argv) 
{    
    //Welcome message
    std::cout << " ========= WOLF-CERES test. Simple Odometry + GPS fix problem (with non-template classes) ===========" << std::endl << std::endl;
    
    //init google log
    google::InitGoogleLogging(argv[0]);

    //variables
    Eigen::VectorXs odom_inc_true(20);//invented motion
    Eigen::Vector3s pose_true; //current true pose
    Eigen::VectorXs ground_truth(30); //accumulated true poses
    Eigen::Vector3s pose_predicted; // current predicted pose
    Eigen::VectorXs state(30); //accumulated predicted poses
    Eigen::Vector2s odom_reading; //current odometry reading
    Eigen::Vector3s gps_fix_reading; //current GPS fix reading
    CorrespondenceOdom2D *odom_corresp; //pointer to odometry correspondence
    CorrespondenceGPSFix *gps_fix_corresp; //pointer to GPS fix correspondence
    ceres::Problem problem; //ceres problem 
    ceres::Solver::Options options; //ceres solver options
    ceres::Solver::Summary summary; //ceres solver summary

    //init true odom and true pose
    odom_inc_true << 0.2,0, 0.3,0.1, 0.3,0.2, 0.3,0, 0.4,0.1, 0.3,0.1, 0.2,0., 0.1,0.1, 0.1,0., 0.05,0.05;
    pose_true << 0,0,0;
    pose_predicted << 0,0,0;
    
    //random generators
    std::default_random_engine generator;
    std::normal_distribution<WolfScalar> distribution_odom(0.0,0.01); //odometry noise
    std::normal_distribution<WolfScalar> distribution_gps(0.0,0.02); //GPS noise
        
    //test loop
    for (unsigned int ii = 0; ii<10; ii++)
    {
        //inventing a simple motion
        pose_true(0) = pose_true(0) + odom_inc_true(ii*2) * cos(pose_true(2)+odom_inc_true(ii*2+1)); 
        pose_true(1) = pose_true(1) + odom_inc_true(ii*2) * sin(pose_true(2)+odom_inc_true(ii*2+1)); 
        pose_true(2) = pose_true(2) + odom_inc_true(ii*2+1);
        ground_truth.middleRows(ii*3,3) << pose_true;
        
        //inventing sensor readings for odometry and GPS
        odom_reading << odom_inc_true(ii*2)+distribution_odom(generator), odom_inc_true(ii*2+1)+distribution_odom(generator); //true range and theta with noise
        gps_fix_reading << pose_true(0) + distribution_gps(generator), pose_true(1) + distribution_gps(generator), 0. + distribution_gps(generator);
        std::cout << "pose_true(" << ii << ") = " << pose_true.transpose() << std::endl;
        
        //setting initial guess as an odometry prediction, using noisy odometry
        pose_predicted(0) = pose_predicted(0) + odom_reading(0) * cos(pose_predicted(2)+odom_reading(1)); 
        pose_predicted(1) = pose_predicted(1) + odom_reading(0) * sin(pose_predicted(2)+odom_reading(1)); 
        pose_predicted(2) = pose_predicted(2) + odom_reading(1);
        state.middleRows(ii*3,3) << pose_predicted;
        
        //creating odom correspondence, exceptuating first iteration. Adding it to the problem 
        if ( ii !=0 )
        {
            odom_corresp = new CorrespondenceOdom2D(state.data()+(ii-1)*3, odom_reading);
            //odom_corresp->display();
            problem.AddResidualBlock(odom_corresp->getCostFunctionPtr(),nullptr, odom_corresp->getPosePreviousPtr(), odom_corresp->getPoseCurrentPtr());
            delete odom_corresp;
        }
        
        //creating gps correspondence and adding it to the problem 
        gps_fix_corresp = new CorrespondenceGPSFix(state.data()+ii*3, gps_fix_reading);
        //gps_fix_corresp->display();
        problem.AddResidualBlock(gps_fix_corresp->getCostFunctionPtr(),nullptr, gps_fix_corresp->getLocation());
        delete gps_fix_corresp;
    }
    
    //display initial guess
    std::cout << "INITIAL GUESS IS: " << state.transpose() << std::endl;
    
    //set options and solve
    options.minimizer_progress_to_stdout = true;
    ceres::Solve(options, &problem, &summary);
    
    //display results
    std::cout << "RESULT IS: " << state.transpose() << std::endl;
    std::cout << "GROUND TRUTH IS: " << ground_truth.transpose() << std::endl;
    std::cout << "ERROR IS: " << (state-ground_truth).transpose() << std::endl;        
  
    //free memory (not necessary since ceres::problem holds their ownership)
//     delete odom_corresp;
//     delete gps_fix_corresp;
    
    //End message
    std::cout << " =========================== END ===============================" << std::endl << std::endl;
       
    //exit
    return 0;
}
