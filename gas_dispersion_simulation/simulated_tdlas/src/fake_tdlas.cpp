#include "simulated_tdlas/fake_tdlas.h"

#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/ChannelFloat32.h>
#include <ros/ros.h>

void chatterCallback(const std_msgs::Float32::ConstPtr& msg) {

	ROS_INFO("I heard: [%f]", msg->data);

	internal_time_step = int(msg->data);
}

//=======================================================
int main(int argc, char** argv)
//=======================================================
		{

	//ros::init(argc, argv, "fake_tdlas");
	ros::init(argc, argv, NODE_NAME);
	ros::NodeHandle n;
	loadNodeParameters(n);
	ros::Rate r(0.15);

	ros::Subscriber sub = n.subscribe("/snapshot_teller", 10, chatterCallback);

	std::string topic_name;

	ros::Publisher sensor_read_pub = n.advertise<std_msgs::Float32>(
			"Sensor_reading", 500);
	ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>(
			"Sensor_display", 100);

	ros::Publisher line_pub = n.advertise<visualization_msgs::Marker>(
			"visualization_marker", 100);

	//ros::Publisher cloud_pub = n.advertise<sensor_msgs::PointCloud>("cloud", 50);

	ros::ServiceClient client = n.serviceClient<msgs_and_srvs::SensorPosition2>(
			"/odor_value");

	std::ifstream stream(
			"/home/itv/Desktop/gas_dispersion/GasSim/data/windData/environments/original.env",
			std::ifstream::binary);

	tf::TransformListener listener;
	//sensor_msgs::PointCloud cloud;

	visualization_msgs::Marker line, points;

	line.header.frame_id = input_fixed_frame.c_str();
	line.ns = "sensor_visualization";
	line.action = visualization_msgs::Marker::ADD;
	line.type = visualization_msgs::Marker::LINE_STRIP;
	line.id = 2;

	line.scale.x = 0.5;

	line.color.g = 1.0f;
	line.color.a = 1.0;

	points.header.frame_id = input_fixed_frame.c_str();
	points.ns = "sensor_visualization";
	points.action = visualization_msgs::Marker::ADD;
	points.type = visualization_msgs::Marker::POINTS;
	points.id = 3;

	points.scale.x = 0.03;
	points.scale.y = 0.03;

	points.color.r = 0.294118f;
	points.color.b = 0.509804f;
	points.color.a = 1.0;

	std: string format = ".txt";

	environment_obstacle_init(&sensingEnvironment, input_area_size_x,
			input_area_size_y, input_area_size_z, input_area_cell_size,
			input_environment_location.c_str());

	ros::Time before = ros::Time::now();

	while (ros::ok()) {

		if (internal_time_step == input_activation_time) {

			std_msgs::Float32 read_msg;
			read_msg.data = 0;
			tf::StampedTransform transform;

			try {
				listener.waitForTransform(input_fixed_frame.c_str(),
						input_sensor_frame.c_str(), ros::Time(0),
						ros::Duration(5));
				listener.lookupTransform(input_fixed_frame.c_str(),
						input_sensor_frame.c_str(), ros::Time(0), transform);
			} catch (tf::TransformException ex) {
				ROS_ERROR("%s", ex.what());
				ros::Duration(1).sleep();
			}

			float x_pos = transform.getOrigin().x();
			float y_pos = transform.getOrigin().y();
			float z_pos = transform.getOrigin().z();

			line.header.stamp = ros::Time::now();
			points.header.stamp = ros::Time::now();

			char file_name[256];
			strncpy(file_name, input_result_location.c_str(),
					sizeof(file_name));
			strncat(file_name, input_sensor_frame.c_str(), sizeof(file_name));

			//starting angle
			float azimuthal_start = input_starting_angle;
			//ending angle
			float azimuthal_end = input_ending_angle;

			//angle of incidence
			float polar = input_incidence_angle;

			float concentration_read = 0;
			double totalConcentration = 0;

			// double VirtualSensorResponse2_TDLAS2D(float laser_length, float azimuthal_start, float azimuthal_end, int num_azimuthal, float original_X, float original_Y,  environment, cell_size)

			// azimuthal_stepSize = (azimuthal_end - azimuthal_start)/num_azimuthal;
			/*for double azimuthal = azimuthal_start:azimuthal_stepSize:azimuthal_end
			 int hit=0; // this variable indicates the obstacles.
			 internalIndex = 1;
			 endingPoint_x = original_X + cos(azimuthal)*laser_length;
			 endingPoint_y = original_Y + sin(azimuthal)*laser_length;


			 float startingPoint_x;
			 float startingPoint_y;

			 if(original_X>endingPoint_x){
			 startingPoint_x = endingPoint_x;
			 endingPoint_x = original_X;
			 }else{
			 startingPoint_x = original_X;
			 }

			 if(original_Y>endingPoint_y){
			 startingPoint_y = endingPoint_y;
			 endingPoint_y = original_Y;
			 }else{
			 startingPoint_x = original_X;
			 }

			 if(currentPosition_x+cell_size<endingPoint_x){
			 currentPosition_x =  currentPosition_x + cell_size;

			 current_segment = cell
			 }



			 */

			std::cout << "NEW ROUND OF MEASUREMENT......" << std::endl;

			for (double azimuthal = azimuthal_start; azimuthal < azimuthal_end;
					azimuthal += (azimuthal_end - azimuthal_start) / 180) {

				//std::cout << "in the middle......" << std::endl;

				int hit = 0;

				//std::cout << "New beam is generated......" << std::endl;

				double resolution = input_area_cell_size / 20;

				int length = input_beam_length / input_area_cell_size;
				int s = 0;

				double finalConcentration = 0;

				float curX = x_pos;
				float curY = y_pos;
				float curZ = z_pos;

				while (s < length && hit == 0) {

					int temp_m = 0;

					for (int m = 0; m < 20 && hit == 0; m++) {
						/*
						 if (curX > input_area_size_x || curX < 0) {
						 hit = 1;
						 //std::cout << "HIT" << std::endl;
						 }

						 if (curY > input_area_size_y || curY < 0) {
						 hit = 1;
						 //std::cout << "HIT" << std::endl;
						 }

						 if (curZ > input_area_size_z || curZ < 0) {
						 hit = 1;
						 //std::cout << "HIT" << std::endl;
						 }*/

						if (!environment_obstacle_valid(&sensingEnvironment,
								curX, curY, curZ)) {
							hit = 1;

							if (s < 1) {
								std::cout
										<< "The initial sampling position is not valid."
										<< std::endl;
							}
						}

						curX = curX + cos(azimuthal) * resolution;
						curY = curY + sin(azimuthal) * resolution;

						if (!environment_obstacle_valid(&sensingEnvironment,
								curX, curY, curZ)) {
							hit = 1;

							curX = curX - cos(azimuthal) * resolution;
							curY = curY - sin(azimuthal) * resolution;

							std::cout << "Come across obstacles." << std::endl;
						}


					/*int offset =
					 (((int) curZ) * ((2 * (input_area_size_x * input_area_size_y + 1))))
					 + 2
					 * (((int) curX) * input_area_size_y
					 + ((int) curY));'/

					 /*stream.seekg(offset, stream.beg);
					 int value;
					 //stream >> value;

					 //std::cout << "Value: " << value << std::endl;*/

					if (hit == 1) {

						//std::cout << "The beam has stopped." << std::endl;

					} else {

						//curZ = curZ; // + m * cos(polar) * resolution;

						srv.request.x[m] = curX;
						srv.request.y[m] = curY;
						srv.request.z[m] = curZ;

						geometry_msgs::Point p;
						p.x = curX;
						p.y = curY;
						p.z = curZ;

						line.points.push_back(p);
						points.points.push_back(p);

						//marker_pub.publish(line);
						//marker_pub.publish(points);


					}

					temp_m = m;
				}

				boost::array<float, 20ul> concentration_values;
				//ROS_INFO("Here");

				if (client.call(srv)) {
					//for(int i =0; i<m; i++){
					concentration_values = srv.response.odor_r;
					// }
				}
				//ROS_INFO("SERVICE CALL ENDED.");

				for (int i = 0; i < temp_m; i++) {
					if (concentration_values[i] != concentration_values[i]
							|| concentration_values[i] < 0) {
						concentration_values[i] = 0;
					}
					finalConcentration = finalConcentration
							+ concentration_values[i] * resolution;
				}

				read_msg.data = finalConcentration;
				sensor_read_pub.publish(read_msg);

				char *sensorIndex = new char[500];
				strcpy(sensorIndex, input_result_location.c_str());
				strcat(sensorIndex, input_sensor_frame.c_str());
				strcat(sensorIndex, format.c_str());

				//ROS_INFO("sensor log location: %s", sensorIndex);

				/*totalConcentration = totalConcentration+ finalConcentration;

				 if (!totalConcentration>0){
				 totalConcentration = 0;
				 }*/

				//cout << "Current segment :" << s << endl;
				s++;

			}

			//std::cout << "Current angle:" << azimuthal << std::endl;

			//cout << "Concentration along this laser :" << finalConcentration << endl;

			marker_pub.publish(line);
			marker_pub.publish(points);

			writeConcentration(x_pos, y_pos, z_pos, curX, curY, curZ,
					finalConcentration, file_name);

		}

		//cout << "Final concentration in the scanned area: " << totalConcentration << endl;

		line.points.clear();
		points.points.clear();

		cout << "Current time step (sampling finised):"
				<< internal_time_step + 1 << endl;
		cout << "Current running sensor :" << input_sensor_frame.c_str()
				<< endl;

		//ros::Duration difference = ros::Time::now() - before;
		//std::cout<<"Took " <<difference<<std::endl;

		//before = ros::Time::now();

	}

	//internal_time_step++;

	ros::spinOnce();
	r.sleep();

}

}

//===================================================================================
float sensor_response(float concentration, int ch_id, int sensor_id,
	int sensor_model, float* sensor)
//===================================================================================

	{

//initial sensor values:
if (initial == true) {
	if (sensor_model == 0) {
		filtered_resp[sensor_id] = 21;
		sensor[counter[sensor_id]] = 21;
	}
	if (sensor_model == 1 && ch_id != 2) {
		filtered_resp[sensor_id] = 1;
		sensor[counter[sensor_id]] = 1;
	}
	if (sensor_model == 2) {
		filtered_resp[sensor_id] = 8.7;
		sensor[counter[sensor_id]] = 8.7;
	}

	return filtered_resp[sensor_id];
}

if (concentration == 0) {
	if (sensor_model == 0) {
		y_out = 21;
	}
	if (sensor_model == 1) {
		y_out = 1;
	}
	if (sensor_model == 2) {
		y_out = 8.7;
	}
} else {

	//A*e^B + C*e^D
	if (ch_id < 3)    //response for ethanol, methan and hydeogen
		y_out = (cons[sensor_model][ch_id][0]
				* exp(cons[sensor_model][ch_id][1] * concentration))
				+ (cons[sensor_model][ch_id][2]
						* exp(cons[sensor_model][ch_id][3] * concentration));
	else
		//response for all other gases
		y_out = (cons[sensor_model][0][0]
				* exp(cons[sensor_model][0][1] * concentration))
				+ (cons[sensor_model][0][2]
						* exp(cons[sensor_model][0][3] * concentration));
}

//storing sensor response in array for comparision:
sensor[counter[sensor_id]] = y_out;

//comparision between last value and current value:
//decay:
if (sensor[counter[sensor_id]] > sensor[counter[sensor_id] - 1]) {
	tau = 40.7 * 0.15;
}

//rise:
if (sensor[counter[sensor_id]] < sensor[counter[sensor_id] - 1]) {
	tau = 7.4 * 0.04;
}

//alpha value
a = 1 / (tau + 1);

//filtered response:
filtered_resp[sensor_id] = (a * y_out) + (1 - a) * filtered_resp[sensor_id];

counter[sensor_id]++;

//TGS2600 hydrogen response=0.1.
if (sensor_model == 1 && ch_id == 2) {
	filtered_resp[sensor_id] = 0.100000;
}

return filtered_resp[sensor_id];

}

//====================================================================
void amclCallback(const geometry_msgs::PoseWithCovarianceStamped& msg)
//====================================================================
	{
amcl_pos.pose.pose.position.x = msg.pose.pose.position.x;
amcl_pos.pose.pose.position.y = msg.pose.pose.position.y;
amcl_pos.pose.pose.position.z = msg.pose.pose.position.z;

}

//========================================================================
void chemicalCallback(const msgs_and_srvs::ChemicalValue::ConstPtr& msg)
//========================================================================
	{
ch_id = msg->id;

}

//========================================================================
void writeConcentration(float pos_xx, float pos_yy, float pos_zz, float end_xx,
	float end_yy, float end_zz, float concentrationValue, const char *file_name)
//========================================================================
	{
static FILE *file;

//char path[1000]="/home/han/gdm_simulator_catkin_ws/src/gas_dispersion_simulation/simulated_mox/src/";
//char path[500];
//strcpy( path, sensorIndex);

file = fopen(file_name,
//"/home/han/gdm_simulator_catkin_ws/src/gas_dispersion_simulation/simulated_tdlas/src/sensor_log.txt",
		"a");

//file = fopen(path, "a");
if (file == NULL) {
	fprintf(stderr, "Error opening sensor_log.txt\n");
	exit(1);
}

fprintf(file, "%f %f %f %f %f \n", pos_xx, pos_yy, end_xx, end_yy,
		concentrationValue);

fclose(file);

}

//========================================================================
void writeTofile(msgs_and_srvs::readings response)
//========================================================================
	{
static FILE *file;

file =
		fopen(
				"/home/itv/espeleo_ws/src/gas_leak_simulator/gas_dispersion_simulation/simulated_tdlas/src/sensor_log.txt",
				"a");
if (file == NULL) {
	fprintf(stderr, "Error opening sensor_log.txt\n");
	exit(1);
}

fprintf(file, "%f %f %f\n", response.r0_val, response.r1_val, response.r2_val);

fclose(file);

}

// Initializes the environment.
void environment_obstacle_init(struct sSensingEnvironment *sensingEnvironment,
	int max_x, int max_y, int max_z, double cell_size, const char *filePath) {
//double cell_size=1;

sensingEnvironment->lines.count = 0;
sensingEnvironment->rectangles.count = 0;

sensingEnvironment->arena.width = max_x;
sensingEnvironment->arena.height = max_y;
sensingEnvironment->arena.length = max_z;

//open file
FILE *file_ptr;
file_ptr = fopen(filePath, "r");
if (file_ptr == NULL)
	printf("Error in opening the environment file.\n");

// Counts the number of blocks needed
char ch;
int n_blocks = 0;
do {
	ch = getc(file_ptr);
	if (ch == '1') {
		n_blocks++;

	}
} while (ch != EOF);
fclose(file_ptr);
printf("The number of obstacles added: %d. \n", n_blocks);

file_ptr = fopen(filePath, "r");

double x_pos, y_pos;
x_pos = y_pos = 0;
int block_idx = 0;

do {
	ch = getc(file_ptr);
	if (ch == '1' || ch == '0') {

		if (ch == '1') {

			environment_obstacles_add(sensingEnvironment, x_pos, y_pos,
					x_pos + cell_size, y_pos + cell_size);

			block_idx++;

		}
		y_pos = y_pos + cell_size;

	}

	else if (ch == ';') {
		x_pos = 0;
		y_pos = 0;

		ch = getc(file_ptr);
	} else if (ch == '\n' || ch == '\r') {
		y_pos = 0;
		x_pos = x_pos + cell_size;
	}
} while (ch != EOF);

fclose(file_ptr);

}

// Adds a rectangle.
void environment_obstacles_add(struct sSensingEnvironment *sensingEnvironment,
	double x1, double y1, double x2, double y2) {
if (sensingEnvironment->rectangles.count >= 20000) {
	printf("Not enough memory allocated for rectangles.\n");
	exit(1);
}

int index = sensingEnvironment->rectangles.count;
sensingEnvironment->rectangles.item[index].x1 = x1;
sensingEnvironment->rectangles.item[index].y1 = y1;
sensingEnvironment->rectangles.item[index].x2 = x2;
sensingEnvironment->rectangles.item[index].y2 = y2;
sensingEnvironment->rectangles.count++;
}

// Returns whether a point (world coordinates) is valid or not.
int environment_obstacle_valid(struct sSensingEnvironment *sensingEnvironment,
	double x, double y, double z) {
// Arena
if ((z < 0) || (z >= sensingEnvironment->arena.length - 1)
		|| ((x < 0) || (x > sensingEnvironment->arena.width - 1))
		|| ((y < 0) || (y > sensingEnvironment->arena.height - 1))) {

	return 0;
}

// Rectangles
int i;
for (i = 0; i < sensingEnvironment->rectangles.count; i++) {
	if ((x >= sensingEnvironment->rectangles.item[i].x1)
			&& (y >= sensingEnvironment->rectangles.item[i].y1)
			&& (x <= sensingEnvironment->rectangles.item[i].x2)
			&& (y <= sensingEnvironment->rectangles.item[i].y2)) {
		return 0;
	}
}

return 1;
}

void loadNodeParameters(ros::NodeHandle private_nh) {

std::string parameter_name;
std::string param_string;
double param_double;
int param_int;
bool param_bool;

//Gas type
parameter_name = std::string(NODE_NAME) + std::string("/gas_type");
if (private_nh.getParam(parameter_name, param_int)) {
	input_gas_type = param_int;
} else {

	input_gas_type = DEFAULT_GAS_TYPE;
}

//Sensor model
parameter_name = std::string(NODE_NAME) + std::string("/sensor_model");
if (private_nh.getParam(parameter_name, param_int)) {
	input_sensor_model = param_int;
} else {

	input_sensor_model = DEFAULT_SENSOR_MODEL;
}

//sensor_frame
parameter_name = std::string(NODE_NAME) + std::string("/sensor_frame");
if (private_nh.getParam(parameter_name, param_string)) {
	input_sensor_frame = param_string;
} else {
	input_sensor_frame = DEFAULT_SENSOR_FRAME;
}

//fixed frame
parameter_name = std::string(NODE_NAME) + std::string("/fixed_frame");

if (private_nh.getParam(parameter_name, param_string)) {
	input_fixed_frame = param_string;
} else {

	input_fixed_frame = DEFAULT_FIXED_FRAME;
}

//Beam length
parameter_name = std::string(NODE_NAME) + std::string("/beam_length");
if (private_nh.getParam(parameter_name, param_int)) {
	input_beam_length = param_int;
} else {

	input_beam_length = DEFAULT_BEAM_LENGTH;
}

//Activation time
parameter_name = std::string(NODE_NAME) + std::string("/activation_time");
if (private_nh.getParam(parameter_name, param_int)) {
	input_activation_time = param_int;
} else {

	input_activation_time = DEFAULT_ACTIVATION_TIME;
}

// The starting angle of TDLAS sensor.
parameter_name = std::string(NODE_NAME) + std::string("/starting_angle");
if (private_nh.getParam(parameter_name, param_double)) {
	input_starting_angle = param_double;
} else {
	input_starting_angle = DEFAULT_STARTING_ANGLE;
}

// The ending angle of TDLAS sensor.
parameter_name = std::string(NODE_NAME) + std::string("/ending_angle");
if (private_nh.getParam(parameter_name, param_double)) {
	input_ending_angle = param_double;
} else {
	input_ending_angle = DEFAULT_ENDING_ANGLE;
}

// The incedence angle of TDLAS sensor.
parameter_name = std::string(NODE_NAME) + std::string("/incidence_angle");
if (private_nh.getParam(parameter_name, param_double)) {
	input_incidence_angle = param_double;
} else {
	input_incidence_angle = DEFAULT_INCIDENCE_ANGLE;
}

// The location of the simulation results.
parameter_name = std::string(NODE_NAME) + std::string("/result_location");
if (private_nh.getParam(parameter_name, param_string)) {
	input_result_location = param_string;
} else {
	input_result_location = DEFAULT_RESULT_LOCATION;
}

//environment file
parameter_name = std::string(NODE_NAME) + std::string("/environment_file");
if (private_nh.getParam(parameter_name, param_string)) {
	input_environment_location = param_string;
} else {
	input_environment_location = DEFAULT_ENVIRONMENT_LOCATION;
}

//Exploration area size x
parameter_name = std::string(NODE_NAME) + std::string("/area_size_x");

if (private_nh.getParam(parameter_name, param_int)) {
	input_area_size_x = param_int;
} else {
	input_area_size_x = DEFAULT_AREA_X;
}

//Exploration area size y
parameter_name = std::string(NODE_NAME) + std::string("/area_size_y");

if (private_nh.getParam(parameter_name, param_int)) {
	input_area_size_y = param_int;
} else {
	input_area_size_y = DEFAULT_AREA_Y;
}

//Exploration area size z
parameter_name = std::string(NODE_NAME) + std::string("/area_size_z");

if (private_nh.getParam(parameter_name, param_int)) {
	input_area_size_z = param_int;
} else {
	input_area_size_z = DEFAULT_AREA_Z;
}

//Cell size for the exploration area
parameter_name = std::string(NODE_NAME) + std::string("/cell_size");

if (private_nh.getParam(parameter_name, param_double)) {
	input_area_cell_size = param_double;
} else {
	input_area_cell_size = DEFAULT_AREA_CELL_SIZE;
}

ROS_INFO("The data provided in the roslaunch file is:");

ROS_INFO("Gas type: %d", input_gas_type);
//ROS_INFO("Sensor model: %d", input_sensor_model);
ROS_INFO("Fixed frame: %s", input_fixed_frame.c_str());
ROS_INFO("Sensor frame: %s", input_sensor_frame.c_str());

}

