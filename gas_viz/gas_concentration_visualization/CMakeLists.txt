cmake_minimum_required(VERSION 2.8.3)
project(gas_concentration_visualization)

find_package(catkin REQUIRED COMPONENTS cv_bridge roscpp pcl_ros
  std_msgs
  nav_msgs
  tf
  cv_bridge 
  sensor_msgs 
  visualization_msgs
  image_transport
  geometry_msgs
  message_generation
  msgs_and_srvs
   )

add_service_files(
  FILES
  SensorPositionViz.srv
)

generate_messages(
  DEPENDENCIES geometry_msgs std_msgs msgs_and_srvs
)
catkin_package(
#    DEPENDS opencv2 libpcl-all-dev 
    CATKIN_DEPENDS cv_bridge roscpp pcl_ros
    INCLUDE_DIRS include
    LIBRARIES ${PROJECT_NAME}
)

#uncomment if you have defined services
#rosbuild_gensrv()

FILE(GLOB_RECURSE MYFILES_CPP "src/*.cpp")
add_executable(gas_concentration_visualization src/gas_concentration_visualization.cpp src/gas_map.cpp)

include_directories(include)
include_directories(include ${catkin_INCLUDE_DIRS})

target_link_libraries(gas_concentration_visualization
   ${catkin_LIBRARIES}
 )
