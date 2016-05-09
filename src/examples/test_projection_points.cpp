#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//std includes
#include <iostream>

//wolf includes
#include "pinholeTools.h"


int main(int argc, char** argv)
{
    std::cout << std::endl << " ========= ProjectPoints test ===========" << std::endl << std::endl;

    cv::Point3f points3D;
    points3D.x = 2.0;
    points3D.y = 5.0;
    points3D.z = 6.0;
    std::vector<cv::Point3f> point_in_3D;
    point_in_3D.push_back(points3D);
    points3D.x = 4.0;
    points3D.y = 2.0;
    points3D.z = 1.0;
    point_in_3D.push_back(points3D);

    std::vector<float> rot_mat = {0,0,0};
    std::vector<float> trans_mat = {1,1,1};

    cv::Mat cam_mat(3,3,CV_32F);
    cam_mat.row(0).col(0).setTo(1);
    cam_mat.row(0).col(1).setTo(0);
    cam_mat.row(0).col(2).setTo(2);
    cam_mat.row(1).col(0).setTo(0);
    cam_mat.row(1).col(1).setTo(1);
    cam_mat.row(1).col(2).setTo(2);
    cam_mat.row(2).col(0).setTo(0);
    cam_mat.row(2).col(1).setTo(0);
    cam_mat.row(2).col(2).setTo(1);

    std::cout << "cam_mat[1,2]: " << cam_mat.row(1).col(0) << std::endl;

    std::vector<float> dist_coef = {0,0,0,0,0};
    std::vector<cv::Point2f> points2D;
    cv::projectPoints(point_in_3D,rot_mat,trans_mat,cam_mat,dist_coef,points2D);

    for (auto it : points2D)
    {
        std::cout << "points2D- X: " << it.x << "; Y: " << it.y << std::endl;
    }

    std::cout << std::endl << " ========= PinholeTools test ===========" << std::endl << std::endl;

    //================================= ProjectPoint

    Eigen::Vector3s test_p;
    test_p[0] = 3;
    test_p[1] = 3;
    test_p[2] = 3;
    Eigen::Vector2s output;
    output = pinhole::projectPointToNormalizedPlane(test_p);

    std::cout << "TEST projectPointToNormalizedPlane; Vector2s" << std::endl;
    std::cout << "x: " << output[0] << "; y: " << output[1] << "; rows: " << output.rows() << "; cols: " << output.cols() << std::endl;


    Eigen::Vector2s output2;
    double dist;
    pinhole::projectPointToNormalizedPlane(test_p,output2,dist);

    std::cout << std::endl << "TEST projectPointToNormalizedPlane; distance" << std::endl;
    std::cout << "x: " << output2[0] << "; y: " << output2[1] << "; rows: " << output2.rows() << "; cols: " << output2.cols()
              << "; dist: " << dist << std::endl;


    Eigen::Vector2s output3;
    Eigen::Matrix3s jacobian;
    pinhole::projectPointToNormalizedPlane(test_p,output3,jacobian);

    std::cout << std::endl << "TEST projectPointToNormalizedPlane; jacobian" << std::endl;
    std::cout << "x: " << output3[0] << "; y: " << output3[1] << "; rows: " << output3.rows() << "; cols: " << output3.cols() << std::endl;
    std::cout << "Jacobian" << std::endl <<
                 jacobian.row(0).col(0) << " " << jacobian.row(0).col(1) << " " << jacobian.row(0).col(2) << " " << std::endl <<
                 jacobian.row(1).col(0) << " " << jacobian.row(1).col(1) << " " << jacobian.row(1).col(2) << " " << std::endl <<
                 jacobian.row(2).col(0) << " " << jacobian.row(2).col(1) << " " << jacobian.row(2).col(2) << " " << std::endl;


    Eigen::Vector2s output4;
    Eigen::Matrix3s jacobian2;
    double dist2;
    pinhole::projectPointToNormalizedPlane(test_p,output4,dist2,jacobian2);

    std::cout << std::endl << "TEST projectPointToNormalizedPlane; jacobian and distance" << std::endl;
    std::cout << "x: " << output4[0] << "; y: " << output4[1] << "; rows: " << output4.rows() << "; cols: " << output4.cols()
                 << "; dist: " << dist2 << std::endl;
    std::cout << "Jacobian" << std::endl <<
                 jacobian2.row(0).col(0) << " " << jacobian2.row(0).col(1) << " " << jacobian2.row(0).col(2) << " " << std::endl <<
                 jacobian2.row(1).col(0) << " " << jacobian2.row(1).col(1) << " " << jacobian2.row(1).col(2) << " " << std::endl <<
                 jacobian2.row(2).col(0) << " " << jacobian2.row(2).col(1) << " " << jacobian2.row(2).col(2) << " " << std::endl;

    //================================= BackprojectPoint

    Eigen::Vector2s back_proj_vector_test;
    back_proj_vector_test[0] = 1;
    back_proj_vector_test[1] = 1;
    double depth = 3;

    Eigen::Vector3s back_proj_output;
    back_proj_output = pinhole::backprojectPointFromNormalizedPlane(back_proj_vector_test,depth);

    std::cout << std::endl << "TEST backprojectPointToNormalizedPlane; Eigen::Vector3s" << std::endl;
    std::cout << "x: " << back_proj_output[0] << "; y: " << back_proj_output[1] << "; z: " << back_proj_output[2] << std::endl;


    Eigen::Vector3s back_proj_output2;
    Eigen::Matrix3s jacobian3;
    Eigen::Vector3s jacobian_depth;
    pinhole::backprojectPointFromNormalizedPlane(back_proj_vector_test,depth,back_proj_output2,jacobian3,jacobian_depth);

    std::cout << std::endl << "TEST backprojectPointToNormalizedPlane; jacobians" << std::endl;
    std::cout << "x: " << back_proj_output2[0] << "; y: " << back_proj_output2[1] << "; z: " << back_proj_output2[2] << std::endl;
    std::cout << "Jacobian" << std::endl <<
                 jacobian3.row(0).col(0) << " " << jacobian3.row(0).col(1) << " " << jacobian3.row(0).col(2) << " " << std::endl <<
                 jacobian3.row(1).col(0) << " " << jacobian3.row(1).col(1) << " " << jacobian3.row(1).col(2) << " " << std::endl <<
                 jacobian3.row(2).col(0) << " " << jacobian3.row(2).col(1) << " " << jacobian3.row(2).col(2) << " " << std::endl;
    std::cout << "Jacobian - depth" << std::endl <<
                 jacobian_depth[0] << " " << jacobian_depth[1] << " " << jacobian_depth[2] << " " << std::endl;

    //================================= PixelizePoint

    Eigen::Vector4f k;
    k[0] = 0.5;
    k[1] = 0.4;
    k[2] = 1.3;
    k[3] = 1.2;
    Eigen::Vector2s ud;
    ud[0] = 50;
    ud[1] = 40;

    Eigen::Vector2s pixelize_output;
    pixelize_output = pinhole::pixellizePoint(k,ud);

    std::cout << std::endl << "TEST pixelizePoint; Eigen::Vector2s" << std::endl;
    std::cout << "x: " << pixelize_output[0] << "; y: " << pixelize_output[1] << std::endl;


    Eigen::Vector2s pixelize_output2;
    Eigen::Matrix2s pixelize_jacobian;
    pinhole::pixellizePoint(k,ud,pixelize_output2,pixelize_jacobian);

    std::cout << std::endl << "TEST pixelizePoint; Jacobians" << std::endl;
    std::cout << "x: " << pixelize_output2[0] << "; y: " << pixelize_output2[1] << std::endl;
    std::cout << "Jacobian" << std::endl <<
                 pixelize_jacobian.row(0).col(0) << " " << pixelize_jacobian.row(0).col(1) << std::endl <<
                 pixelize_jacobian.row(1).col(0) << " " << pixelize_jacobian.row(1).col(1) << std::endl;

    //================================= DepixelizePoint

    Eigen::Vector2s u;
    ud[0] = 65;
    ud[1] = 48;

    Eigen::Vector2s depixelize_output;
    depixelize_output = pinhole::depixellizePoint(k,u);

    std::cout << std::endl << "TEST depixelizePoint; Eigen::Vector2s" << std::endl;
    std::cout << "x: " << depixelize_output[0] << "; y: " << depixelize_output[1] << std::endl;


    Eigen::Vector2s depixelize_output2;
    Eigen::Matrix2s depixelize_jacobian;
    pinhole::depixellizePoint(k,u,depixelize_output2,depixelize_jacobian);

    std::cout << std::endl << "TEST depixelizePoint; Jacobians" << std::endl;
    std::cout << "x: " << depixelize_output2[0] << "; y: " << depixelize_output2[1] << std::endl;
    std::cout << "Jacobian" << std::endl <<
                 depixelize_jacobian.row(0).col(0) << " " << depixelize_jacobian.row(0).col(1) << std::endl <<
                 depixelize_jacobian.row(1).col(0) << " " << depixelize_jacobian.row(1).col(1) << std::endl;


    //================================= IsInRoi / IsInImage

    Eigen::Vector2s pix;
    pix[0] = 40;
    pix[1] = 40;
    int roi_x = 30;
    int roi_y = 30;
    int roi_width = 20;
    int roi_height = 20;

    bool is_in_roi;
    is_in_roi = pinhole::isInRoi(pix,roi_x,roi_y,roi_width,roi_height);

    std::cout << std::endl << "TEST isInRoi" << std::endl;
    std::cout << "is_in_roi: " << is_in_roi << std::endl;

    int image_width = 640;
    int image_height = 480;

    bool is_in_image;
    is_in_image = pinhole::isInImage(pix,image_width,image_height);

    std::cout << std::endl << "TEST isInImage" << std::endl;
    std::cout << "is_in_image: " << is_in_image << std::endl;


    //================================= distortPoint

    Eigen::Vector2s distortion;
    distortion[0] = 1;
    distortion[1] = 0.8;
    Eigen::Vector2s dis_point;
    dis_point[0] = 40;
    dis_point[1] = 40;

    Eigen::Vector2s distored_point;
    distored_point = pinhole::distortPoint(distortion,dis_point);

    std::cout << std::endl << "TEST distortPoint" << std::endl;
    std::cout << "x: " << distored_point[0] << "; y: " << distored_point[1] << std::endl;



    Eigen::Vector2s distored_point2;
    Eigen::Matrix2s distortion_jacobian;
    pinhole::distortPoint(distortion,dis_point,distored_point2,distortion_jacobian);

    std::cout << std::endl << "TEST distortPoint" << std::endl;
    std::cout << "x: " << distored_point2[0] << "; y: " << distored_point2[1] << std::endl;
//    std::cout << "Jacobian" << std::endl <<
//                 distortion_jacobian.row(0).col(0) << " " << distortion_jacobian.row(0).col(1) << std::endl <<
//                 distortion_jacobian.row(1).col(0) << " " << distortion_jacobian.row(1).col(1) << std::endl;
}







