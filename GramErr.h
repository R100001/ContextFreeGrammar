
//----------------------------------------------------------------

#pragma once

//----------------------------------------------------------------

#include <iostream>
#include <string>

//----------------------------------------------------------------

#include "Macros.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	class Errors {
	public:

		// All the possible errors that can be caused
		// using namespace Grammars
		enum class ErrorType {
			fileNotFound, nTermSymbolsError, duplicateTermSymbol,
			nNonTermSymbolsError, duplicateNonTermSymbol,
			initialSymbolError, nRulesError, rulesError
		};

		// Construct the error by providing the line and the type
		// Use Errors::ErrorType to get the possible errors
		Errors(std::string infile, int line, ErrorType error);

		// What happened
		std::string what() const;

	private:

		std::string filename;
		int eLine;
		ErrorType eType;

	}; // of class Errors

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
