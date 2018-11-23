/*
 * JPEGReader.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: linh
 */

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "LibJpeg/jpeglib.h"
#include <setjmp.h>
#include <stdint.h>

using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Matrix.h"

#include "JPEGReader.h"

Matrix<RGB> decompressJPEG(const char* filename, int &rows, int &cols) {
	//std::cout << "\nBegin decompress JPEG image.\n";

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	unsigned char* buffer;
	int row_stride;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// step 2
	FILE * infile;
	if ((infile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	Matrix<RGB> rgbMatrix((int)cinfo.output_height, (int)cinfo.output_width);
	rows = rgbMatrix.getRows();
	cols = rgbMatrix.getCols();

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = new unsigned char[cinfo.output_height * row_stride];
	//int i = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		int row = cinfo.output_scanline;
		unsigned char* p = buffer
				+ cinfo.output_scanline * cinfo.image_width
						* cinfo.num_components;
		jpeg_read_scanlines(&cinfo, &p, 1);
		for (int i = 0; i < cinfo.output_width; i++) {
			unsigned int k = (i + cinfo.output_width * row) * 3;
			RGB rgb;
			rgb.R = (unsigned short int) buffer[k];
			rgb.G = (unsigned short int) buffer[k + 1];
			rgb.B = (unsigned short int) buffer[k + 2];
			rgbMatrix.setAtPosition(row, i, rgb);
		}
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	//std::cout << "\nFinished de-compress JPEG.\n";
	return rgbMatrix;
}
void saveJPEG(const char* filename, unsigned char *buffer, int width, int height,int components, J_COLOR_SPACE colorspace) {
	//std::cout << "\nBegin compress JPEG image.\n";

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	// step 2
	FILE * outfile;
	if ((outfile = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}
	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = components;
	cinfo.in_color_space = colorspace;

	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);

	JSAMPROW row_pointer[1];
	int row_stride = width * 3;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = &buffer[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);

	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);
	//std::cout << "\nFinished compress JPEG.\n";
}
void RGB2JPEG(const char* filename, ptr_RGBMatrix rgbMatrix) {

	unsigned char *buffer = new unsigned char[rgbMatrix->getRows()
			* rgbMatrix->getCols() * 3];

	for (int r = 0; r < rgbMatrix->getRows(); r++) {
		for (int c = 0; c < rgbMatrix->getCols(); c++) {
			RGB value = rgbMatrix->getAtPosition(r, c);
			int i = (c + rgbMatrix->getCols() * r) * 3;
			buffer[i] = (unsigned char) value.R;
			buffer[i + 1] = (unsigned char) value.G;
			buffer[i + 2] = (unsigned char) value.B;
		}
	}

	saveJPEG(filename,buffer,rgbMatrix->getCols(),rgbMatrix->getRows(),3,JCS_RGB);
}
void Gray2JPEG(const char* filename, ptr_IntMatrix grayMatrix) {

	unsigned char *buffer = new unsigned char[grayMatrix->getRows()
			* grayMatrix->getCols() * 3];

	for (int r = 0; r < grayMatrix->getRows(); r++) {
		for (int c = 0; c < grayMatrix->getCols(); c++) {
			int value = grayMatrix->getAtPosition(r, c);
			int i = (c + grayMatrix->getCols() * r) * 3;
			buffer[i] = value;
			buffer[i + 1] = value;
			buffer[i + 2] = value;
		}
	}

	saveJPEG(filename,buffer,grayMatrix->getCols(),grayMatrix->getRows(),3,JCS_RGB);
}
