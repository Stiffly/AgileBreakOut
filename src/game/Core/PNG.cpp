/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PrecompiledHeader.h"
#include "Core/PNG.h"

dd::PNG::PNG(std::string path)
{
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) {
		LOG_ERROR("Failed to open texture file \"%s\": %s", path.c_str(), const_cast<const char*>(strerror(errno)));
		return;
	}

	png_byte header[8];
	fread(header, 1, 8, file);
	bool isPNG = !png_sig_cmp(header, 0, 8);
	if (!isPNG) {
		LOG_ERROR("Failed to load texture file \"%s\": File isn't PNG", path.c_str());
		fclose(file);
		return;
	}

	// Initialize libpng
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, (png_error_ptr)&PNG::pngErrorFunction, (png_error_ptr)&PNG::pngErrorFunction);
	if (!png_ptr) {
		LOG_ERROR("libpng: Failed to initialze png_struct");
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		fclose(file);
		return;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		LOG_ERROR("libpng: Failed to initialze png_info");
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		fclose(file);
		return;
	}
	png_infop info_end_ptr = png_create_info_struct(png_ptr);
	if (!info_end_ptr) {
		LOG_ERROR("libpng: Failed to initialze second png_info");
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(file);
		return;
	}
	png_init_io(png_ptr, file);

	// We already read the first 8 bytes of the header
	png_set_sig_bytes(png_ptr, 8);
	// Read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// Get info
	int bit_depth, color_type;
	unsigned int width, height;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	if (bit_depth != 8) {
		LOG_ERROR("libpng: Unsupported bit depth \"%i\" of image \"%s\", must be 8", bit_depth, path.c_str());
		return;
	}
	switch (color_type) {
		case PNG_COLOR_TYPE_RGB:
			Format = Image::ImageFormat::RGB;
			break;
		case PNG_COLOR_TYPE_RGBA:
			Format = Image::ImageFormat::RGBA;
			break;
		default:
			LOG_ERROR("libpng: Unsupported color format \"%i\" of image \"%s\"", color_type, path.c_str());
			return;
	}
	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);

	this->Data = new unsigned char[height * row_bytes];
	png_bytep* row_pointers = new png_bytep[height];

	// Point each row to the continuous data array
	for (int i = 0; i < height; ++i) {
		// Invert Y for OpenGL
		row_pointers[height - 1 - i] = this->Data + i * row_bytes;
	}

	// Read in the data
	png_read_image(png_ptr, row_pointers);
	delete[] row_pointers;

	this->Width = width;
	this->Height = height;

	png_destroy_read_struct(&png_ptr, &info_ptr, &info_end_ptr);
	fclose(file);
}

dd::PNG::~PNG()
{
	if (this->Data != nullptr) {
		delete[] this->Data;
		this->Data = nullptr;
	}
}

void dd::PNG::pngErrorFunction(png_structp png_ptr, png_const_charp error_msg)
{
	LOG_WARNING("%s", error_msg);
}

void dd::PNG::pngWarningFunction(png_structp png_ptr, png_const_charp warning_msg)
{
	LOG_WARNING("%s", warning_msg);
}
