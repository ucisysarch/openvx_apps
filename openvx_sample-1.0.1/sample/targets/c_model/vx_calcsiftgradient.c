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


#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_extras.h>
#include <VX/vx_lib_debug.h>
#include <vx_internal.h>

#include <stdlib.h>
#include <stdio.h>

static vx_status VX_CALLBACK vxCalcSiftGradientKernel(vx_node node, vx_reference *parameters, vx_uint32 num)
{
	if (num == 4)
	{
		//parameters
		vx_image orien = (vx_image)parameters[0];
		vx_image mag = (vx_image)parameters[1];
		vx_array keypts = (vx_array)parameters[2];
		vx_array descr = (vx_array)parameters[3];


		//some important value. Because we're not handling radian or degree value.
		//Rather we'll deal with 0~255 values which have been mapped from 0~6.28 radian values.


		//FILE* fff = NULL;
		//fff = fopen("www3.txt", "a+");



		//neeeded kernels for small convolution.
		//convolution matrix for entire window. sigma 1.5 (keypoint orientation)
		vx_float32 window_convolution_mat[3][3] = {
			{ 0.095332f, 0.118095f, 0.095332f },
			{ 0.118095f, 0.146293f, 0.118095f },
			{ 0.095332f, 0.118095f, 0.095332f },

		};



		//convolution matrix for each patch (descriptor orientation)
		vx_float32 patch_convolution_mat[3][3] = {
			{ 0.110533f, 0.111399f, 0.110533f },
			{ 0.111399f, 0.112271f, 0.111399f },
			{ 0.110533f, 0.111399f, 0.110533f }

		};

		//access to keypoint array
		vx_size num_keypts = 0;
		vx_size stride = 0ul;
		void* base = 0;
		//vx_coordinates2d_t foundKey;
		vx_int32 kpt_x, kpt_y;

		vxQueryArray(keypts, VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_keypts, sizeof(num_keypts));
		//vx_status st = vxAccessArrayRange(keypts, (vx_size)0, (vx_size)num_keypts, &stride, (void**)&base, VX_READ_ONLY);
		vxAccessArrayRange(keypts, (vx_size)0, (vx_size)num_keypts, &stride, (void**)&base, VX_READ_ONLY);

		//if no keypoints were contained in array, just return
		if (num_keypts <= 0)
		{
			return VX_SUCCESS;
		}

		//fprintf(fff, "<%d>\n", num_keypts);

		for (int kptidx = 0; kptidx < num_keypts; kptidx++)
		{
			//retrieve a keypoint from array
			vx_coordinates2d_t* xp = &vxArrayItem(vx_coordinates2d_t, base, kptidx, stride);
			kpt_x = xp->x; kpt_y = xp->y;

			//=================================================loop?

			// @ ----- orientation of keypoint
			vx_float32 keypoint_orient;

			// @ ----- a window containing 16 bits around a keypoint
			// but, at first, orientations arr saved as unsigned 8 bit.(0~255)
			// So we need to convert them to radian float. (0~255 => 0~6.28)
			vx_float32 oriWin[16][16];		// - orientation values
			vx_float32 magWin[16][16];		// - magnitude values

			vx_float32 blurred_magWin[16][16];		// - blurred(convoluted) magnitude values for patches

			vx_uint32 w, h;	//images' width&height. Assume size of two images(orientation/magnitude) are the same
			vxQueryImage(orien, VX_IMAGE_ATTRIBUTE_WIDTH, &w, sizeof(w));
			vxQueryImage(orien, VX_IMAGE_ATTRIBUTE_HEIGHT, &h, sizeof(h));

			// @ ----- get 16 ORIENTATION pixels around keypoint to make 2d array
			//patch for access gradient orientation image
			vx_rectangle_t ori_imrect;
			vx_uint32 ori_plane = 0;
			vx_imagepatch_addressing_t ori_imaddr;
			void* ori_imbaseptr = NULL;

			//setting patch as entire size of image
			ori_imrect.start_x = ori_imrect.start_y = 0;
			ori_imrect.end_x = w; ori_imrect.end_y = h;

			//access -> fill -> commit
			vxAccessImagePatch(orien, &ori_imrect, ori_plane, &ori_imaddr, &ori_imbaseptr, VX_READ_ONLY);

			for (int y = -7; y <= 8; y++)
			{
				for (int x = -7; x <= 8; x++)
				{
					vx_uint8* currpixel = (vx_uint8 *)vxFormatImagePatchAddress2d(ori_imbaseptr, kpt_x + x, kpt_y + y, &ori_imaddr);
					vx_float32 pixel_value = (vx_float32)(*currpixel);
					pixel_value *= 6.28f;
					pixel_value /= 255.0f;
					if (pixel_value >= 6.28f) pixel_value = 0.0f;	//2*PHI == 0

					oriWin[7 + y][7 + x] = pixel_value;
				}

			}
			
			vxCommitImagePatch(orien, &ori_imrect, ori_plane, &ori_imaddr, ori_imbaseptr);

			// @ ----- get 16 MAGNITUDE pixels around keypoint to make 2d array
			//patch for access gradient magnitude image
			vx_rectangle_t mag_imrect;
			vx_uint32 mag_plane = 0;
			vx_imagepatch_addressing_t mag_imaddr;
			void* mag_imbaseptr = NULL;

			//setting patch as entire size of image
			mag_imrect.start_x = mag_imrect.start_y = 0;
			mag_imrect.end_x = w; mag_imrect.end_y = h;

			//access -> fill -> commit
			vxAccessImagePatch(mag, &mag_imrect, mag_plane, &mag_imaddr, &mag_imbaseptr, VX_READ_ONLY);

			for (int y = -7; y <= 8; y++)
			{
				for (int x = -7; x <= 8; x++)
				{
					vx_int16* currpixel = (vx_int16 *)vxFormatImagePatchAddress2d(mag_imbaseptr, kpt_x + x, kpt_y + y, &mag_imaddr);
					magWin[7 + y][7 + x] = (*currpixel);
				}

			}
			vxCommitImagePatch(mag, &mag_imrect, mag_plane, &mag_imaddr, mag_imbaseptr);


			// @ ----- do gaussian on magnitude values and fill histogram
			vx_float32 histogram_36[36] = { 0, };

			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					vx_float32 sum = 0;

					for (int cr = -1; cr <= 1; cr++)
					{
						for (int cc = -1; cc <= 1; cc++)
						{
							//skip if we convolute nearby border
							if ((i + cr >= 0 && i + cr <= 15) && (j + cc >= 0 && j + cc <= 15))
								sum += magWin[i + cr][j + cc] * window_convolution_mat[1 + cr][1 + cc];
						}
					}

					blurred_magWin[i][j] = sum;

					//the index is subject to containing orientation.
					//it must be 0~35 (over 35 == 0)
					int ori_idx = ((int)(oriWin[i][j] * 360 / 6.28f) / 10);
					if (ori_idx > 35) ori_idx = 0;

					//add to histogram array, indexing will be done with following i, j
					histogram_36[ori_idx] += sum;

				}
			}


			// @ ----- make HISTOGRAM from 16*16 window : 36 bins and pick dominant orientation of keypoint

			//travel oriWin and check corresponding blurred_magWin
			vx_float32 maxmag = 0.0f;
			vx_int32 maxidx = 0;
			for (int i = 0; i < 36; i++)
			{
				if (histogram_36[i] > maxmag)
				{
					maxmag = histogram_36[i];
					maxidx = i;
				}
			}
			//dominant orientation of keypoint
			//10 : 360 = 1.744f : 6.28f 
			keypoint_orient = (vx_float32)(0.1744f * maxidx);


			// @ ----- do gaussian on magnitude values for patch gaussian again
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					vx_float32 sum = 0;

					for (int cr = -1; cr <= 1; cr++)
					{
						for (int cc = -1; cc <= 1; cc++)
						{
							//skip if we convolute nearby border
							if ((i + cr >= 0 && i + cr <= 15) && (j + cc >= 0 && j + cc <= 15))
								sum += magWin[i + cr][j + cc] * patch_convolution_mat[1 + cr][1 + cc];
						}
					}

					//now, blurred_magWin contains blurred for patches.
					//(Before, it was containing blurred for window)
					blurred_magWin[i][j] = sum;


				}
			}

			// @ ----- subtract keypoint dominant orientation from orientation window.
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					//every orientation values must be positibe value.
					oriWin[i][j] -= keypoint_orient;
					if (oriWin[i][j] < 0.0f) oriWin[i][j] += 6.28f;
				}
			}


			// @ ----- assume 4*4 patches
			vx_float32 descriptor_arr[128];
			int descr_idx = 0;

			// @ ----- make HISTOGRAM from 4*4 patches : 8 bins 
			// @ ----- 8 values per patch -> 4*4*8 => 128 values per keypoint
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					vx_float32 patch_histogram[8] = { 0.0f, };

					//a patch.
					for (int pi = 0; pi < 4; pi++)
					{
						for (int pj = 0; pj < 4; pj++)
						{
							int ori_idx = (int)(oriWin[i * 4 + pi][j * 4 + pj] / 0.785f);
							if (ori_idx > 7) ori_idx = 0;

							patch_histogram[ori_idx] += blurred_magWin[i * 4 + pi][j * 4 + pj];
						}
					}

					// 45 : 360 = 0.785f : 6.28f

					//[0] = 0~44		[1] = 45~89				[2] = 90~134
					//[3] = 135~179		[4] = 180~224			[5] = 225~269
					//[6] = 270~314		[7] = 315~359	

					//concat patch_histogram (8 bytes) to descriptor_arr (128 bytes)
					for (int di = 0; di < 8; di++)
						descriptor_arr[descr_idx * 8 + di] = patch_histogram[di];

					descr_idx++;

				}
			}

			// @ ----- add to array
			for (int di = 0; di < 128; di++)
			{
				vxAddArrayItems(descr, 1, &descriptor_arr[di], sizeof(vx_float32));
				//fprintf(fff, "%f ", descriptor_arr[di]);
			}

			//fprintf(fff, "\n");

		}

		vxCommitArrayRange(keypts, 0, num_keypts, base);


		//fclose(fff);



		return VX_SUCCESS;
    }
    
	return VX_ERROR_INVALID_PARAMETERS;
}

static vx_status VX_CALLBACK vxCalcSiftGradientInputValidator(vx_node node, vx_uint32 index)
{
	vx_status status = VX_ERROR_INVALID_PARAMETERS;
	if (index == 0 || index == 1)
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
	
	if (index == 2)
	{
		vx_parameter param = vxGetParameterByIndex(node, index);
		if (param)
		{
			vx_array arr = 0;
			vxQueryParameter(param, VX_PARAMETER_ATTRIBUTE_REF, &arr, sizeof(arr));
			if (arr)
			{
				vx_enum item_type = 0;
				vxQueryArray(arr, VX_ARRAY_ATTRIBUTE_ITEMTYPE, &item_type, sizeof(item_type));
				if (item_type == VX_TYPE_COORDINATES2D)
				{
					status = VX_SUCCESS;
				}
				vxReleaseArray(&arr);
			}
			vxReleaseParameter(&param);
		}
	}

	
	return status;
}

static vx_status VX_CALLBACK vxCalcSiftGradientOutputValidator(vx_node node, vx_uint32 index, vx_meta_format_t *ptr)
{

    vx_status status = VX_ERROR_INVALID_PARAMETERS;
	if (index == 3)
	{
		ptr->type = VX_TYPE_ARRAY;
		ptr->dim.array.item_type = VX_TYPE_FLOAT32;
		ptr->dim.array.capacity = 30000;
		//no defined capacity requirement
		status = VX_SUCCESS;
	}
    

    return status;
	
}


static vx_param_description_t calcsiftgradient_kernel_params[] = {
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED },
	{ VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_OPTIONAL },
};

vx_kernel_description_t calcsiftgradient_kernel = {
	VX_KERNEL_CALCSIFTGRADIENT,
	"org.khronos.openvx.nothing",
	vxCalcSiftGradientKernel,
	calcsiftgradient_kernel_params, dimof(calcsiftgradient_kernel_params),
	vxCalcSiftGradientInputValidator,
	vxCalcSiftGradientOutputValidator,
	NULL,
	NULL,
};

