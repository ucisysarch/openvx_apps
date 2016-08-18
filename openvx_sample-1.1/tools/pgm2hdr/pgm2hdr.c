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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <VX/vx.h>
#include <VX/vx_helper.h>


void raw2header(char *basename, FILE *fin, uint32_t bpc, uint32_t channels, uint32_t width, uint32_t height, uint32_t planes)
{
    char filename[256];
    FILE *fout = NULL;
    uint32_t p,y,x,c;
    uint32_t b = bpc*8, wbpc;
    //uint32_t stride_x = channels;
    //uint32_t stride_y = width * stride_x;
    //uint32_t stride_p = height * stride_y;
    switch (b)
    {
        case 24:
            b = 32;
            break;
        case 40:
        case 48:
        case 56:
            b = 64;
            break;
    }
    wbpc = b / 8;
    sprintf(filename, "%s.h", basename);
    fout = fopen(filename, "wb");
    if (fout)
    {
        fprintf(fout, "/* AUTOMATICALLY GENERATED */\n");
        fprintf(fout, "#include <stdint.h>\n");
        fprintf(fout, "/* indexed as: [PLANES][HEIGHT][WIDTH][CHANNELS] */\n");
        fprintf(fout, "uint%u_t %s[%u][%u][%u][%u] = {\n", b, basename, planes, height, width, channels);
        for (p = 0; p < planes; p++)
        {
            fprintf(fout, "    {");
            for (y = 0; y < height; y++)
            {
                fprintf(fout, "{");
                for (x = 0; x < width; x++)
                {
                    fprintf(fout, "{");
                    for (c = 0; c < channels; c++)
                    {
                        //uint32_t offset = (stride_p * p) + (stride_y * y) + (stride_x * x) + (c);
                        uint64_t value = 0;
                        size_t elems;
                        elems = fread(&value, bpc, 1, fin);
                        if (elems)
                        {
                            if (wbpc == sizeof(uint8_t))
                                fprintf(fout, "0x%02x", (uint8_t)value);
                            else if (wbpc == sizeof(uint16_t))
                                fprintf(fout, "0x%04x", (uint16_t)value);
                            else if (wbpc == sizeof(uint32_t))
                                fprintf(fout, "0x%08x", (uint32_t)value);
                            else if (wbpc == sizeof(uint64_t))
                                fprintf(fout, "0x%016lx", value);
                        }
                        else
                        {
                            fprintf(fout, "fread failed");
                        }

                        if (c != (channels - 1))
                            fprintf(fout,", ");
                    }
                    if (x < (width - 1))
                        fprintf(fout, "},");
                    else
                        fprintf(fout, "}");
                }
                if (y < (height - 1))
                    fprintf(fout, "},\n     ");
                else
                    fprintf(fout, "}");
            }
            if (p < (planes - 1))
                fprintf(fout, "},\n\n");
            else
                fprintf(fout, "}\n");
        }
        fprintf(fout, "};\n\n");
        fclose(fout);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        char basename[256];
        char extension[5];
        uint32_t width = 0, height = 0, channels = 1, planes = 1;
        FILE *fin = fopen(argv[1], "rb");

        sscanf(argv[1], "%[a-zA-Z0-9_-].%s", basename, extension);
        printf("basename=%s\nextension=%s\n", basename, extension);
        if (strncmp("pgm", extension, 3) == 0)
        {
            char tmp[256];
            char shortname[256];
            int  elems;
            uint32_t max, bpc = 0;
            sscanf(basename, "%[a-zA-Z0-9]_", shortname);
            FGETS(tmp, fin); // Px
            FGETS(tmp, fin); // comment
            elems  = fscanf(fin, "%u %u\n", &width, &height);
            elems += fscanf(fin, "%u\n", &max);
            if (elems < 3)
            {
                printf("fscanf failed\n");
            }
            if (max == UINT8_MAX)
                bpc = 1;
            if (max == UINT16_MAX)
                bpc = 2;
            if (max == UINT32_MAX)
                bpc = 4;
            printf("shortname=%s\n", shortname);
            raw2header(shortname, fin, bpc, channels, width, height, planes);
        }
        else if (strncmp("bw", extension, 2) == 0 ||
                  strncmp("rgb", extension, 3) == 0)
        {
            char shortname[256];
            char code[5];
            char bits[4] = "1";
            uint32_t bpc = 1;
            sscanf(basename, "%[a-zA-Z0-9]_%ux%u_%[A-Z0-9]_%[0-9-]b", shortname, &width, &height, code, bits);
            bpc = abs(atoi(bits));
            if (bpc != 1)
                bpc /= 8;
            printf("width=%u\n", width);
            printf("height=%u\n", height);
            printf("code=%s\n",code);
            printf("bpc=%u\n",bpc);
            if (strncmp("UYVY", code, 4) == 0)
            {
                channels = 2; /* really a macro-pixel */
            }
            else if (strncmp("I444", code, 4) == 0)
            {
                channels = 3;
            }
            else if (strncmp("P444", code, 4) == 0)
            {
                channels = 1;
                planes = 3;
            }
            else if (strncmp("P420", code, 4) == 0)
            {
                /* this is not really represented well in this format */
                exit(-1);
            }
            else if (strncmp("P400", code, 4) == 0)
            {
                // do nothing
            }
            raw2header(shortname, fin, bpc, channels, width, height, planes);
        }
        fclose(fin);
    }
}
