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

/*!
 * \file
 * \brief The Fill Object Kernels.
 * \author Erik Rainey <erik.rainey@gmail.com>
 * \defgroup group_debug_ext Debugging Extension
 */

#include <VX/vx.h>
#include <VX/vx_lib_debug.h>
#include <VX/vx_helper.h>

static
vx_param_description_t fill_image_kernel_params[] =
{
    { VX_INPUT,  VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
    { VX_OUTPUT, VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED },
};

static
vx_status VX_CALLBACK ownFillImageKernel(vx_node node, const vx_reference parameters[], vx_uint32 num)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (NULL != node && NULL != parameters && num == dimof(fill_image_kernel_params))
    {
        vx_int32 i = 0;
        vx_uint32 x = 0u;
        vx_uint32 y = 0u;
        vx_uint32 p = 0u;
        vx_scalar fill = (vx_scalar)parameters[0];
        vx_image image = (vx_image)parameters[1];
        vx_pixel_value_t value;
        vx_uint32 count = 0u;
        vx_size planes = 0u;

        value.reserved[0] = 0xDE;
        value.reserved[1] = 0xAD;
        value.reserved[2] = 0xBE;
        value.reserved[3] = 0xEF;

        status = VX_SUCCESS;

        status |= vxCopyScalar(fill, &value.U32, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
        status |= vxQueryImage(image, VX_IMAGE_PLANES, &planes, sizeof(planes));

        for (p = 0u; p < planes; p++)
        {
            void* ptr = NULL;
            vx_imagepatch_addressing_t addr = VX_IMAGEPATCH_ADDR_INIT;
            vx_map_id map_id = 0;
            vx_rectangle_t rect;

            status |= vxGetValidRegionImage(image, &rect);

            status |= vxMapImagePatch(image, &rect, p, &map_id, &addr, &ptr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);

            if ((status == VX_SUCCESS) && (ptr))
            {
                for (y = 0u; y < addr.dim_y; y += addr.step_y)
                {
                    for (x = 0u; x < addr.dim_x; x += addr.step_x)
                    {
                        vx_uint8* pixel = vxFormatImagePatchAddress2d(ptr, x, y, &addr);
                        for (i = 0; i < addr.stride_x; i++)
                        {
                            pixel[i] = value.reserved[i];
                            count++;
                        }
                    }
                }

                status |= vxUnmapImagePatch(image, map_id);
                if (status != VX_SUCCESS)
                {
                    vxAddLogEntry((vx_reference)node, VX_FAILURE, "Failed to set image patch for "VX_FMT_REF"\n", image);
                }
            }
            else
            {
                vxAddLogEntry((vx_reference)node, VX_FAILURE, "Failed to get image patch for "VX_FMT_REF"\n", image);
            }
        } // for planes
    } // if ptrs non NULL

    return status;
} /* ownFillImageKernel() */

static
vx_status VX_CALLBACK own_fill_image_validator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (NULL != node && NULL != parameters && num == dimof(fill_image_kernel_params) && NULL != metas)
    {
        vx_parameter param1 = vxGetParameterByIndex(node, 0); /* input scalar */
        vx_parameter param2 = vxGetParameterByIndex(node, 1);

        if (VX_SUCCESS == vxGetStatus((vx_reference)param1) &&
            VX_SUCCESS == vxGetStatus((vx_reference)param2))
        {
            vx_scalar scalar = 0;
            vx_image image = 0;

            status = VX_SUCCESS;

            status |= vxQueryParameter(param1, VX_PARAMETER_REF, &scalar, sizeof(scalar));
            status |= vxQueryParameter(param2, VX_PARAMETER_REF, &image, sizeof(image));

            if (VX_SUCCESS == status)
            {
                vx_enum type = 0;

                status |= vxQueryScalar(scalar, VX_SCALAR_TYPE, &type, sizeof(type));
                if (VX_SUCCESS == status && VX_TYPE_UINT32 == type)
                {
                    status = VX_SUCCESS;
                }
                else
                    status = VX_ERROR_INVALID_TYPE;

                if (VX_SUCCESS == status)
                {
                    vx_uint32 width = 0;
                    vx_uint32 height = 0;
                    vx_df_image format = VX_DF_IMAGE_VIRT;

                    status |= vxQueryImage(image, VX_IMAGE_WIDTH,  &width,  sizeof(width));
                    status |= vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(height));
                    status |= vxQueryImage(image, VX_IMAGE_FORMAT, &format, sizeof(format));

                    status |= vxSetMetaFormatAttribute(metas[1], VX_IMAGE_WIDTH,  &width,  sizeof(width));
                    status |= vxSetMetaFormatAttribute(metas[1], VX_IMAGE_HEIGHT, &height, sizeof(height));
                    status |= vxSetMetaFormatAttribute(metas[1], VX_IMAGE_FORMAT, &format, sizeof(format));
                }
            }

            if (NULL != scalar)
                vxReleaseScalar(&scalar);

            if (NULL != image)
                vxReleaseImage(&image);
        }

        if (NULL != param1)
            vxReleaseParameter(&param1);

        if (NULL != param2)
            vxReleaseParameter(&param2);
    } // if ptrs non NULL

    return status;
} /* own_fill_image_validator() */

vx_kernel_description_t fill_image_kernel =
{
    VX_KERNEL_FILL_IMAGE,
    "org.khronos.debug.fill_image",
    ownFillImageKernel,
    fill_image_kernel_params, dimof(fill_image_kernel_params),
    own_fill_image_validator,
    NULL,
    NULL,
    NULL,
    NULL,
};
