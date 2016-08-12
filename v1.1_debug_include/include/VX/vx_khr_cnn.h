/* 
 * Copyright (c) 2012-2016 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
 *    https://www.khronos.org/registry/
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef _VX_KHR_CNN_H_
#define _VX_KHR_CNN_H_

/*!
 * \file
 * \brief The Khronos Extension for Deep Convolutional Networks Functions.
 *
 * \defgroup group_cnn Extension: Deep Convolutional Networks API
 * \brief Convolutional Network Nodes.
 * \defgroup group_tensor Tensor API
 * \brief The Tensor API for Deep Convolutional Networks Functions.
 * \details The tensor is a multidimensional opaque object.Since the object have no visibility to the programmer. Vendors can introduce many optimization possibilities.
 * An example of such optimization can be found in the following article.http://arxiv.org/abs/1510.00149
*/

#define OPENVX_KHR_CNN   "vx_khr_cnn"

#if defined(OPENVX_CNN_1_0)
#undef OPENVX_CNN_1_1
#endif

#include <VX/vx.h>

/*! \brief tensor Data attributes.
 * \ingroup group_tensor
 */
enum vx_tensor_attribute_e
{
    /*! \brief Number of dimensions. */
    VX_TENSOR_NUM_OF_DIMS = VX_ATTRIBUTE_BASE( VX_ID_KHRONOS, VX_TYPE_TENSOR ) + 0x0,
    /*! \brief Dimension sizes. */
    VX_TENSOR_DIMS        = VX_ATTRIBUTE_BASE( VX_ID_KHRONOS, VX_TYPE_TENSOR ) + 0x1,
    /*! \brief tensor Data element data type. <tt>vx_type_e</tt> */
    VX_TENSOR_DATA_TYPE   = VX_ATTRIBUTE_BASE( VX_ID_KHRONOS, VX_TYPE_TENSOR ) + 0x2,
	/*! \brief fixed point position when the input element type is int16. */
	VX_TENSOR_FIXED_POINT_POS = VX_ATTRIBUTE_BASE(VX_ID_KHRONOS, VX_TYPE_TENSOR) + 0x4
};


/*! \brief A list of context attributes.
* \ingroup group_tensor
*/
enum vx_context_attribute_e {
	/*! \brief tensor Data max num of dimensions supported by HW. */
	VX_CONTEXT_MAX_TENSOR_DIMENSIONS = VX_ATTRIBUTE_BASE(VX_ID_KHRONOS, VX_TYPE_CONTEXT) + 0x0
	/*! \brief max accumulation bits supported */
	VX_CONTEXT_MAX_CONVOLUTIONAL_NETWORK_ACCUMULATION_BITS = VX_ATTRIBUTE_BASE(VX_ID_KHRONOS, VX_TYPE_CONTEXT) + 0x1
	/*! \brief min accumulation bits supported */
	VX_CONTEXT_MIN_CONVOLUTIONAL_NETWORK_ACCUMULATION_BITS = VX_ATTRIBUTE_BASE(VX_ID_KHRONOS, VX_TYPE_CONTEXT) + 0x2
};


/*==============================================================================
CONVOLUTIONAL_NETWORK structs and enums
=============================================================================*/
/*! \brief The multidimensional data object (Tensor).
 * \see vxCreateTensor
 * \ingroup group_tensor
 * \extends vx_reference
 */
typedef struct _vx_tensor_t * vx_tensor;

/*! \brief The multi dimensional view data structure.
* \details Used to split tensors into several views. Or concatenate several view into one tensor.
* \see vxCreateTensorFromView
* \ingroup group_tensor
*/
typedef struct _vx_tensor_view_t * vx_tensor_view;

/*! \brief The addressing of a tensor view patch structure is used by the Host only
* to address elements in a tensor view patch.
* \see <tt>\ref vxCopyTensorPatch</tt>
* \ingroup group_tensor
*/
typedef struct _vx_tensor_addressing_t  * vx_tensor_addressing;

/*! \brief The Convolutional Network down scaling size rounding type list.
* \details rounding done downscaling, In convolution and pooling functions.
* Relevant when input size is even.
* \ingroup group_cnn
*/
enum vx_convolutional_networks_rounding_type_e
{
	/*! \brief floor rounding  */
	VX_CONVOLUTIONAL_NETWORK_DS_SIZE_ROUNDING_FLOOR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ROUNDING_TYPE) + 0x0,
	/*! \brief ceil rounding */
	VX_CONVOLUTIONAL_NETWORK_DS_SIZE_ROUNDING_CEILING = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ROUNDING_TYPE) + 0x1
};


/*! \brief The Convolutional Network pooling type list.
* \details kind of pooling done in pooling function
* \ingroup group_cnn
*/
enum vx_convolutional_network_pooling_type_e
{
	/*! \brief max pooling*/
	VX_CONVOLUTIONAL_NETWORK_POOLING_MAX = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_POOL_TYPE) + 0x0,
	/*! \brief average pooling*/
	VX_CONVOLUTIONAL_NETWORK_POOLING_AVG = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_POOL_TYPE) + 0x1
};


/*! \brief The Convolutional Network normalization type list.
* \ingroup group_cnn
*/
enum vx_convolutional_network_norm_type_e
{
	/*! \brief normalization is done on same IFM*/
	VX_CONVOLUTIONAL_NETWORK_NORM_SAME_MAP = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_NORM_TYPE) + 0x0,
	/*! \brief Normalization is done across different IFMs*/
	VX_CONVOLUTIONAL_NETWORK_NORM_ACROSS_MAPS = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_NORM_TYPE) + 0x1,
};



/*! \brief The Convolutional Network activation functions list.
* \details 
* <table>
* <tr><td> <B>Function name </B> <td> <B>Mathematical definition</B> <td> <B>Parameters</B> <td> <B>Parameters type</B>
* <tr><td>logistic <td> \f$f(x)=1/(1+e^{-x}) \f$  <td>  <td>
* <tr><td>hyperbolic tangent <td> \f$f(x)=a\cdot tanh(b\cdot x) \f$  <td> a,b  <td> VX_INT32
* <tr><td>relu <td> \f$f(x)=max(0,x)\f$  <td>  <td>
* <tr><td>bounded relu <td> \f$f(x)=min(a,max(0,x)) \f$  <td> a  <td> VX_INT32
* <tr><td>soft relu <td> \f$f(x)=log(1+e^{x}) \f$  <td>  <td>
* <tr><td>abs <td> \f$f(x)=\mid x\mid \f$  <td>  <td>
* <tr><td>square <td> \f$f(x)= x^2 \f$  <td>  <td>
* <tr><td>square root <td> \f$f(x)=\sqrt{x} \f$  <td>  <td>
* <tr><td>linear <td> \f$f(x)=ax+b \f$  <td>  a,b  <td> VX_INT32
* </table>
* \ingroup group_cnn
*/
enum vx_convolutional_network_activation_func_e
{
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_LOGISTIC = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x0,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_HYPERBOLIC_TAN = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x1,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_RELU = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x2,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_BRELU = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x3,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_SOFTRELU = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x4,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_ABS = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x5,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_SQUARE = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x6,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_SQRT = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x7,
	VX_CONVOLUTIONAL_NETWORK_ACTIVATION_LINEAR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVOLUTIONAL_NETWORK_ACTIVATION_FUNC) + 0x8,
};

/* END CNN types*/

/*==============================================================================
    TENSOR DATA FUNCTIONS
=============================================================================*/
/*! \brief Creates an opaque reference to a tensor data buffer.
 * \details Not guaranteed to exist until the <tt>vx_graph</tt> containing it has been verified.
 * \param [in] context The reference to the implementation context.
 * \param [in] num_of_dims The number of dimensions.
 * \param [in] sizes Dimensions sizes in elements.
 * \param [in] data_format The <tt>vx_type_t</tt> that represents the data type of the tensor data elements.
 * \param [in] fixed_point_pos Specifies the fixed point position when the input element type is int16, if 0 calculations are performed in integer math
 * \return A tensor data reference or zero when an error is encountered.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_tensor VX_API_CALL vxCreateTensor(vx_context context, vx_uint32 num_of_dims, vx_uint32 *sizes, vx_enum data_format,vx_uint8 fixed_point_pos);

/*! \brief Creates an array of images into the multi-dimension data, this can be a adjacent 2D images or not depending on the stride value. 
 * The stride value is representing bytes in the third dimension.
 * The OpenVX image object that points to a three dimension data and access it as an array of images.
 * This has to be portion of the third lowest dimension, and the stride correspond to that third dimension.
 * The returned Object array is an array of images. Where the image data is pointing to a specific memory in the input tensor.
 * \param [in] tensor The tensor data from which to extract the images. Has to be a 3d tensor.
 * \param [in] rect Image coordinates within tensor data.
 * \param [in] array_size Number of images to extract.
 * \param [in] stride Delta between two images in the array.
 * \param [in] image_format The requested image format. Should match the tensor data's data type.
 * \return An array of images pointing to the tensor data's data.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_object_array VX_API_CALL vxCreateImageObjectArrayFromTensor(vx_tensor tensor, vx_rectangle_t rect, vx_uint32 array_size, vx_uint32 stride, vx_df_image image_format);

/*! \brief Creates a tensor data from another tensor data given a view. This second
 * reference refers to the data in the original tensor data. Updates to this tensor data
 * updates the parent tensor data. The view must be defined within the dimensions
 * of the parent tensor data.
 * \param [in] tensor The reference to the parent tensor data.
 * \param [in] view The region of interest of a tensor view. Must contain points 
 * within the parent tensor data dimensions. <tt>\ref vx_tensor_view</tt>
 * \return The reference to the sub-tensor or zero if the view is invalid.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_tensor VX_API_CALL vxCreateTensorFromView(vx_tensor tensor, vx_tensor_view view);

/*! \brief Creates an opaque reference to a tensor data buffer with no direct
 * user access. This function allows setting the tensor data dimensions or data format.
 * \details Virtual data objects allow users to connect various nodes within a
 * graph via data references without access to that data, but they also permit the
 * implementation to take maximum advantage of possible optimizations. Use this
 * API to create a data reference to link two or more nodes together when the
 * intermediate data are not required to be accessed by outside entities. This API
 * in particular allows the user to define the tensor data format of the data without
 * requiring the exact dimensions. Virtual objects are scoped within the graph
 * they are declared a part of, and can't be shared outside of this scope.
 * \param [in] graph The reference to the parent graph.
 * \param [in] num_of_dims The number of dimensions.
 * \param [in] sizes Dimensions sizes in elements.
 * \param [in] data_format The <tt>vx_type_t</tt> that represents the data type of the tensor data elements.
 * \param [in] fixed_point_pos Specifies the fixed point position when the input element type is int16, if 0 calculations are performed in integer math
 * \return A tensor data reference or zero when an error is encountered.
 * \note Passing this reference to <tt>\ref vxCopyTensorPatch</tt> will return an error.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_tensor VX_API_CALL vxCreateVirtualTensor(vx_graph graph, vx_uint32 num_of_dims, vx_uint32 *sizes, vx_enum data_format, vx_uint8 fixed_point_pos);


/*! \brief Allows the application to copy a view patch from/into an tensor object .
* \param [in] tensor The reference to the tensor object that is the source or the
* destination of the copy.
* \param [in] view Optional parameter of type <tt>\ref vx_tensor_view</tt>. The coordinates of the view patch. The patch must be within
* the bounds of the tensor. (start[index],end[index]) gives the coordinates of the view
* element out of the patch. Must be 0 <= start < end <= number of elements in the tensor dimension.
* see <tt>\ref vxCreateTensorView</tt>. If NULL is given instead of the object. Then the function behaves as if view was the size of the full tensor.
* \param [in] user_addr The address of a structure describing the layout of the
* user memory location pointed by user_ptr. In the structure, dim[index],
* stride[index] fields must be provided, other fields are ignored by the function.
* The layout of the user memory must follow a row major order. see <tt>\ref vxCreateTensorAddressing</tt>
* \param [in] user_ptr The address of the memory location where to store the requested data
* if the copy was requested in read mode, or from where to get the data to store into the tensor
* object if the copy was requested in write mode. The accessible memory must be large enough
* to contain the specified patch with the specified layout:\n
* accessible memory in bytes >= (end[last_dimension] - start[last_dimension]) * stride[last_dimension].\m
* see <tt>\ref vxCreateTensorAddressing</tt> and <tt>\ref vxCreateTensorView</tt>.
* \param [in] usage This declares the effect of the copy with regard to the tensor object
* using the <tt>vx_accessor_e</tt> enumeration. Only VX_READ_ONLY and VX_WRITE_ONLY are supported:
* \arg VX_READ_ONLY means that data is copied from the tensor object into the application memory
* \arg VX_WRITE_ONLY means that data is copied into the tensor object from the application memory
* \param [in] user_mem_type A <tt>vx_memory_type_e</tt> enumeration that specifies
* the memory type of the memory referenced by the user_addr.
* \return A <tt>vx_status_e</tt> enumeration.
* \retval VX_ERROR_OPTIMIZED_AWAY This is a reference to a virtual tensor that cannot be
* accessed by the application.
* \retval VX_ERROR_INVALID_REFERENCE The tensor reference is not actually an tensor reference.
* \retval VX_ERROR_INVALID_PARAMETERS An other parameter is incorrect.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_status VX_API_CALL vxCopyTensorPatch(vx_tensor tensor, vx_tensor_view view, vx_tensor_addressing user_addr, void *user_ptr, vx_enum usage, vx_enum user_mem_type);

/*! \brief Retrieves various attributes of a tensor data.
 * \param [in] tensor The reference to the tensor data to query.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_tensor_attribute_e</tt>.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size of the container to which \a ptr points.
 * \return A <tt>vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If data is not a <tt>\ref vx_tensor</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS If any of the other parameters are incorrect.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryTensor(vx_tensor tensor, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Releases a reference to a tensor data object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] tensor The pointer to the tensor data to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_SUCCESS Success
 * \retval * An error occurred. See <tt>vx_status_e</tt>.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseTensor(vx_tensor *tensor);


/*! \brief Create  an opaque reference to a tensor view object.
 * \details Not guaranteed to exist until the <tt>vx_graph</tt> containing it has been verified.
 * \param [in] context The reference to the implementation context.
 * \param [in] view_array_start a vx_uint32 array of start values of the view.
 * \param [in] view_array_end a vx_uint32 array of end values of the view.
 * \param [in] numViewDimensions number of dimensions of view_array_start and view_array_end.
 * \return A tensor data view reference or zero when an error is encountered.
 * \ingroup group_tensor
 */
VX_API_ENTRY vx_tensor_view VX_API_CALL vxCreateTensorView(vx_context context, vx_uint32 *view_array_start, vx_uint32 * view_array_end, vx_uint8 numViewDimensions);

/*! \brief Releases a reference to a tensor data view object.
* The object may not be garbage collected until its total reference count is zero.
* \param [in] tensor_view The pointer to the tensor data view to release.
* \post After returning from this function the reference is zeroed.
* \return A <tt>vx_status_e</tt> enumeration.
* \retval VX_SUCCESS No errors.
* \retval VX_SUCCESS Success
* \retval * An error occurred. See <tt>vx_status_e</tt>.
* \ingroup group_tensor
*/
VX_API_ENTRY vx_status VX_API_CALL vxReleaseTensorView(vx_tensor_view *tensor_view);

/*! \brief Create  an opaque reference to a tensor addressing object.
* \details Not guaranteed to exist until the <tt>vx_graph</tt> containing it has been verified.
* \param [in] context The reference to the implementation context.
* \param [in] addressing_array_dimension a vx_uint32 array of sLength of patch in all dimensions in elements.
* \param [in] addressing_array_stride a vx_uint32 arrayStride in all dimensions in bytes.
* \param [in] numViewDimensions number of dimensions of view_array_start and view_array_end.
* \return A tensor data view reference or zero when an error is encountered.
* \ingroup group_tensor
*/
VX_API_ENTRY vx_tensor_addressing VX_API_CALL vxCreateTensorAddressing(vx_context context, vx_uint32 *addressing_array_dimension, vx_uint32 * addressing_array_stride, vx_uint8 numViewDimensions);

/*! \brief Releases a reference to a tensor data addressing object.
* The object may not be garbage collected until its total reference count is zero.
* \param [in] tensor_addr The pointer to the tensor data addressing to release.
* \post After returning from this function the reference is zeroed.
* \return A <tt>vx_status_e</tt> enumeration.
* \retval VX_SUCCESS No errors.
* \retval VX_SUCCESS Success
* \retval * An error occurred. See <tt>vx_status_e</tt>.
* \ingroup group_tensor
*/
VX_API_ENTRY vx_status VX_API_CALL vxReleaseTensorAddressing(vx_tensor_addressing *tensor_addr);

/*==============================================================================
    NN Nodes
=============================================================================*/
/*! \brief [Graph] Creates a Convolutional Network Convolution Layer Node.
* \details This function implement Convolutional Network Convolution layer.
* In case the input and output <tt>\ref vx_tensor</tt> are signed 16. A fixed point calculation is performed with round and saturate according to the number of accumulator bits. \n
* round: rounding according the <tt>vx_round_policy_e</tt> enumeration. \n
* saturate: A saturation according the <tt>vx_convert_policy_e</tt> enumeration.
* The saturation is done based on the accumulator_bits parameter.
* According the accumulator_bits, the saturation might not be performed every operation. 
* But every a specified amount of operations, 
* that are suspected to saturate the accumulation bits\n
* The following equation is implemented: \n
* \f$ outputs[j,k,i] = (\sum_{l} \sum_{m,n} saturate(round(inputs[j-m,k-n,l] \times weights[m,n,l,i])))+biasses[j,k,i] \f$\n
* Where \f$m,n\f$ are indexes on the convolution matrices. \f$ l\f$ is an index on all the convolutions per input.\f$ i\f$ is an index per output.
* \f$ j,k \f$ are the inputs/outputs spatial indexes.
* Convolution is done on the first 2 dimensions of the <tt>\ref vx_tensor</tt>. Therefore, we use here the term x for the first dimension and y for the second.\n
* before the Convolution is done, a padding of the first 2D with zeros is performed.
* Then down scale is done by picking the results according to a skip jump. The skip in the x and y dimension is determined by the output size dimensions.
* The relation between input to output is as follows: \n
* \f$ width_{output} = round(\frac{(width + 2 * pad_x - kernel_x)}{skip_x} + 1) \f$\n
* and \n 
* \f$ height_{output} = round(\frac{(height + 2 * pad_y - kernel_y)}{skip_y} + 1) \f$\n
* where \f$width\f$ is the size of the first input dimension. \f$height\f$ is the size of the second input dimension.
* \f$width_{output}\f$ is the size of the first output dimension. \f$height_{output}\f$ is the size of the second output dimension.
* \f$kernel_x\f$ and \f$kernel_y\f$ are the convolution sizes in x and y.
* skip is calculated by the relation between input and output.
* rounding is done according to <tt>\ref vx_convolutional_network_rounding_type_e</tt>. 
* \param [in] graph The handle to the graph.
* \param [in] inputs The input tensor data. 3 lower dims represent a single input, and an optional 4th dimension for batch of inputs.\n
* \param [in] weights Weights are 4d tensor with dimensions [kernel_x, kernel_y, #IFM, #OFM].\n
* \param [in] biases The biases, which may be shared (one per ofm) or unshared (one per ofm * output location).
* \param [in] pad_x Number of elements added at each side in the x dimension of the input.
* \param [in] pad_y Number of elements added at each side in the y dimension of the input. In fully connected layers this input is ignored.
* \param [in] accumulator_bits Is the total number of bits used during intermediate accumulation.
* \param [in] overflow_policy A <tt> VX_TYPE_ENUM</tt> of the <tt> vx_convert_policy_e</tt> enumeration.
* \param [in] rounding_policy A <tt> VX_TYPE_ENUM</tt> of the <tt> vx_round_policy_e</tt> enumeration.
* \param [in] down_scale_size_rounding Rounding method for calculating output dimensions. See <tt>\ref vx_convolutional_network_rounding_type_e</tt>
* \param [out] outputs The output tensor data. Output will have the same number of dimensions as input.
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
* \ingroup group_cnn
*/
VX_API_ENTRY vx_node VX_API_CALL vxConvolutionLayer(vx_graph graph, vx_tensor inputs, vx_tensor weights, vx_tensor biases,
    vx_uint32 pad_x,
    vx_uint32 pad_y,
	vx_uint8 accumulator_bits, 
	vx_enum overflow_policy, 
	vx_enum rounding_policy,
    vx_enum down_scale_size_rounding,	
    vx_tensor outputs);

/*! \brief [Graph] Creates a Fully connected Convolutional Network Layer Node.
* \details This function implement Fully connected Convolutional Network layers.
* In case the input and output <tt>\ref vx_tensor</tt> are signed 16. A fixed point calculation is performed with round and saturate according to the number of accumulator bits. \n
* round: rounding according the <tt>vx_round_policy_e</tt> enumeration. \n
* saturate: A saturation according the <tt>vx_convert_policy_e</tt> enumeration.
* The saturation is done based on the accumulator_bits parameter.
* According the accumulator_bits, the saturation might not be performed every operation. 
* But every a specified amount of operations, 
* that are suspected to saturate the accumulation bits\n
* The equation for Fully connected layer:\n
* \f$ outputs[i] = ( \sum_{j} saturate(round(inputs[j] \times weights[j,i])))+biasses[i] \f$\n
* Where \f$j\f$ is a index on the input feature and \f$i\f$ is a index on the output.
* before the fully connected is done, a padding of the input is performed.
* Then down scale is done by picking the results according to a skip jump. The skip is determined by the output size dimensions.
* The relation between input to output is as follows:
* \f$ size_{output} = round(\frac{(size_{input} + 2 * pad)}{skip} + 1) \f$\n
* where \f$size_{input}\f$ is the size of the input dimension. 
* \f$size_{output}\f$ is the size of the output dimension. 
* skip is calculated by the relation between input and output.
* rounding is done according to <tt>\ref vx_convolutional_network_rounding_type_e</tt>. 
* \param [in] graph The handle to the graph.
* \param [in] inputs The input tensor data. 1-3 lower dims represent a single input, and all dims above dim(weights)-1 are optional for batch of inputs. Note that batch may be multidimensional.
* \param [in] weights Number of dimensions equals dim(single input)+1. Single input dims are [width, height, #IFM], with height and #IFM being optional.\n
* \param [in] biases The biases, which may be shared (one per ofm) or unshared (one per ofm * output location).
* \param [in] pad Number of elements added at each side in the input.
* \param [in] accumulator_bits Is the total number of bits used during intermediate accumulation.
* \param [in] overflow_policy A <tt> VX_TYPE_ENUM</tt> of the <tt> vx_convert_policy_e</tt> enumeration.
* \param [in] rounding_policy A <tt> VX_TYPE_ENUM</tt> of the <tt> vx_round_policy_e</tt> enumeration.
* \param [in] down_scale_size_rounding Rounding method for calculating output dimensions. See <tt>\ref vx_convolutional_network_rounding_type_e</tt>
* \param [out] outputs The output tensor data. Output will have the same number of dimensions as input.
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
* \ingroup group_cnn
*/
VX_API_ENTRY vx_node VX_API_CALL vxFullyConnectedLayer(vx_graph graph, vx_tensor inputs, vx_tensor weights, vx_tensor biases,
    vx_uint32 pad,
	vx_uint8 accumulator_bits, 
	vx_enum overflow_policy, 
	vx_enum rounding_policy, 
    vx_enum down_scale_size_rounding,	
    vx_tensor outputs);

/*! \brief [Graph] Creates a Locally Connected Convolutional Network Layer Node.
* \details This function implement Locally Connected Convolutional Network layers.
* In case the input and output <tt>\ref vx_tensor</tt> are signed 16. A fixed point calculation is performed with round and saturate according to the number of accumulator bits. \n
* round: rounding according the <tt>vx_round_policy_e</tt> enumeration. \n
* saturate: A saturation according the <tt>vx_convert_policy_e</tt> enumeration.
* The saturation is done based on the accumulator_bits parameter.
* According the accumulator_bits, the saturation might not be performed every operation. 
* But every a specified amount of operations, 
* that are suspected to saturate the accumulation bits\n
* The equation for Locally connected layer: \n
* \f$ outputs[j,k,i] = (\sum_{l} \sum_{m,n} saturate(round(inputs[j-m,k-n,l] \times weights[m,n,l,i,j,k])))+biasses[j,k,i] \f$\n
* Where \f$m,n\f$ are indexes on the convolution matrices. \f$ l\f$ is an index on all the convolutions per input.\f$ i\f$ is an index per output.
* \f$ j,k\f$ are the inputs/outputs spatial indexes.
* Convolution is done on the first 2 dimensions of the <tt>\ref vx_tensor</tt>. Therefore, we use here the term x for the first dimension and y for the second.\n
* before the Convolution is done, a padding of the first 2D with zeros is performed.
* Then down scale is done by picking the results according to a skip jump. The skip in the x and y dimension is determined by the output size dimensions.
* The relation between input to output is as follows:
* \f$ width_{output} = round(\frac{(width + 2 * pad_x - kernel_x)}{skip_x} + 1) \f$\n
* and \n 
* \f$ height_{output} = round(\frac{(height + 2 * pad_y - kernel_y)}{skip_y} + 1) \f$\n
* where \f$width\f$ is the size of the first input dimension. \f$height\f$ is the size of the second input dimension.
* \f$width_{output}\f$ is the size of the first output dimension. \f$height_{output}\f$ is the size of the second output dimension.
* \f$kernel_x\f$ and \f$kernel_y\f$ are the convolution sizes in x and y.
* skip is calculated by the relation between input and output.
* rounding is done according to <tt>\ref vx_convolutional_network_rounding_type_e</tt>. 
* \param [in] graph The handle to the graph.
* \param [in] inputs The input tensor data. 3 lower dims represent a single input, and an optional 4th dimension for batch of inputs.\n
* \param [in] weights 6d weight (one 6d-weight-cube per kernelx*kernely#ofm*#ifm*inputx*inputy).\n
* \param [in] biases The biases, which may be shared (one per ofm) or unshared (one per ofm * output location).
* \param [in] pad_x Number of elements added at each side in the x dimension of the input.
* \param [in] pad_y Number of elements added at each side in the y dimension of the input. In fully connected layers this input is ignored.
* \param [in] accumulator_bits Is the total number of bits used during intermediate accumulation.
* \param [in] overflow_policy A <tt> VX_TYPE_ENUM</tt> of the <tt> vx_convert_policy_e</tt> enumeration.
* \param [in] rounding_policy A <tt> VX_TYPE_ENUM</tt> of the <tt> vx_round_policy_e</tt> enumeration.
* \param [in] down_scale_size_rounding Rounding method for calculating output dimensions. See <tt>\ref vx_convolutional_network_rounding_type_e</tt>
* \param [out] outputs The output tensor data. Output will have the same number of dimensions as input.
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
* \ingroup group_cnn
*/
VX_API_ENTRY vx_node VX_API_CALL vxLocallyConnectedLayer(vx_graph graph, vx_tensor inputs, vx_tensor weights, vx_tensor biases,
    vx_uint32 pad_x,
    vx_uint32 pad_y,
	vx_uint8 accumulator_bits, 
	vx_enum overflow_policy, 
	vx_enum rounding_policy, 
    vx_enum down_scale_size_rounding,	
    vx_tensor outputs);	
	
/*! \brief [Graph] Creates a Convolutional Network Pooling Layer Node.
 * \details Pooling is done on the first 2 dimensions or the <tt>\ref vx_tensor</tt>. Therefore, we use here the term x for the first dimension and y for the second.\n
 * Pooling operation is a function operation over a rectangle size and then a nearest neighbour down scale.
 * Here we use pool_size_x and pool_size_y to specify the rectangle size on which the operation
 * is performed. \n
 * before the operation is done (average or maximum value). the data is padded in the first 2D with zeros.
 * The down scale is done by picking the results according to a skip jump. The skip in the x and y dimension is determined by the output size dimensions.
* \param [in] graph The handle to the graph.
* \param [in] inputs The input tensor data. 3 lower dims represent a single input with dimensions [width, height, IFM], and an optional 4th dimension for batch of inputs.
* \param [in] pool_type Either max pooling or average pooling (see <tt>\ref vx_convolutional_network_pooling_type_e</tt>).
* \param [in] pool_size_x Size of the pooling region in the x dimension
* \param [in] pool_size_y Size of the pooling region in the y dimension. 
* \param [in] pool_pad_x Padding size in the x dimension. 
* \param [in] pool_pad_y Padding size in the y dimension.
* \param [in] rounding, Rounding method for calculating output dimensions. See <tt>\ref vx_convolutional_network_rounding_type_e</tt>
* \param [out] outputs The output tensor data. Output will have the same number of dimensions as input.
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
* \ingroup group_cnn
*/
VX_API_ENTRY vx_node VX_API_CALL vxPoolingLayer(vx_graph graph, vx_tensor inputs, vx_enum pool_type, 
    vx_uint32 pool_size_x,
    vx_uint32 pool_size_y,
    vx_uint32 pool_pad_x,
    vx_uint32 pool_pad_y,
    vx_enum rounding,
    vx_tensor outputs);

/*! \brief [Graph] Creates a Convolutional Network Softmax Layer Node.
* \param [in] graph The handle to the graph.
* \param [in] inputs The input tensor data, with number of dimensions equals dim(input batch) + 1. Softmax will be calculated per IFM.
* \param [out] outputs The output tensor data. Output will have the same number of dimensions as input.
* \ingroup group_cnn
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
*/
VX_API_ENTRY vx_node VX_API_CALL vxSoftmaxLayer(vx_graph graph, vx_tensor inputs, vx_tensor outputs);

/*! \brief [Graph] Creates a Convolutional Network Normalization Layer Node.
* \details Normalizing over local input regions. Each input value is divided by \f$ (1+\frac{\alpha}{n}\sum_i x^2_i)^\beta \f$ , where n is the number of elements to normalize across.
* and the sum is taken over the region centred at that value (zero padding is added where necessary).
* \param [in] graph The handle to the graph.
* \param [in] inputs The input tensor data. 3 lower dims represent a single input with dimensions [width, height, IFM], and an optional 4th dimension for batch of inputs.
* \param [in] type Either same map or across maps (see vx_convolutional_network_norm_type_e).
* \param [in] norm_size Number of elements to normalize across.
* \param [in] alpha Alpha parameter in the normalization equation.
* \param [in] beta  Beta parameter in the normalization equation.
* \param [out] outputs The output tensor data. Output will have the same number of dimensions as input.
* \ingroup group_cnn
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
*/
VX_API_ENTRY vx_node VX_API_CALL vxNormalizationLayer(vx_graph graph, vx_tensor inputs, vx_enum type,
    vx_uint32 norm_size,
    vx_float32 alpha,
    vx_float32 beta,
    vx_tensor outputs);

/*! \brief [Graph] Creates a Convolutional Network Activation Layer Node.
* \param [in] graph The handle to the graph.
* \param [in] inputs The input tensor data.
* \param [in] func Non-linear function (see <tt>\ref vx_convolutional_network_activation_func_e</tt>).
* \param [in] a Function parameters a. (see <tt>\ref vx_convolutional_network_activation_func_e</tt>).
* \param [in] b Function parameters b. (see <tt>\ref vx_convolutional_network_activation_func_e</tt>).
* \param [out] outputs The output tensor data. Output will have the same number of dimensions as input.
* \ingroup group_cnn
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
*/
VX_API_ENTRY vx_node VX_API_CALL vxActivationLayer(vx_graph graph, vx_tensor inputs, vx_enum func, vx_int32 a,vx_int32 b, vx_tensor outputs);




/*! \brief [Graph] Performs element wise multiplications on element values in the input tensor data's with a scale.
* \param [in] graph The handle to the graph.
* \param [in] in1 input tensor data.
* \param [in] in2 input tensor data, inputs must be of equal in dimensions.
* else, If in one of the vx_mddata dimension is 1.
* That dimension is considered as a const on all the dimension terms.
* And will perform as if the values are duplicated on all terms in that dimensions.
* After the expansion. The dimensions are equal. 
* \param [in] scale The scale value.
* \param [in] overflow_policy A <tt>vx_convert_policy_e</tt> enumeration.
* \param [in] rounding_policy A <tt>vx_round_policy_e</tt> enumeration.
* \param [out] out The output tensor data with the same dimensions as the input tensor data's.
* \ingroup group_tensor
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
*/
VX_API_ENTRY vx_node VX_API_CALL vxTensorMultiplyNode(vx_graph graph, vx_tensor in1, vx_tensor in2, vx_scalar scale, vx_enum overflow_policy, vx_enum rounding_policy, vx_tensor out);

/*! \brief [Graph] Performs arithmetic addition on element values in the input tensor data's.
 * \param [in] graph The handle to the graph.
 * \param [in] in1 input tensor data,. 
 * \param [in] in2 input tensor data, inputs must be of equal in dimensions.
 * else, If in one of the vx_mddata dimension is 1.
 * That dimension is considered as a const on all the dimension terms.
 * And will perform as if the values are duplicated on all terms in that dimensions.
 * After the expansion. The dimensions are equal.
 * \param [in] policy A vx_convert_policy_e enumeration.
 * \param [out] out The output tensor data with the same dimensions as the input tensor data's.
 * \ingroup group_tensor
 * \return <tt> vx_node</tt>.
 * \retval 0 Node could not be created.
 * \retval * Node handle.
 */
VX_API_ENTRY vx_node VX_API_CALL vxTensorAddNode(vx_graph graph, vx_tensor in1, vx_tensor in2, vx_enum policy, vx_tensor out);

/*! \brief [Graph] Performs arithmetic subtraction on element values in the input tensor data's.
* \param [in] graph The handle to the graph.
* \param [in] in1 input tensor data.
* \param [in] in2 input tensor data, inputs must be of equal in dimensions.
* else, If in one of the vx_mddata dimension is 1.
* That dimension is considered as a const on all the dimension terms.
* And will perform as if the values are duplicated on all terms in that dimensions.
* After the expansion. The dimensions are equal.
* \param [in] policy A vx_convert_policy_e enumeration.
* \param [out] out The output tensor data with the same dimensions as the input tensor data's.
* \ingroup group_tensor
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
*/
VX_API_ENTRY vx_node VX_API_CALL vxTensorSubtractNode(vx_graph graph, vx_tensor in1, vx_tensor in2, vx_enum policy, vx_tensor out);

/*! \brief [Graph] Performs LUT on element values in the input tensor data's.
* \param [in] graph The handle to the graph.
* \param [in] in1 input tensor data.
* \param [in] lut of type <tt>vx_lut</tt>
* \param [out] out The output tensor data with the same dimensions as the input tensor data's.
* \ingroup group_tensor
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
*/
VX_API_ENTRY vx_node VX_API_CALL vxTensorTableLookupNode(vx_graph graph, vx_tensor in1, vx_lut lut, vx_tensor out);


/*! \brief [Graph] Performs transpose on the input tensor.
* The node transpose the tensor according to a specified 2 indexes in the tensor (0-based indexing)
* \param [in] graph The handle to the graph.
* \param [in] in input tensor data,
* \param [out] out output tensor data,
* \param [in] dim1 that is transposed with dim 2.
* \param [in] dim2 that is transposed with dim 1.
* \ingroup group_tensor
* \return <tt> vx_node</tt>.
* \retval 0 Node could not be created.
* \retval * Node handle.
*/
VX_API_ENTRY vx_status VX_API_CALL vxTensorTransposeNode(vx_graph graph, vx_tensor in, vx_tensor out, vx_uint32 dim1, vx_uint32 dim2);




#endif
