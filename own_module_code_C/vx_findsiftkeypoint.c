/*
 * Copyright (c) 2012-2014 The Khronos Group Inc.
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
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

/*!
 * \file
 * \brief The Filter Kernels.
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_extras.h>
#include <VX/vx_lib_debug.h>
#include <vx_internal.h>

static vx_status VX_CALLBACK vxFindSiftKeypointKernel(vx_node node, vx_reference *parameters, vx_uint32 num)
{
    if (num == 4)
    {
		vx_image input1 = (vx_image)parameters[0];
		vx_image input2 = (vx_image)parameters[1];
		vx_image input3 = (vx_image)parameters[2];
		vx_array arr = (vx_array)parameters[3];

		//vx_reference DOG_pyramid_ref = (vx_reference)parameters[0];
		//vx_image*  DOG_pyramid = (vx_image*)DOG_pyramid_ref;
		//vx_array keypoints = (vx_array)parameters[1];

		
		vx_coordinates2d_t a1;
		vx_coordinates2d_t a2;
		vx_coordinates2d_t a3;
		a1.x = (vx_uint32)111;
		a1.y = (vx_uint32)333;
		a2.x = (vx_uint32)2222;
		a2.y = (vx_uint32)4444;
		a3.x = (vx_uint32)66;
		a3.y = (vx_uint32)55;

		vxAddArrayItems(arr, 1, &a1, 0);
		vxAddArrayItems(arr, 1, &a2, 0);
		vxAddArrayItems(arr, 1, &a3, 0);
		//

		//기존 모듈 같았더라면 이 위치에 return vxFindSiftKeypoint(vx_image*, vx_array) 가 들어갈 것이다.

		return VX_SUCCESS;
    }
    
	return VX_ERROR_INVALID_PARAMETERS;
}

static vx_status VX_CALLBACK vxFindSiftKeypointInputValidator(vx_node node, vx_uint32 index)
{
	vx_status status = VX_ERROR_INVALID_PARAMETERS;
	if (index == 0 | index == 1 | index == 2)
	{
		vx_image input = 0;
		vx_parameter param = vxGetParameterByIndex(node, index);

		vxQueryParameter(param, VX_PARAMETER_ATTRIBUTE_REF, &input, sizeof(input));
		if (input)
		{
			vx_df_image format = 0;
			vxQueryImage(input, VX_IMAGE_ATTRIBUTE_FORMAT, &format, sizeof(format));
			if ((format == VX_DF_IMAGE_U8)
				|| (format == VX_DF_IMAGE_S16)
#if defined(EXPERIMENTAL_USE_S16)
				|| (format == VX_DF_IMAGE_U16)
				|| (format == VX_DF_IMAGE_U32)
				|| (format == VX_DF_IMAGE_S32)
#endif
				)
			{
				status = VX_SUCCESS;
			}
			vxReleaseImage(&input);
		}
		vxReleaseParameter(&param);
	}
	return status;
}

static vx_status VX_CALLBACK vxFindSiftKeypointOutputValidator(vx_node node, vx_uint32 index, vx_meta_format_t *ptr)
{

    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (index == 3)
    {
        ptr->type = VX_TYPE_ARRAY;
		ptr->dim.array.item_type = VX_TYPE_COORDINATES2D;
        ptr->dim.array.capacity = 1000; 
		//no defined capacity requirement
        status = VX_SUCCESS;
    }

    return status;
	
}


static vx_param_description_t findsiftkeypoint_kernel_params[] = {
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_OPTIONAL }, 
};

vx_kernel_description_t findsiftkeypoint_kernel = {
	VX_KERNEL_FINDSIFTKEYPOINT,
	"org.khronos.openvx.nothing",
	vxFindSiftKeypointKernel,
	findsiftkeypoint_kernel_params, dimof(findsiftkeypoint_kernel_params),
	vxFindSiftKeypointInputValidator,
	vxFindSiftKeypointOutputValidator,
	NULL,
	NULL,
};

