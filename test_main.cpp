// ----------------------------------------------------------------------------
// parses all test cases and returns 0 if all pass or 1 if any fail
//
// to build and run on Linux or Windows (Cygwin):
//  g++ --std=c++17 test_main.cpp -o test_parser.exe
//  ./test_parser.exe testdata
//
//  NOTE: assumes parser saved to "example_parser.h"

#include "example_parser.h"

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

using namespace IPG;

// ----------------------------------------------------------------------------
// recursively searches directory tree under 'dirname' for files and appends to
// 'filenames' vector; if 'fileext' not empty string, only includes files with
// given extension
void collect_files(std::vector<std::string> &filenames, std::string dirname,
	std::string fileext = "")
{
	for (const auto &entry : fs::directory_iterator(dirname))
	{
		std::string fn = entry.path().string();
		if (entry.is_regular_file())
		{
			if (fileext != "")
			{
				std::string fe = "." + fileext;
				if (fn.size() >= fe.size())
				{
					std::string tmp = fn.substr(fn.size() - fe.size());
					if (tmp == fe) filenames.push_back(fs::canonical(fn));
				}
			}
			else
			{
				filenames.push_back(fn);
			}
		}
		else if (entry.is_directory())
		{
			collect_files(filenames, fn, fileext);
		}
	}
}

// ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		eprintln("Usage: ", argv[0], " <parent_dir_name>");
		return 1;
	}

	int status = 0;

	// *.good.ipl
	eprintln("Testing expected good files");
	eprintln("---------------------------");
	std::vector<std::string> filenames;
	collect_files(filenames, std::string(argv[1]), "good.ipl");
	for (std::string fn : filenames)
	{
		eprintln("\t", fn);
		FILE *fp;
		fp = fopen(fn.c_str(), "rb");
		if (nullptr == fp)
		{
			eprintln("ERROR opening file: ", fn);
			return 1;
		}
		fseek(fp, 0, SEEK_END);
		size_t file_len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char *buf = new char[file_len + 1];
		buf[file_len] = '\0';
		size_t bytes_read = fread(buf, 1, file_len, fp);
		fclose(fp);
		ASTNode astn(0, "ROOT");
		Parser p(buf);
		if (RET_OK != p.parse(astn))
		{
			status = 1;
			eprintln("ERROR: good file parse failed --- ", fn);
			eprintln("last fully-parsed element is before line ", p.line(),
				", col ", p.col(), ", file position ", p.pos(), " of ", p.len());
			eprintln("last partially-parsed element is before line ",
				p.line_ok(), ", col ", p.col_ok());
		}
		delete[] buf;
	}

	eprintln("");

	// *.bad.ipl
	eprintln("Testing expected bad files");
	eprintln("--------------------------");
	filenames.clear();
	collect_files(filenames, std::string(argv[1]), "bad.ipl");
	for (std::string fn : filenames)
	{
		eprintln("\t", fn);
		FILE *fp;
		fp = fopen(fn.c_str(), "rb");
		if (nullptr == fp)
		{
			eprintln("ERROR opening file: ", fn);
			return 1;
		}
		fseek(fp, 0, SEEK_END);
		size_t file_len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char *buf = new char[file_len + 1];
		buf[file_len] = '\0';
		size_t bytes_read = fread(buf, 1, file_len, fp);
		fclose(fp);
		ASTNode astn(0, "ROOT");
		Parser p(buf);
		if (RET_FAIL != p.parse(astn))
		{
			status = 1;
			eprintln("ERROR: bad file parsed successfully --- ", fn);
		}
		delete[] buf;
	}

	if (0 == status) eprintln("PASSED");
	else eprintln("FAILED");

	return status;
}
