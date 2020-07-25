#ifndef PARSERTESTCASE_H
#define PARSERTESTCASE_H

#include <string>
#include <vector>

// ----------------------------------------------------------------------------
struct ParserTestCase
{
	// name of test case
	std::string m_name;
	// code to parse
	std::string m_code;
	// expected parse result: true if it should parse ok, otherwise false
	bool m_expected;

	// ------------------------------------------------------------------------
	ParserTestCase(std::string tc_name, std::string tc_code, bool tc_expected)
	{
		m_name = tc_name;
		m_code = tc_code;
		m_expected = tc_expected;
	}

	// ------------------------------------------------------------------------
	const char *name() { return m_name.c_str(); }
	const char *code() { return m_code.c_str(); }
	bool expected() { return m_expected; }
};

#endif
