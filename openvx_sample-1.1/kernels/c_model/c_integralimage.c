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

#include <c_model.h>

// nodeless version of the XXXX kernel
vx_status vxIntegralImage(vx_image src, vx_image dst)
{
    vx_uint32 y, x;
    void *src_base = NULL;
    void *dst_base = NULL;
    vx_imagepatch_addressing_t src_addr = VX_IMAGEPATCH_ADDR_INIT;
    vx_imagepatch_addressing_t dst_addr = VX_IMAGEPATCH_ADDR_INIT;
    vx_rectangle_t rect;
    vx_map_id src_map_id = 0;
    vx_map_id dst_map_id = 0;

    vx_status status = VX_SUCCESS;

    status = vxGetValidRegionImage(src, &rect);

    status |= vxMapImagePatch(src, &rect, 0, &src_map_id, &src_addr, &src_base, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
    status |= vxMapImagePatch(dst, &rect, 0, &dst_map_id, &dst_addr, &dst_base, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

    for (y = 0; (y < src_addr.dim_y) && (status == VX_SUCCESS); y++)
    {
        vx_uint8 *pixels = vxFormatImagePatchAddress2d(src_base, 0, y, &src_addr);
        vx_uint32 *sums = vxFormatImagePatchAddress2d(dst_base, 0, y, &dst_addr);

        if (y == 0)
        {
            sums[0] = pixels[0];
            for (x = 1; x < src_addr.dim_x; x++)
            {
                sums[x] = sums[x-1] + pixels[x];
            }
        }
        else
        {
            vx_uint32 *prev_sums = vxFormatImagePatchAddress2d(dst_base, 0, y-1, &dst_addr);
            sums[0] = prev_sums[0] + pixels[0];
            for (x = 1; x < src_addr.dim_x; x++)
            {
                sums[x] = pixels[x] + sums[x-1] + prev_sums[x] - prev_sums[x-1];
            }
        }
    }

    status |= vxUnmapImagePatch(src, src_map_id);
    status |= vxUnmapImagePatch(dst, dst_map_id);

    return status;
}

