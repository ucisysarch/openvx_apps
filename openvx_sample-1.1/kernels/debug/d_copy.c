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

#include "debug_k.h"


// nodeless version of the CopyImage kernel
vx_status ownCopyImage(vx_image input, vx_image output)
{
    vx_uint32 p = 0;
    vx_uint32 y = 0;
    vx_uint32 len = 0;
    vx_size planes = 0;
    void* src;
    void* dst;
    vx_imagepatch_addressing_t src_addr = VX_IMAGEPATCH_ADDR_INIT;
    vx_imagepatch_addressing_t dst_addr = VX_IMAGEPATCH_ADDR_INIT;
    vx_map_id src_map_id = 0;
    vx_map_id dst_map_id = 0;
    vx_rectangle_t rect;
    vx_status status = VX_SUCCESS; // assume success until an error occurs.

    status |= vxGetValidRegionImage(input, &rect);

    status |= vxQueryImage(input, VX_IMAGE_PLANES, &planes, sizeof(planes));

    for (p = 0; p < planes && status == VX_SUCCESS; p++)
    {
        status = VX_SUCCESS;
        src = NULL;
        dst = NULL;

        status |= vxMapImagePatch(input, &rect, p, &src_map_id, &src_addr, &src, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);
        status |= vxMapImagePatch(output, &rect, p, &dst_map_id, &dst_addr, &dst, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);

        for (y = 0; y < src_addr.dim_y && status == VX_SUCCESS; y += src_addr.step_y)
        {
            /*
             * in the case where the secondary planes are subsampled, the
             * scale will skip over the lines that are repeated.
             */
            vx_uint8* srcp = vxFormatImagePatchAddress2d(src, 0, y, &src_addr);
            vx_uint8* dstp = vxFormatImagePatchAddress2d(dst, 0, y, &dst_addr);

            len = (src_addr.stride_x * src_addr.dim_x * src_addr.scale_x) / VX_SCALE_UNITY;

            memcpy(dstp, srcp, len);
        }

        if (status == VX_SUCCESS)
        {
            status |= vxUnmapImagePatch(input, src_map_id);
            status |= vxUnmapImagePatch(output, dst_map_id);
        }
    }

    return status;
} /* ownCopyImage() */


// nodeless version of the CopyArray kernel
vx_status ownCopyArray(vx_array src, vx_array dst)
{
    vx_size src_num_items = 0;
    vx_size dst_capacity = 0;
    vx_size src_stride = 0;
    void* srcp = NULL;
    vx_map_id map_id = 0;
    vx_status status = VX_SUCCESS; // assume success until an error occurs.

    status = VX_SUCCESS;

    status |= vxQueryArray(src, VX_ARRAY_NUMITEMS, &src_num_items, sizeof(src_num_items));
    status |= vxQueryArray(dst, VX_ARRAY_CAPACITY, &dst_capacity, sizeof(dst_capacity));

    if (status == VX_SUCCESS)
    {
        status |= vxMapArrayRange(src, 0, src_num_items, &map_id, &src_stride, &srcp, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);

        if (src_num_items <= dst_capacity && status == VX_SUCCESS)
        {
            status |= vxTruncateArray(dst, 0);
            status |= vxAddArrayItems(dst, src_num_items, srcp, src_stride);
        }
        else
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }

        status |= vxUnmapArrayRange(src, map_id);
    }

    return status;
} /* ownCopyArray() */

