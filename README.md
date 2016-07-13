# openvx_samples

Based on OpenVX 1.0. Some own module has been added to library. So to execute given code, we have to refer attached .lib, .dll, .h files.

own_module_code_C : own module code written in C. Originally used to be located in sample/targets/c_model. This was necessary for building process so won't be needed for running given code.
bin : .dll files
include : header files
lib : static .lib files

--------------
Own module 'findSiftKeypointNode' is a testing code yet. This just add 3 2d points into vx_array passed as parameter to be printed out on console.