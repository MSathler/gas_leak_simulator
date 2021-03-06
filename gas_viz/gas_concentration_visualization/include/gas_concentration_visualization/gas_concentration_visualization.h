/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c)  2011, Örebro University, Sweden
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.

 *Authors:
 *********************************************************************/


#include "ros/ros.h"
#include "std_msgs/Float32.h"

#include "nav_msgs/Odometry.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
//#include <cv_bridge/CvBridge.h>

#include <boost/thread/mutex.hpp>
#include <boost/math/constants/constants.hpp>
#include <visualization_msgs/MarkerArray.h>
#include "gas_concentration_visualization/gas_map.h"

//#include <msgs_and_srvs/SensorPosition2.h>
//#include "msgs_and_srvs/SensorPosition2.h"


#define NODE_VERSION		3

using namespace std;

//constants
//display width (pixels)
#define DISPLAY_WIDTH 250
//display height (pixels)
#define DISPLAY_HEIGHT 150

//double MAX_CONCENTRATION=0;
//double MAX_CONCENTRATION=0;


//nose displacement along robot vertical axis (m)
#define NOSE_POSITION_U 2.0
//nose displacement along robot horizontal axis (m)
#define NOSE_POSITION_V 3.0
//Number of grids to be displayed (x-axis)
#define DISPLAY_GRIDS_COLS 8
//Number of grids to be displayed (7-axis)
#define DISPLAY_GRIDS_ROWS 5

//-------------------------------------------------------
//	Default values
//-------------------------------------------------------
#define	DEFAULT_FRAME_ID		"/map"

//#define	DEFAULT_SENSOR_TOPIC		"/mox_sensor"
#define	DEFAULT_MAP_MAX_X		61
#define	DEFAULT_MAP_MIN_X		0
#define	DEFAULT_MAP_MAX_Y		21
#define	DEFAULT_MAP_MIN_Y		0
#define	DEFAULT_MAP_MAX_Z		6
#define	DEFAULT_MAP_MIN_Z		0

#define	DEFAULT_CELL_SIZE		1
#define	DEFAULT_COLORMAP		"jet"
#define	DEFAULT_SENSOR_OFFSET_X		2
#define	DEFAULT_SENSOR_OFFSET_Y		2
#define	DEFAULT_SENSOR_OFFSET_Z		2
#define	DEFAULT_MAX_SENSOR_VAL		0
#define	DEFAULT_MIN_SENSOR_VAL		0
#define	DEFAULT_N_POINTS_MAP		2000
//#define	DEFAULT_PUBLISH_HZ			100
#define DEFAULT_SNAPSHOTS			200

#define	NODE_NAME 	"gas_concentration_visualization"
#define	DEFAULT_RESULT_LOCATION					"/home/han/gdm_simulator_catkin_ws/src/simulated_tdlas/recordedData/results_ethanol/gasType1_simulation"

#define DEFAULT_TEMPERATURE                 298 //K
#define DEFAULT_PRESSURE                    1   // atm
#define DEFAULT_UNIT_CHOICE                 0 // 0 for #molecules per cm³ and 1 for ppm

//global variables
// const double PI = boost::math::constants::pi<double>();
//const float sigma_omega = 1/(2*PI*pow(kernel_size,2));
/*   to move
  cv::Mat weights;
  cv::Mat confidence;
  cv::Mat weightedReadings;
  cv::Mat weightedVariance;
  cv::Mat meanMap;
  cv::Mat varianceMap;
  cv::Mat kernelCoefficients;
  cv::Mat robotTrajectory;
 */
//boost::mutex mutex_nose;
//boost::mutex mutex_position;
//bool new_data_nose = false;
//bool new_data_position = false;

std::string	frame_id;
//std::string position_topic;
std::string colormap;



double map_min_x;
double map_max_x;
double map_min_y;
double map_max_y;
double map_min_z;
double map_max_z;
double cell_size;

int num_snapshots;
int	n_points_map;

double max_sensor_val;
double min_sensor_val;

double sensor_offset_x=0;
double sensor_offset_y=0;
double sensor_offset_z=0;

std::string input_result_location;

double      input_temperature;
double      input_pressure;
int         input_unit_choice;

//int		publish_hz;

float curr_x = 0;
float curr_y = 0;
float curr_z = 0;
float curr_reading = 0;

struct sCellConcentration {
	double ***concentraion_value;
};


double*** arr3dAlloc(const int ind1, const int ind2, const int ind3);

double concentrationRecorded(float, float,  float, float, double, double, int);
void 		loadNodeParameters(ros::NodeHandle private_nh);

////for color maps in rviz.
//void publishmarker(float x, float y,float size,float r,float g,float b);
//void color_map(std::string colormap, int val);
//ros::Publisher pub_marker_mean;
//, pub_marker_var;
//visualization_msgs::Marker draw;
geometry_msgs::Point p;
//std::string COLORMAP="summer";
double colr=0;
double colg=0;
double colb=0;
