// ----------------------------------------------------------------------------
// parses all test cases and returns 0 if all pass or 1 if any fail
//
// to build and run on Linux or Windows (Cygwin):
//  g++ --std=c++17 test_main.cpp -o test_parser.exe
//  ./test_parser.exe testdata

#include "IPLParser.h"
#include "ParserTestCase.h"

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
void run_tests(int *tests_total, int *tests_failed)
{
	std::vector<ParserTestCase> ptcs;

	ptcs.push_back(ParserTestCase("nothing to parse", "", true));
	ptcs.push_back(ParserTestCase("comment", "# this is a comment", true));

	ptcs.push_back(ParserTestCase("bool #01", "true;", true));
	ptcs.push_back(ParserTestCase("bool #02", "false;", true));

	ptcs.push_back(ParserTestCase("integer #01", "0;", true));
	ptcs.push_back(ParserTestCase("integer #02", "1234;", true));
	ptcs.push_back(ParserTestCase("integer #03", "0x5;", true));
	ptcs.push_back(ParserTestCase("integer #04", "0x1;", true));
	ptcs.push_back(ParserTestCase("integer #05", "0x1a;", true));
	ptcs.push_back(ParserTestCase("integer #06", "0xe8;", true));

	ptcs.push_back(ParserTestCase("float #01", "123.;", true));
	ptcs.push_back(ParserTestCase("float #02", ".234;", true));
	ptcs.push_back(ParserTestCase("float #03", "0.3;", true));
	ptcs.push_back(ParserTestCase("float #04", "4.0;", true));
	ptcs.push_back(ParserTestCase("float #05", "5e6;", true));
	ptcs.push_back(ParserTestCase("float #06", "6.e7;", true));
	ptcs.push_back(ParserTestCase("float #07", ".7e8;", true));
	ptcs.push_back(ParserTestCase("float #08", "8.1e9;", true));
	ptcs.push_back(ParserTestCase("float #09", "8.2e-9;", true));
	ptcs.push_back(ParserTestCase("float #10", "8.3e+9;", true));

	ptcs.push_back(ParserTestCase("string #01", "\"\";", true));
	ptcs.push_back(ParserTestCase("string #02", "\"foo\";", true));
	ptcs.push_back(ParserTestCase("string #03", "\"\\\"foo\\\"\";", true));
	// multiple adjacent strings are allowed
	ptcs.push_back(ParserTestCase("string #04", "\"multiple\" \" adjacent\" \" strings\";", true));

	ptcs.push_back(ParserTestCase("identifer #01", "a;", true));
	ptcs.push_back(ParserTestCase("identifer #02", "Avar;", true));
	ptcs.push_back(ParserTestCase("identifer #03", "_avar;", true));
	ptcs.push_back(ParserTestCase("identifer #04", "a_var;", true));
	ptcs.push_back(ParserTestCase("identifer #05", "avar_;", true));
	ptcs.push_back(ParserTestCase("identifer #06", "a1var_1;", true));
	ptcs.push_back(ParserTestCase("identifer #07", "1var;", false));

	ptcs.push_back(ParserTestCase("arithmetic", "a = (b % c + d / e) - x * -y;", true));
	ptcs.push_back(ParserTestCase("boolean arithmetic", "a = ~x & (y << 1) | (z >> 2) ^ w;", true));
	ptcs.push_back(ParserTestCase("boolean logic", "a = !(x < 1) && (x < 10) || (x == 12);", true));

	ptcs.push_back(ParserTestCase("parentheses", "a = (1);", true));
	ptcs.push_back(ParserTestCase("nested parentheses", "a = ((1));", true));
	// parentheses must contain something
	ptcs.push_back(ParserTestCase("empty parentheses", "a = ();", false));
	// parentheses must match count
	ptcs.push_back(ParserTestCase("too many closing parentheses", "a = ());", false));
	ptcs.push_back(ParserTestCase("too many opening parentheses", "a = (();", false));
	ptcs.push_back(ParserTestCase("group #01", "(1);", true));
	ptcs.push_back(ParserTestCase("group #02", "(\"a literal string\");", true));
	ptcs.push_back(ParserTestCase("group #03", "(someFunctionOrMethod(a, 1));", true));
	ptcs.push_back(ParserTestCase("group #04", "(aVariable);", true));
	// cannot assign in a group
	ptcs.push_back(ParserTestCase("group #05", "(a = b + c);", false));

	ptcs.push_back(ParserTestCase("loop test #01", "loop {}", true));
	ptcs.push_back(ParserTestCase("loop test #02", "loop post {}", true));
	ptcs.push_back(ParserTestCase("loop test #03", "loop { break; }", true));
	ptcs.push_back(ParserTestCase("loop test #04", "loop { continue; }", true));
	ptcs.push_back(ParserTestCase("loop test #05", "loop { return; }", true));
	ptcs.push_back(ParserTestCase("loop test #06", "loop (;;) {}", true));
	ptcs.push_back(ParserTestCase("loop test #07", "loop (a += 1;;) {}", true));
	ptcs.push_back(ParserTestCase("loop test #08", "loop (a += 1, b += 1;;) {}", true));
	ptcs.push_back(ParserTestCase("loop test #09", "loop (a = b - 7 * 5, c = d + 1, x = 5;;) {}", true));
	ptcs.push_back(ParserTestCase("loop test #10", "loop (int32 a = b, c = d + 1, x = 5;;) {}", true));
	ptcs.push_back(ParserTestCase("loop test #11", "loop (SomeClass a = 1, b = 2, c = 3;;) {}", true));
	ptcs.push_back(ParserTestCase("loop test #12", "loop (int32 a = 1, b = 2, c = 3; a < 10; a += 1, b += 1, c += 1) {}", true));
	ptcs.push_back(ParserTestCase("loop test #12", "loop post (int32 a = 1; a < 10; a += 1) {}", true));

	ptcs.push_back(ParserTestCase("vector test #01", "vector<sint32> foo1 = [];", true));
	ptcs.push_back(ParserTestCase("vector test #02", "vector<sint32> foo2 = [ 5 ];", true));
	ptcs.push_back(ParserTestCase("vector test #03", "vector<sint32> foo3 = [5,];", true));
	ptcs.push_back(ParserTestCase("vector test #04", "vector<sint32> foo4 = [5,6];", true));
	ptcs.push_back(ParserTestCase("vector test #05", "vector<sint32> foo2 = [2+7, asdf ];", true));
	ptcs.push_back(ParserTestCase("vector test #06", "vector<vector<sint32>> foo2 = [[1], [2, 3]];", true));

	// parser does no type checking, so it will not fail on type mismatch in assignment
	ptcs.push_back(ParserTestCase("map test #01", "map<uint32, customType > bar = 1;", true));
	ptcs.push_back(ParserTestCase("map test #02", "map<uint32, customType > bar = [ \"a\" : 1];", true));
	ptcs.push_back(ParserTestCase("map test #03", "map< uint32, vector < customType>> bar = [];", true));
	ptcs.push_back(ParserTestCase("map test #04", "map< uint32, vector < customType>> bar = [1:1];", true));
	ptcs.push_back(ParserTestCase("map test #05", "map< uint32, vector < customType>> bar = [ 1 : 1, asdf : 5 + 7, ];", true));

	ptcs.push_back(ParserTestCase("function test #01", "void aVoidFunc() {}", true));
	// parser does no type checking, so it will not fail on missing return statement
	ptcs.push_back(ParserTestCase("function test #02", "uint32 aParameterlessFunc() {}", true));
	ptcs.push_back(ParserTestCase("function test #03", "sint32 myfunc(uint32 foo, uint8 bar) {}", true));
	// curly braces required (i.e. no forward declarations allowed)
	ptcs.push_back(ParserTestCase("function test #04", "void aVoidFunc()", false));
	ptcs.push_back(ParserTestCase("function test #05", "void aVoidFunc();", false));
	// cannot declare access specifier for a function, only for a method
	ptcs.push_back(ParserTestCase("function test #06", "public uint32 myFunc1() {}", false));

	ptcs.push_back(ParserTestCase("method test #01", "myMethod1();", true));
	ptcs.push_back(ParserTestCase("method test #01", "someInstance.myMethod1();", true));
	ptcs.push_back(ParserTestCase("method test #01", "someInstance.someSubInstance.myMethod1();", true));

	// variable access specifier required
	ptcs.push_back(ParserTestCase("class test #01", "class SomeClass { int x = 1; }", false));
	// method access specifier required
	ptcs.push_back(ParserTestCase("class test #02", "class SomeClass { void someMethod() {} }", false));
	// method return type required
	ptcs.push_back(ParserTestCase("class test #03", "class SomeClass { private someMethod() {} }", false));
	ptcs.push_back(ParserTestCase("class test #04", "class SomeClass {}", true));
	ptcs.push_back(ParserTestCase("class test #05", "class SomeChildClass : SomeParentClass {}", true));
	ptcs.push_back(ParserTestCase("class test #06", "class SomeClass { private int x = 1; }", true));
	ptcs.push_back(ParserTestCase("class test #07", "class SomeClass { private int x = 1, y = 2; }", true));
	ptcs.push_back(ParserTestCase("class test #08", "class SomeClass { protected int x = 1; }", true));
	ptcs.push_back(ParserTestCase("class test #09", "class SomeClass { public int x = 1; }", true));
	ptcs.push_back(ParserTestCase("class test #10", "class SomeClass { private void someMethod() {} }", true));
	ptcs.push_back(ParserTestCase("class test #11", "class SomeClass { protected void someMethod() {} }", true));
	ptcs.push_back(ParserTestCase("class test #12", "class SomeClass { public void someMethod() {} }", true));
	ptcs.push_back(ParserTestCase("class test #13", "class SomeClass { public int32 someMethod(int64 a, string b) {} }", true));

	// empty regex not allowed
	ptcs.push_back(ParserTestCase("regex test #01", "//;", false));
	ptcs.push_back(ParserTestCase("regex test #02", "/1/;", true));
	ptcs.push_back(ParserTestCase("regex test #03", "/[ab]/;", true));
	ptcs.push_back(ParserTestCase("regex test #04", "/[a-z]/;", true));
	ptcs.push_back(ParserTestCase("regex test #05", "/[a-z]*/;", true));
	ptcs.push_back(ParserTestCase("regex test #06", "/[a-z]+/;", true));
	ptcs.push_back(ParserTestCase("regex test #07", "/[a-z]?/;", true));
	ptcs.push_back(ParserTestCase("regex test #07", "/[a-z]+|[0-9]+/;", true));
	ptcs.push_back(ParserTestCase("regex test #08", "/[_A-Za-z][0-9_A-Za-z]*/;", true));
	ptcs.push_back(ParserTestCase("regex test #09", "/ab+(c|[de])*/;", true));
	// check for match
	ptcs.push_back(ParserTestCase("regex test #10", "bool found =~ /[_A-Za-z][0-9_A-Za-z]*/;", true));
	// check for match and capture results
	ptcs.push_back(ParserTestCase("regex test #11", "vector<string> matches =~ /x(([_A-Za-z])[0-9_A-Za-z]*)y/;", true));

	for (auto ptc : ptcs)
	{
		ASTNode astn(0, 1, 1, "ROOT");
		Parser p(ptc.code());
		bool result = (RET_OK == p.parse(astn));
		(*tests_total)++;
		eprintln("\t", ptc.name());
		if (ptc.expected() != result)
		{
			(*tests_failed)++;
			eprintln("\t\t*** FAILED ***");
			eprintln("\t\t ", p.line_ok(), " ", p.col_ok());
		}
	}
}

// ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	if (argc < 1)
	{
		eprintln("Usage: ", argv[0], " [parent_dir_name]");
		return 1;
	}

	int tests_total = 0, tests_failed = 0;

	eprintln("");

	// run built-in tests
	eprintln("Running built-in test cases");
	eprintln("---------------------------");
	run_tests(&tests_total, &tests_failed);

	// if parent_dir_name argument provided, run those test cases
	if (argc > 1)
	{
		eprintln("");

		// run tests from *.good.ipl
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
			ASTNode astn(0, 1, 1, "ROOT");
			Parser p(buf);
			tests_total++;
			if (RET_OK != p.parse(astn))
			{
				tests_failed++;
				eprintln("ERROR: good file parse failed --- ", fn);
				eprintln("last fully-parsed element is before line ", p.line(),
					", col ", p.col(), ", file position ", p.pos(), " of ", p.len());
				eprintln("last partially-parsed element is before line ",
					p.line_ok(), ", col ", p.col_ok());
			}
			delete[] buf;
		}

		eprintln("");

		// run tests from *.bad.ipl
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
			ASTNode astn(0, 1, 1, "ROOT");
			Parser p(buf);
			tests_total++;
			if (RET_FAIL != p.parse(astn))
			{
				tests_failed++;
				eprintln("ERROR: bad file parsed successfully --- ", fn);
			}
			delete[] buf;
		}

		eprintln("");
	}

	eprintln("tests    run = ", tests_total);
	eprintln("      passed = ", tests_total - tests_failed);

	eprintln("");

	if (0 == tests_failed) eprintln("PASSED");
	else eprintln("FAILED");

	return (0 == tests_failed) ? 0 : 1;
}
