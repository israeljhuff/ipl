#ifndef IPLEVALUATORIMPLEMENTATION_H
#define IPLEVALUATORIMPLEMENTATION_H

#include <string>
#include <vector>

#include "IPLParser.h"

using namespace IPG;

// ----------------------------------------------------------------------------
// state of execution in a scope including:
//  current position in AST
//  variables defined in this scope
//  classes defined in this scope
class ScopeExecutionState
{
	ScopeExecutionState *parentScope = nullptr;
	std::vector<ScopeExecutionState> childScopes;

// TODO: destructor should recursively delete and clean up all child scopes

// TODO: AST position, vars, classes, etc.
};

// ----------------------------------------------------------------------------
// variables (including primitives like int and float), instances of classes
class Object
{
// TODO
};

// ----------------------------------------------------------------------------
class EvaluationStateImplementation : public EvaluationState
{
public:
	void print()
	{
		eprintln("EvaluationStateImplementation::print() called");
	}
};

// ----------------------------------------------------------------------------
class IPLEvaluatorImplementation : public Evaluator
{
private:
	ScopeExecutionState sesGlobal;

public:
	bool eval(ASTNode &root_node, EvaluationState &eval_state) override
	{
		eprintln("IPLEvaluatorImplementation.eval() called");
		bool retval = eval_program(root_node, eval_state);
		EvaluationStateImplementation &esi
			= static_cast<EvaluationStateImplementation &>(eval_state);
		esi.print();
		return retval;
	}

	bool eval_program(ASTNode &node, EvaluationState &eval_state) override
	{
		eprintln("IPLEvaluatorImplementation.eval_program() called");
		return Evaluator::eval_program(node, eval_state);
	}

	// ***RULE*** else_scope : | "else" wscm* scope
	bool eval_else_scope(ASTNode &node, EvaluationState &eval_state) override
	{
		eprintln("IPLEvaluatorImplementation.eval_else_scope() called");
		return Evaluator::eval_else_scope(node, eval_state);
	}
};

#endif
