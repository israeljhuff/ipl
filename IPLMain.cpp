// ----------------------------------------------------------------------------
// main function for IPL
//
// to build and run on Linux or Windows (Cygwin):
//  ../ipg/ipg.exe ipl.grammar > IPLParser.h
//  g++ --std=c++11 -I../ipg IPLMain.cpp -o ipl.exe
//  ./ipl.exe SOMEFILENAME

#include "IPLParser.h"
#include "IPLEvaluatorImplementation.h"

using namespace IPG;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		eprintln("Usage: ", argv[0], " <filename>");
		return 1;
	}
	FILE *fp;
	fp = fopen(argv[1], "rb");
	if (nullptr == fp)
	{
		eprintln("ERROR opening file: ", argv[1]);
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
	if (RET_OK != p.parse(astn))
	{
		eprintln("ERROR parsing");
		eprintln("last fully-parsed element is before line ", p.line(),
			", col ", p.col(), ", file position ", p.pos(), " of ", p.len());
		eprintln("last partially-parsed element is before line ",
			p.line_ok(), ", col ", p.col_ok());
	}
	else
	{
		astn.print();
		eprintln("parsed successfully");

		IPLEvaluatorImplementation iplei;
		EvaluationStateImplementation esi;
		// skip "ROOT" node and assume 1 child node
		if (iplei.eval(astn.child(0), esi))
		{
			eprintln("evaluated successfully");
		}
		else
		{
			eprintln("ERROR evaluating");
		}
	}
	delete[] buf;
	return 0;
}
