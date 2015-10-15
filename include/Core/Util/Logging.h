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

#ifndef Logging_h__
#define Logging_h__

#ifdef _WIN32
// http://stackoverflow.com/a/2282433
#define __func__ __FUNCTION__
// http://stackoverflow.com/a/8488201
#define __BASE_FILE__ \
	(strrchr(__FILE__, '/') \
	? strrchr(__FILE__, '/') + 1 \
	: (strrchr(__FILE__, '\\') \
	? strrchr(__FILE__, '\\') + 1 \
	: __FILE__))
#endif

#include <iostream>
#include <limits>
#include <string>
#include <cstdio>
#include <stdarg.h>

enum _LOG_LEVEL
{
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG
};

extern _LOG_LEVEL LOG_LEVEL;

const static char* _LOG_LEVEL_PREFIX[] =
{
	"E: ",
	"W: ",
	"",
	"D: "
};

static void _LOG(_LOG_LEVEL logLevel, const char* file, const char* func, unsigned int line, const char* format, ...)
{
	if (logLevel > LOG_LEVEL)
		return;

	char* message = nullptr;
	va_list args;

	va_start(args, format);
	size_t size = vsnprintf(message, 0, format, args) + 1;
	va_end(args);

	va_start(args, format);
	message = new char[size];
	vsnprintf(message, size, format, args);
	va_end(args);

	if (logLevel == LOG_LEVEL_ERROR)
	{
		std::cerr << file << ":" << line << " " << func << std::endl;
		std::cerr << _LOG_LEVEL_PREFIX[logLevel] << message << std::endl;
	}
	else
	{
		std::cout << _LOG_LEVEL_PREFIX[logLevel] << message << std::endl;
	}

	delete[] message;
}

#define LOG(logLevel, format, ...) \
	_LOG(logLevel, __BASE_FILE__, __func__, __LINE__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
	LOG(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#define LOG_WARNING(format, ...) \
	LOG(LOG_LEVEL_WARNING, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
	LOG(LOG_LEVEL_INFO, format, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) \
	LOG(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)

#endif // Logging_h__