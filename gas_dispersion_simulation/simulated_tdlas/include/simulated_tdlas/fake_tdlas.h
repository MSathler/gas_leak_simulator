#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <visualization_msgs/Marker.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>

#include <tf/transform_listener.h>
#include "msgs_and_srvs/SensorPosition2.h"


#include <cstdlib>
//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include "msgs_and_srvs/ChemicalValue.h"
#include "msgs_and_srvs/readings.h"


#define	ETHANOL_ID		0
#define	METHANE_ID		1
#define	HYDROGEN_ID		2
#define	PROPANOL_ID		3
#define	CHLORIDE_ID		4
#define	FLURORINE_ID	5
#define	ACETONE_ID		6
#define	NEON_ID			7
#define	HELIUM_ID		9
#define	HOTAIR_ID		10

#define	TGS2620_ID		0
#define	TGS2600_ID		1
#define	TGS2611_ID		2

#define S_NO 3
#define sim_time 100000 //ms/s..

# define PI           3.14159265358979323846  /* pi */



#define		NODE_NAME 							"fake_tdlas"
#define		DEFAULT_GAS_TYPE					METHANE_ID
#define		DEFAULT_SENSOR_NAME					"tdlas_sensor"
#define		DEFAULT_SENSOR_MODEL				TGS2620_ID				
#define		DEFAULT_SENSOR_FRAME				"tdlas_frame"
#define		DEFAULT_FIXED_FRAME					"map"
#define	    DEFAULT_RESULT_LOCATION				"/home/itv/espeleo_ws/src/gas_leak_simulator/gas_dispersion_simulation/recordedData/TDLAS_sensor/" // you can also set the beginnings of the file names here.
#define		DEFAULT_STARTING_ANGLE				0*(PI /4)
#define		DEFAULT_ENDING_ANGLE				1*(PI /10)
#define		DEFAULT_INCIDENCE_ANGLE	            8.5*(PI/18)
#define	    DEFAULT_BEAM_LENGTH                 15
#define     DEFAULT_ACTIVATION_TIME             119

#define	DEFAULT_ENVIRONMENT_LOCATION		"/home/itv/DATA-forHan/corridor4/environment.env"
#define	DEFAULT_AREA_X						100
#define	DEFAULT_AREA_Y						37
#define	DEFAULT_AREA_Z						7
#define	DEFAULT_AREA_CELL_SIZE				1


using namespace std;

int ch_id;
geometry_msgs::PoseWithCovarianceStamped amcl_pos;

//display sensor marker:

msgs_and_srvs::readings response;

//Service request for the value i.e hit_rate.value[][][]
msgs_and_srvs::SensorPosition2 srv;
//msgs_and_srvs::SensorPosition srv;

int sensor0;
int sensor1;
int sensor2;
int robot_0_sensor_model;
int robot_1_sensor_model;
int robot_2_sensor_model;

float sensor0_log[sim_time];
float sensor1_log[sim_time];
float sensor2_log[sim_time];
float tau=12.4*0.04;
int   counter[S_NO];
float y_out=0;
float filtered_resp[S_NO];
float servive_val[S_NO]={0};
bool initial = true;
float a;
float cons[3][3][4]={
		//constants: A, B, C, D			
		{  //TGS2620
				{6.018,-0.01662, 0.9832,-0.0005651},  //ethanol
				{18.79,-0.01062, 6.138, -0.0002136 },  //methane
				{3.884 ,-0.0127,0.8745,-0.0003222 }, //hydrogen
		},

		{  //TGS2600
				{0.4668,-0.3249, 0.3293, -0.01051 },  //ethanol
				{0.2202, -0.1122, 0.8356, -0.001932},  //methane
				{0.0,0.0,0.0,0.0},  //hydrogen
		},

		{  //TGS2611
				{4.766,-0.001639, 3.497, -7.348e-05 },  //ethanol
				{3.286 ,-0.002211, 1.806 -0.000103},  //methane
				{4.535, -0.001723, 2.69, -5.191e-05},  //hydrogen
		}
};

int internal_time_step;
int input_activation_time;

double		input_area_cell_size;
int			input_area_size_x;
int 		input_area_size_y;
int 		input_area_size_z;

std::string input_environment_location;

int             input_beam_length;
double          input_starting_angle;
double          input_ending_angle;
double          input_incidence_angle;

int				input_gas_type;
//std::string		input_sensor_name;
int				input_sensor_model;
std::string		input_sensor_frame;
std::string		input_fixed_frame;
std::string input_result_location;


// Environment structure

struct sSensingEnvironment {
	struct {
		int width;
		int height;
		int length;
	} arena;

	struct {
		struct {
			double x1;
			double y1;
			double x2;
			double y2;
		} item[20000];
		int count;
	} lines;

	struct {
		struct {
			double x1;
			double y1;
			double x2;
			double y2;
		} item[20000];
		int count;
	} rectangles;
} sensingEnvironment;


//functions:
float sensor_response  (float concentration, int ch_id, int robot_id, int sensor_model,float* sensor);
void  amclCallback     (const geometry_msgs::PoseWithCovarianceStamped& msg);
void  chemicalCallback (const msgs_and_srvs::ChemicalValue::ConstPtr& msg);
void  writeConcentration(float pos_xx, float pos_yy, float pos_zz, float end_xx, float end_yy, float end_zz, float concentrationValue, const char *);
void  writeTofile       (msgs_and_srvs::readings response);
void environment_obstacle_init(struct sSensingEnvironment *sensingEnvironment,int,int,int, double, const char *filePath);
void environment_obstacles_add(struct sSensingEnvironment *sensingEnvironment, double x1, double y1, double x2, double y2);
// Returns whether a point (world coordinates) is valid or not.
int environment_obstacle_valid(struct sSensingEnvironment *sensingEnvironment, double x, double y, double z);
// END - GENERATED BY C-UPDATE-HEADER
void  loadNodeParameters(ros::NodeHandle private_nh);
