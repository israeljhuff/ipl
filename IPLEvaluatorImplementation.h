#ifndef IPLEVALUATORIMPLEMENTATION_H
#define IPLEVALUATORIMPLEMENTATION_H

#include <string>
#include <vector>

#include "IPLParser.h"

using namespace IPG;

// ----------------------------------------------------------------------------
class IPLEvaluatorImplementation : public Evaluator
{
public:
	bool eval(ASTNode &root_node)
	{
		eprintln("IPLEvaluatorImplementation.eval() called");
		bool retval = eval_program(root_node);
		return retval;
	}

	bool eval_program(ASTNode &node)
	{
		eprintln("IPLEvaluatorImplementation.eval_program() called");
		return Evaluator::eval_program(node);
	}
};

#endif
