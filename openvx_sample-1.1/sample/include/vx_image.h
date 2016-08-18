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

#ifndef _OPENVX_INT_IMAGE_H_
#define _OPENVX_INT_IMAGE_H_

/*!
 * \file
 * \brief The internal image implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_image Internal Image API
 * \ingroup group_internal
 * \brief The internal Image API.
 */


#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Used to validate the vx_image types.
 * \param [in] img The vx_image to validate.
 * \ingroup group_int_image
 */
vx_bool ownIsValidImage(vx_image img);

/*! \brief Determines which VX_DF_IMAGE are supported in the sample implementation.
 * \param [in] code The df_image code to test.
 * \retval vx_bool
 * \retval vx_true_e The code is supported.
 * \retval vx_false_e The code is not supported.
 * \ingroup group_int_image
 */
vx_bool ownIsSupportedFourcc(vx_df_image code);

/*! \brief Used to initialize a single plane in an image object.
 * \param [in] image The image object.
 * \param [in] index The index to the plane.
 * \param [in] bpp The unit size (size of pixel).
 * \param [in] width The width in pixels.
 * \param [in] height The height in pixels.
 * \ingroup group_int_image
 */
void ownInitPlane(vx_image image,
                 vx_uint32 index,
                 vx_uint32 soc,
                 vx_uint32 channels,
                 vx_uint32 width,
                 vx_uint32 height);


/*! \brief Used to initialize the image meta-data structure with the correct
 * values per the df_image code.
 * \param [in,out] image The image object.
 * \param [in] width Width in pixels
 * \param [in] height Height in pixels
 * \param [in] color VX_DF_IMAGE color space.
 * \ingroup group_int_image
 */
void ownInitImage(vx_image image, vx_uint32 width, vx_uint32 height, vx_df_image color);

/*! \brief Used to free an image object.
 * \param [in] image The image object to free. Only the data is freed, not the
 * meta-data structure.
 * \ingroup group_int_image
 */
void ownFreeImage(vx_image image);

/*! \brief Used to allocate an image object.
 * \param [in,out] image The image object.
 * \ingroup group_int_image
 */
vx_bool ownAllocateImage(vx_image image);

/*! \brief Prints the values of the images.
 * \ingroup group_int_image
 */
void ownPrintImage(vx_image image);

/*! \brief Destroys an image
 * \param [in] ref The image to destroy.
 * \ingroup group_int_image
 */
void ownDestructImage(vx_reference ref);

#ifdef __cplusplus
}
#endif

#endif
