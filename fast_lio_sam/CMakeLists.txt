cmake_minimum_required(VERSION 3.4)
project(fast_lio_sam)

### set compiler
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++17 -pthread -fexceptions -g -ggdb") #c++17, for parallel programming 
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra ${CMAKE_CXX_FLAGS}")

### get packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  visualization_msgs
  tf
  tf_conversions
  pcl_ros
  pcl_conversions
  message_filters
)
find_package(PCL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GTSAM REQUIRED)
if(PCL_FOUND)
  message(WARNING "PCL_VER: ${PCL_VERSION}, and PCL_DIR: ${PCL_INCLUDE_DIRS}")
endif()
if(Eigen3_FOUND)
  message(WARNING "EIGEN_VER: ${EIGEN3_VERSION_STRING}, and Eigen DIR: ${EIGEN3_INCLUDE_DIR}")
endif()
if(GTSAM_FOUND)
  message(WARNING "GTSAM_VER: ${GTSAM_VERSION}, and GTSAM DIR: ${GTSAM_INCLUDE_DIR}")
endif()

catkin_package() #this automatically adds/installs the results of build

### get packages' headers
include_directories(
  include
	${catkin_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${GTSAM_INCLUDE_DIR}
)
#### for tbb
set(TBB2020_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../third_party/tbb-aarch64/dist/include") #CMAKE_CURRENT_SOURCE_DIR should be used when in add_subdirectory
set(TBB2020_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/../third_party/tbb-aarch64/dist/lib")
include_directories(${TBB2020_INCLUDE_DIR})
link_directories(${TBB2020_LIBRARY_DIR})

###########
## Build ##
###########
### main
add_library(${PROJECT_NAME}_constructor src/constructor.cpp)
target_link_libraries(${PROJECT_NAME}_constructor ${catkin_LIBRARIES} ${PCL_LIBRARIES} ${EIGEN3_LIBS} gtsam tbb)
add_library(${PROJECT_NAME}_methods src/methods.cpp)
target_link_libraries(${PROJECT_NAME}_methods ${catkin_LIBRARIES} ${PCL_LIBRARIES} ${EIGEN3_LIBS} gtsam tbb)
add_library(${PROJECT_NAME}_callbacks src/callbacks.cpp)
target_link_libraries(${PROJECT_NAME}_callbacks ${catkin_LIBRARIES} ${PCL_LIBRARIES} ${EIGEN3_LIBS} gtsam tbb)

add_executable(${PROJECT_NAME}_node src/main.cpp)
target_link_libraries(${PROJECT_NAME}_node ${catkin_LIBRARIES} ${PCL_LIBRARIES} ${EIGEN3_LIBS} gtsam tbb ${PROJECT_NAME}_constructor ${PROJECT_NAME}_callbacks ${PROJECT_NAME}_methods)