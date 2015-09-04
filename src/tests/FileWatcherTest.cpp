#include <chrono>
#include <thread>
#include <fstream>
#include <cstdio>

#include <boost/test/unit_test.hpp>

#include "Core/Util/FileWatcher.h"
using namespace dd;

const std::string testFileName = "FileWatcherTestFile";

BOOST_AUTO_TEST_CASE(file_watcher)
{
	auto fw = new FileWatcher("");

	bool f_created = false;
	bool f_size_changed = false;
	bool f_timestamp_changed = false;
	bool f_deleted = false;

	// Make sure the test file doesn't exist
	remove(testFileName.c_str());

	// Set up the file watcher to watch for changes
	fw->AddWatch(testFileName,
		[&f_created, &f_size_changed, &f_timestamp_changed, &f_deleted]
	(std::string path, FileWatcher::FileEventFlags flags)
	{
		if (flags & FileWatcher::FileEventFlags::Created)
			f_created = true;

		if (flags & FileWatcher::FileEventFlags::SizeChanged)
			f_size_changed = true;

		if (flags & FileWatcher::FileEventFlags::TimestampChanged)
			f_timestamp_changed = true;

		if (flags & FileWatcher::FileEventFlags::Deleted)
			f_deleted = true;
	});

	// Create the file
	FILE* testFile = fopen(testFileName.c_str(), "w");
	fw->Check();

	// Write to the file
	fputs("test", testFile);
	fclose(testFile);
	fw->Check();

	// Change file timestamp
	std::time_t time;
	std::localtime(&time);
	time -= 1;
	boost::filesystem::last_write_time(testFileName, time);
	fw->Check();

	// Delete the file;
	remove(testFileName.c_str());
	fw->Check();

	BOOST_CHECK(f_created);
	BOOST_CHECK(f_size_changed);
	BOOST_CHECK(f_timestamp_changed);
	BOOST_CHECK(f_deleted);

	delete fw;
}