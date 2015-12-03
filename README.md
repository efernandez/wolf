WOLF - Windowed Localization Frames
===================================

Overview
--------

Wolf is a library to solve localization problems in mobile robotics, such as SLAM, map-based localization, or visual odometry. The approach contemplates the coexistence of multiple sensors of different kind, be them synchronized or not. It is thought to build state vectors formed by a set of key-frames (window), defining the robot trajectory, plus other states such as landmarks or sensor parameters for calibration, and compute error vectors given the available measurements in that window.

Wolf is mainly a structure for having the data accessible and organized, plus some functionality for managing this data. It requires, on one side, several front-ends (one per sensor, or per sensor type), and a back-end constituted by the solver.

Wolf may be interfaced with many kinds of solvers, including filters and nonlinear optimizers. It can be used with EKF, error-state KF, iterative EKF, and other kinds of filters such as information filters. It can also be used with nonlinear optimizers, most especially -- though not necessarily -- in their sparse flavors, and in particular the incremental ones.

The task of interfacing Wolf with these solvers is relegated to wrappers, which are coded out of Wolf. We provide a wrapper to Google CERES. We also provide an experimental wrapper-less solver for incremental, sparse, nonlinear optimization based on the QR decomposition (implementing the iSAM algorithm).

The basic Wolf structure is a tree of base classes reproducing the elements of the robotic problem. A robot, with sensors, with a trajectory formed by keyframes, and the map with its landmarks. These base classes can be derived to build the particularizations you want. You have the basic functionality in the base classes, and you add what you want on top. The tree connectivity is augmented with the constraints linking different parts of it, becoming a real network of relations. This network is equivalent to the factor graph that would be solved by graphical models and nonlinear optimization. Wrappers are the ones transferring the Wolf structure into a factor graph that can be provided to the solver. See the documentation for a proper rationale and all the details.

Wolf can be used within ROS for an easy integration. We provide examples of ROS nodes using Wolf. Wolf can also be used in other robotics frameworks.

### Features

-   Keyframe based
-   Multi-sensor
-   SLAM or visual odometry
-   With or without landmarks
-   Different state types -- the state blocks
-   Different measurement models -- the constraint blocks
-   Polymorphic classes using virtual inheritance and templates.

### Some preliminary documentation

-   You can visit this [Wolf inspiring document](https://docs.google.com/document/d/1_kBtvCIo33pdP59M3Ib4iEBleDDLcN6yCbmwJDBLtcA). Contact [Joan](mailto:jsola@iri.upc.edu) if you need permissions for the link.
-   You can also have a look at the [Wolf tree](https://docs.google.com/drawings/d/1jj5VVjQThddswpTPMLG2xv87vtT3o1jiMJo3Mk1Utjg), showing the organization of the main elements in the Wolf project. Contact [Andreu](mailto:acorominas@iri.upc.edu) if you need permissions for the link.
-   You can finally visit this [other inspiring document](https://docs.google.com/document/d/18XQlgdfTwplakYKKsfw2YAoaVuSyUEQoaU6bYHQDNpU) providing the initial motivation for the Wolf project. Contact [Joan](mailto:jsola@iri.upc.edu) if you need permissions for the link.

Dependencies
------------

### Eigen

[Eigen](http://eigen.tuxfamily.org). Linear algebra, header library. Eigen 3.2 is also a depencency of ROS-Hydro. In case you don't have ROS in your machine, you can install Eigen by typing:

    `$ sudo apt-get install libeigen3-dev`

### Ceres (5 steps)

[Ceres](http://www.ceres-solver.org/) is an optimization library. Currently, this dependency is optional, so the build procedure of Wolf skips part of compilation in case this dependency is not found on the system. **Installation** is desctibed at [Ceres site](http://www.ceres-solver.org/building.html). However we report here an alternative step by step procedure to install Ceres.

**(1)** Skip this step if Cmake 2.8.0+ and Eigen3.0+ are already installed. Otherwise install them with *apt-get*.

**(2) GFLAGS**

-   Git clone the source

    `$ git clone `[`https://github.com/gflags/gflags.git`](https://github.com/gflags/gflags.git)

-   Build and install with:

    `$ cd gflags`
    `$ mkdir build`
    `$ cd build`
    `$ cmake -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC" -DGFLAGS_NAMESPACE="google" ..`
    `$ make`
    `$ sudo make install `

libgflags.a will be installed at **/usr/local/lib**

**(3) GLOG**

-   Download a tar-gz-ball from [here](https://code.google.com/p/google-glog/), download section.
-   Uncompress it with:

    `$ tar -xvzf glog-0.3.3.tar.gz `

-   Build and install with:

    `$ cd glog-0.3.3`
    `$ ./configure --with-gflags=/usr/local/`
    `$ make`
    `$ sudo make install`

libglog.so will be installed at **/usr/local/lib**

**(4) SUITESPARSE**

-   Easy way!:

    `$ sudo apt-get install libsuitesparse-dev`

**(5) CERES**

-   Get the tar-gz-ball corresponding to the latest stable release from [here](http://www.ceres-solver.org/building.html).
-   Uncompress it with:

    `$ tar -xvzf ceres-solver-1.10.0.tar.gz`

-   Build and install with:

    `$ cd ceres-solver-1.10.0`
    `$ mkdir build`
    `$ cd build`
    `$ cmake -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC" ..`
    `$ make`
    `$ sudo make install `

libceres.a will be installed at **/usr/local/lib** and headers at **/usr/local/include/ceres**

### Laser Scan Utils (Optional. Install only if you want to use IRI's laser scan utils)

**(1)** Download:

    `$ svn checkout `[`https://devel.iri.upc.edu/labrobotica/algorithms/laser_scan_utils/tags/v1`](https://devel.iri.upc.edu/labrobotica/algorithms/laser_scan_utils/tags/v1)` laser_scan_utils`

Or, in case you don't have permissions:

    `$ svn checkout `[`https://devel.iri.upc.edu/pub/labrobotica/algorithms/laser_scan_utils/tags/v1`](https://devel.iri.upc.edu/pub/labrobotica/algorithms/laser_scan_utils/tags/v1)` laser_scan_utils`

**(2)** Build and install:

    `$ cd laser_scan_utils/build`
    `$ cmake ..`
    `$ make`
    `$ sudo make install`

Download and build
------------------

### Wolf C++ Library

**Download:**

    `$ git clone `[`https://github.com/IRI-MobileRobotics/Wolf.git`](https://github.com/IRI-MobileRobotics/Wolf.git)

**Build:**

    `$ cd Wolf/build`
    `$ cmake ..`
    `$ make`
    `$ sudo make install  //optional in case you want to install wolf library`

### Wolf ROS Node

    `$ git clone `[`https://github.com/IRI-MobileRobotics/Wolf_ros.git`](https://github.com/IRI-MobileRobotics/Wolf_ros.git)

Inspiring Links
---------------

-   [Basics on code optimization](http://www.eventhelix.com/realtimemantra/basics/optimizingcandcppcode.htm)

-   [Headers, Includes, Forward declarations, ...](http://www.cplusplus.com/forum/articles/10627/)

-   Using Eigen quaternion and CERES: [explanation](http://www.lloydhughes.co.za/index.php/using-eigen-quaternions-and-ceres-solver/) & [GitHub CERES extension](https://github.com/system123/ceres_extensions)

Useful tools
------------

### Profiling with Valgrind and Kcachegrind

Kcachegrind is a graphical frontend for profiling your program and optimizing your code.

#### Install in Ubuntu

Get the programs with

    `   sudo apt-get install valgrind kcachegrind`

#### Install in Mac OSX

In Mac, you can use qcachegrind instead. To get it through Homebrew, type

    `   brew install valgrind qcachegrind`

I don't know if these packages are available through MacPorts. Try

    `   ports search --name valgrind`
    `   ports search --name qcachegrind`

If they are available, just do

    `   sudo port install valgrind qcachegrind`

#### Do the profiling and watch the reports

Type in your `wolf/bin/` directory:

    `   cd bin/`
    `   valgrind --tool=callgrind ./my_program `<my_prg_params>

this produces a log report called `callgrind.out.XXXX`, where XXXX is a number. Then type (Ubuntu)

    `   kcachegrind callgrind.out.XXXX`

or (Mac)

    `   qcachegrind callgrind.out.XXXX`

and enjoy.
