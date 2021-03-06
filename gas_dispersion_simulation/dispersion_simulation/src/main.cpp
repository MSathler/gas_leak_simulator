#include <math.h>
#include "dispersion_simulation/main.h"
using namespace std;

//---------------service response----------------------//
bool pos(msgs_and_srvs::SensorPosition2::Request  &req,
		msgs_and_srvs::SensorPosition2::Response &res )
{
	int size = req.x.size();
	//cout << "Inside service " << endl;
	for(int i =0; i < size; i++){

		//concentration=concentration at(x,y,z):
		res.odor_r[i]=concentration(req.x[i], req.y[i], req.z[i], input_area_cell_size, input_temperature, input_pressure, input_unit_choice);
		//cout << req.x[i] << " " << req.y[i]<< " " << req.z[i]<<" " << res.odor_r[i] << endl;;
		//res.odor_r1=concentration(req.x_r1, req.y_r1, req.z_r1);
		//res.odor_r2=concentration(req.x_r2, req.y_r2, req.z_r2);
	}


	return true;
}


//====================================================================================
//
//			M   A   I   N       L   O   O   P 
//
//====================================================================================

// The node implements the filament-base gas dispersal simulation. At each time step, the puffs of filaments are sequentially released at a source location. Each puff is composed of n filaments.
// Filaments are affected by turbulence and molecular diffusion along its path while being transported by advection with the wind. 
// The 3-dimensional positions of these filaments are represented by the points of the “visualization msgs/markers”. At each time step,“Dispersal_Simulation” node calculates or determines the positions of n filaments. 
// Gas plumes are simulated with or without acceleration.

int main(int argc, char **argv) {

	//(1) Creates folders for videos and results
	//mkdir(input_result_location.c_str(), 0755);
	//mkdir("/home/han/gdm_simulator_catkin_ws/src/simulated_tdlas/recordedData/results", 0755);
	//mkdir("video", 0755);

	//(2) Creates a log file.
	//ofstream myfile;
	//myfile.open ("/home/han/gdm_simulator_catkin_ws/src/simulated_tdlas/recordedData/pos.log");
	//myfile.open (input_result_location.c_str());
	configuration_init();
	configuration_parse_commandline(argc, argv);

	//ofstream gasTypeFile;
	//gasTypeFile.open (input_result_location.c_str());
	//gasTypeFile.open ("/home/han/gdm_simulator_catkin_ws/src/simulated_tdlas/recordedData/gasType.log");
	//gasTypeFile << input_gas_type <<"\n";
	//gasTypeFile.close();

	//(3) Initialize the random number generator
	mt_goodseed();

	//(4) Initializes and loads the node configuration
	ros::init(argc, argv, NODE_NAME);
	ros::NodeHandle n;
	loadNodeParameters(n);

	//block_idx=loadObstacles(input_environment_location, input_area_cell_size);
	//ROS_INFO("#Obstacles: %d",block_idx);

	//	input_area_size_x=61;
	//input_area_size_y=61;
	//input_area_size_z=61;
	//(5) Initializes and starts the gas dispersion simulation

	ROS_INFO("simulation starts .......");

	algorithm_init(input_wind_data.c_str(),input_snapshots,input_filament_number,input_area_size_x,input_area_size_y,input_area_size_z,input_area_cell_size,input_source_pos_x,input_source_pos_y,input_source_pos_z, input_spreading_width, input_environment_location.c_str());

	long long int numMolecules = input_num_molecules/input_snapshots;
	ROS_INFO("#molecules per snapshot: %lld",numMolecules);

	//(6) Loads the plume constants w.r.t. the selected gas

	plume_start();

	//Plume publisher  
	//The 3-dimensional positions of these filaments are represented by the points of the “visualization msgs/markers”.

	ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("dispersion_visualization", 1);
	ros::ServiceServer service = n.advertiseService("odor_value", pos); // Here the service for concentration information is created and advertised over ROS.



	// Preparing for naming the files that storing the simulation results
	ostringstream gasTypeString;
	gasTypeString << input_gas_type;
	std::string gt(gasTypeString.str());
	ROS_INFO("gas: %s",gt.c_str());


	std::string nameString = "gasType";


	std:string tempString;
	tempString = "_simulation";

	string filePathInput = input_result_location + nameString + gt + tempString;

	char *cfileName = new char[filePathInput.length() + 1];
	strcpy(cfileName, filePathInput.c_str());

	//ROS_INFO("wind file location: %s", filePathInput.c_str());

	ros::Rate r(1/input_delta_t);

	bool flow=false;

    while (ros::ok())
	{

		//chemical id publisher
		//msgs_and_srvs::ChemicalValue chemical;
		//chemical.id=input_gas_type;
		density=Chemicalvalue[input_gas_type];

		//Service respone (hit_rate.value)



		//marker points for filaments
		visualization_msgs::Marker draw;
		draw.header.frame_id  = input_fixed_frame;
		draw.header.stamp =ros::Time::now();
		draw.ns = "plume_simulation";
		draw.action = visualization_msgs::Marker::ADD;
		draw.pose.orientation.w = 1.0;
		draw.id = 0;
		draw.type = visualization_msgs::Marker::POINTS;
		draw.color.a = 1;


		ROS_INFO("the current snapshot is: %d",algorithm.marker.counter);
		/*if ( algorithm.marker.counter%2 != 0) {
			numMolecules=numMolecules*(0.5+mt_drand());
			ROS_INFO("the current snapshot is: %d",algorithm.marker.counter);
		}
		else{
			numMolecules=numMolecules*1;
		}*/

		//Calculating the hit_rate
		if(flow==true){
			// ''0.5'' is based on the cell sized set here.
			algorithm_hit_rate_calculate(input_filament_number, input_snapshots, algorithm.source.x - 0.5, algorithm.source.y - 0.5,algorithm.source.z-0.5,
					algorithm.source.x + 0.5, algorithm.source.y + 0.5, algorithm.source.z+0.5, cfileName, input_spreading_width, numMolecules, input_temperature, input_pressure, input_unit_choice, input_windflow_resolution);
		}   flow=true;


		int k;
		//for(int k=0;k<algorithm.marker.counter*input_filament_number; k++)
		for(int ii=0;ii<algorithm.marker.counter;ii++)
		{
			for(int jj=0;jj<input_filament_number;jj++)
			{
				k = ii*input_filament_number+jj;
				//points

				geometry_msgs::Point p;

				//color of points
				std_msgs::ColorRGBA c;

				//width of points
				draw.scale.x = algorithm.marker.width[k];
				draw.scale.y = algorithm.marker.width[k];
				draw.scale.z = algorithm.marker.width[k];

				//assigning the point positions
				p.x = algorithm.marker.x[k];
				p.y = algorithm.marker.y[k];
				p.z = algorithm.marker.z[k];

				//ROS_INFO("gas position: (%f,%f,%f)",p.x,p.y,p.z);

				//float tempConcentration;
				//double tempConcentration = concentration(p.x, p.y, p.z,input_area_cell_size);
				//ROS_INFO("current concentration: %f",tempConcentration);


				//c.g=1;
				c.a=1;

				//uncomment these two lines to visualize color of filaments according to increasing width
				red=algorithm.marker.colour[k];
				color_width(&red, &green, &blue); c.r=red; c.g=green; c.b=blue;

				//uncomment these two lines to visualizecolor of filaments according to concentration
				//red = concentration(p.x, p.y, p.z);
				//color_concentration(&red, &green, &blue); c.r=red; c.g=green; c.b=blue;


				draw.colors.push_back(c);
				draw.points.push_back(p);
			}


		}

		//log of plume height
		//myfile << algorithm.marker.z[0] <<"\n";

		//Publishing the points
		marker_pub.publish(draw);

		//publishing the chemical value
		//ChemicalValue.publish(chemical);


		r.sleep();
		ros::spinOnce();

	}

	//myfile.close();

	// release the char.
	delete [] cfileName;
	return 0;
}




/*int loadObstacles(std::string file_name, double cell_size){


	//open file
	FILE *file_ptr;
	file_ptr = fopen(file_name.c_str(), "r");
	// Counts the number of blocks needed
	char ch;
	int n_blocks=0;
	do {
		ch = getc(file_ptr);
		if (ch=='1') {
			n_blocks++;
		}
	}while(ch!=EOF);
	fclose(file_ptr);



	file_ptr = fopen(file_name.c_str(), "r");

	double x_pos,y_pos;
	x_pos=y_pos=0;
	int block_idx=0;


	do {
		ch = getc(file_ptr);
		if (ch=='1' || ch=='0'){

			if(ch=='1'){

				//obstacle[block_idx].coordinate.obstacle_x1 = x_pos;
				//obstacle[block_idx].coordinate.obstacle_x2 = y_pos + cell_size;
				//obstacle[block_idx].coordinate.obstacle_y1 = x_pos;
				//obstacle[block_idx].coordinate.obstacle_y2 = y_pos + cell_size;

				block_idx++;

			}
			y_pos=y_pos+cell_size;

		}

		else if (ch==';') {
			x_pos=0;
			y_pos=0;

			ch = getc(file_ptr);
		}
		else if (ch == '\n' || ch == '\r') {
			y_pos=0;
			x_pos=x_pos+cell_size;
		}
	} while(ch!=EOF);

	fclose(file_ptr);

	return block_idx;

}*/



void	loadNodeParameters(ros::NodeHandle private_nh)
{

	std::string parameter_name;
	std::string param_string;
	double	param_double;
	int		param_int;
	bool	param_bool;



	//Source position (x)
	parameter_name=std::string(NODE_NAME)+std::string("/source_position_x");
	if(private_nh.getParam(parameter_name,param_double)) {  
		input_source_pos_x=param_double;
	}
	else {
		input_source_pos_x=DEFAULT_SOURCE_POS_X;
	}

	//Source position (y)
	parameter_name=std::string(NODE_NAME)+std::string("/source_position_y");
	if(private_nh.getParam(parameter_name,param_double)) {  
		input_source_pos_y=param_double;
	}
	else {
		input_source_pos_y=DEFAULT_SOURCE_POS_Y;
	}

	//Source postion (z)
	parameter_name=std::string(NODE_NAME)+std::string("/source_position_z");
	if(private_nh.getParam(parameter_name,param_double)) {  
		input_source_pos_z=param_double;
	}
	else {
		input_source_pos_z=DEFAULT_SOURCE_POS_Z;
	}

	//Gas type
	parameter_name=std::string(NODE_NAME)+std::string("/gas_type");
	if(private_nh.getParam(parameter_name,param_int)) {  
		input_gas_type=param_int;
	}
	else {
		input_gas_type=DEFAULT_GAS_TYPE;
	}

	//delta_t
	parameter_name=std::string(NODE_NAME)+std::string("/delta_t");
	if(private_nh.getParam(parameter_name,param_double)) {  
		input_delta_t=param_double;
	}
	else {
		input_delta_t=DEFAULT_DELTA_T;
	}


	//wind files
	parameter_name=std::string(NODE_NAME)+std::string("/wind_data");
	if(private_nh.getParam(parameter_name,param_string)) {  
		input_wind_data=param_string;
	}
	else {
		input_wind_data=DEFAULT_WIND_DATA;
	}


	//environment file
	parameter_name=std::string(NODE_NAME)+std::string("/environment_file");
	if(private_nh.getParam(parameter_name,param_string)) {
		input_environment_location=param_string;
	}
	else {
		input_environment_location=DEFAULT_ENVIRONMENT_LOCATION	;
	}

	// The location of the simulation results.
	parameter_name=std::string(NODE_NAME)+std::string("/result_location");
	if(private_nh.getParam(parameter_name,param_string)) {
		input_result_location=param_string;
	}
	else {
		input_result_location=DEFAULT_RESULT_LOCATION;
	}


	//filaments
	parameter_name=std::string(NODE_NAME)+std::string("/filaments");


	if(private_nh.getParam(parameter_name,param_int)) {  
		input_filament_number=param_int;
	}
	else {
		input_filament_number=DEFAULT_FILAMENT_NUMBER;
	}



	//fixed frame
	parameter_name=std::string(NODE_NAME)+std::string("/fixed_frame");

	if(private_nh.getParam(parameter_name,param_string)) {  
		input_fixed_frame=param_string;
	}
	else {
		input_fixed_frame=DEFAULT_FIXED_FRAME;
	}

	//snapshots
	parameter_name=std::string(NODE_NAME)+std::string("/snapshots");

	if(private_nh.getParam(parameter_name,param_int)) {  
		input_snapshots=param_int;
	}
	else {
		input_snapshots=DEFAULT_SNAPSHOTS;
	}

	//Exploration area size x
	parameter_name=std::string(NODE_NAME)+std::string("/area_size_x");	

	if(private_nh.getParam(parameter_name,param_int)) {  
		input_area_size_x=param_int;
	}
	else {
		input_area_size_x=DEFAULT_AREA_X;
	}


	//Exploration area size y
	parameter_name=std::string(NODE_NAME)+std::string("/area_size_y");	

	if(private_nh.getParam(parameter_name,param_int)) {  
		input_area_size_y=param_int;
	}
	else {
		input_area_size_y=DEFAULT_AREA_Y;
	}	


	//Exploration area size z
	parameter_name=std::string(NODE_NAME)+std::string("/area_size_z");	

	if(private_nh.getParam(parameter_name,param_int)) {  
		input_area_size_z=param_int;
	}
	else {
		input_area_size_z=DEFAULT_AREA_Z;
	}	

	//Cell size for the exploration area
	parameter_name=std::string(NODE_NAME)+std::string("/cell_size");

	if(private_nh.getParam(parameter_name,param_double)) {  
		input_area_cell_size=param_double;
	}
	else {
		input_area_cell_size=DEFAULT_AREA_CELL_SIZE;
	}	

	// The parameter for setting the dispersion width.
	parameter_name=std::string(NODE_NAME)+std::string("/gas_dispersion_width");

	if(private_nh.getParam(parameter_name,param_double)) {
		input_spreading_width=param_double;
	}
	else {
		input_spreading_width=DEFAULT_SPREADING_WIDTH;
	}

	// The parameter for setting the dispersion width.
	parameter_name=std::string(NODE_NAME)+std::string("/total_num_molecules");

	if(private_nh.getParam(parameter_name,param_int)) {
		input_num_molecules=param_int;
	}
	else {
		input_num_molecules=DEFAULT_NUM_MOLECULES;
	}

	// The parameter for setting the environment temperature.
	parameter_name=std::string(NODE_NAME)+std::string("/temperature");
	if(private_nh.getParam(parameter_name,param_double)) {
		input_temperature=param_double;
	}
	else {
		input_temperature=DEFAULT_TEMPERATURE;
	}

	// The parameter for setting the enviorment pressure.
	parameter_name=std::string(NODE_NAME)+std::string("/pressure");
	if(private_nh.getParam(parameter_name,param_double)) {
		input_pressure=param_double;
	}
	else {
		input_pressure=DEFAULT_PRESSURE;
	}

	// The parameter for setting the unit for concentration.
	parameter_name=std::string(NODE_NAME)+std::string("/concentration_unit_choice");
	if(private_nh.getParam(parameter_name,param_int)) {
		input_unit_choice=param_int;
	}
	else {
		input_unit_choice=DEFAULT_UNIT_CHOICE;
	}

	// The parameter for setting the map resolution.
	parameter_name=std::string(NODE_NAME)+std::string("/map_resolution");
	if(private_nh.getParam(parameter_name,param_double)) {
		input_windflow_resolution=param_double;
	}
	else {
		input_windflow_resolution=DEFAULT_WINDFLOW_RESOLUTION;
	}


	ROS_INFO("The data provided in the roslaunch file is:");
	ROS_INFO("Source position: (%f,%f,%f)",input_source_pos_x,input_source_pos_y,input_source_pos_z);
	ROS_INFO("Gas type: %d",input_gas_type);
	ROS_INFO("Delta time: %f",input_delta_t);
	ROS_INFO("wind file location: %s",input_wind_data.c_str());
	ROS_INFO("Number of filaments: %d",input_filament_number);
	ROS_INFO("Number of molecules: %lld",input_num_molecules);
	ROS_INFO("Fixed frame: %s",input_fixed_frame.c_str());
	ROS_INFO("Snapshots: %d",input_snapshots);
	ROS_INFO("Area size (%d,%d,%d) - with cell %f",input_area_size_x,input_area_size_y,input_area_size_z,input_area_cell_size);
	ROS_INFO("Unit type for concentration: %d",input_unit_choice);


}

