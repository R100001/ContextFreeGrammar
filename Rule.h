
//----------------------------------------------------------------

#pragma once

//----------------------------------------------------------------

#include <string>
#include <sstream>
#include <fstream>

//----------------------------------------------------------------

#define emptyString "@"
#define replaceIndicator emptyString

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	struct Rule {

		// Default constructor
		Rule() : input{ ' ' }, output{ std::string() } {}

		//Rule(Rule& r) : input{ r.input }, output{ r.output } {}

		char input;
		std::string output;

	}; // of struct Rule

	std::fstream& operator>>(std::fstream& fs, Rule& r);

	bool operator==(const Rule& r1, const Rule& r2);

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
