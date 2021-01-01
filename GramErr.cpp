
//----------------------------------------------------------------

#include "GramErr.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// The constructor of an error
	//
	// Inputs:
	//		int line: the line that the error occured
	//		ErrorType error: the type of the error (check Errors::ErrorType)
	//
	// Outputs:
	//
	Errors::Errors(int line, ErrorType error) {
		eLine = line;
		eType = error;
	}

//----------------------------------------------------------------

	// Construct the error message and return it
	//
	// Inputs:
	//
	// Outputs:
	//		std::string msg: the error message
	std::string Errors::what() const {

		if (eType == ErrorType::fileNotFound)
			return "File not found\n";

		std::string msg{ "Error in line:" + std::to_string(eLine) + "\n" };

		if (eType == ErrorType::nTermSymbolsError)
			msg += "Please use a positive integer";

		if (eType == ErrorType::duplicateTermSymbol)
			msg += "Terminal symbols should be unique";

		if (eType == ErrorType::nNonTermSymbolsError)
			msg += "Please use a positive integer";

		if (eType == ErrorType::duplicateNonTermSymbol)
			msg += "Non-terminal symbols should be unique and different from the terminal ones";

		if (eType == ErrorType::initialSymbolError)
			msg += "The initial symbol has to be one of the non-terminal ones";

		if (eType == ErrorType::nRulesError)
			msg += "Please use a positive integer";

		if (eType == ErrorType::rulesError)
			msg += "Rules should be unique and defined like this: S aSb (the space inbetween is necerrary)";

		return msg;

	}

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
