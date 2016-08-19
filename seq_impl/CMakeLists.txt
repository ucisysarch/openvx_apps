cmake_minimum_required(VERSION 2.6)
project(openvx_apps)


if( NOT DEFINED ENV{OPENCV_DIR} )
    message ("OPENCV_DIR needs to be set to the OpenCV root dir before continuing; now attempting to find package using CMake")
    find_package(opencv REQUIRED)

else( NOT DEFINED ENV{OPENCV_DIR} )
    message ("$ENV{OPENCV_DIR}")
    set (OpenCV_INCLUDE_DIR "$ENV{OPENCV_DIR}/include")
    set (OpenCV_LIBS "opencv_core" "opencv_highgui"  "opencv_imgcodecs" "opencv_imgproc" "opencv_features2d")
    set (OpenCV_LIBS_DIR "$ENV{OPENCV_DIR}/lib")

endif( NOT DEFINED ENV{OPENCV_DIR} )

include_directories( ${OpenCV_INCLUDE_DIR} )
link_directories( ${OpenCV_LIBS_DIR}) 

add_executable(opencv_sift sift/opencv_sift.cpp)
target_link_libraries(opencv_sift ${OpenCV_LIBS})

