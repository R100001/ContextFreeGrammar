
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

		// Default constructorS
		Rule() : input{ ' ' }, output{ std::string() } {}

		//Rule(Rule& r) : input{ r.input }, output{ r.output } {}

		char input;
		std::string output;

		friend std::fstream& operator>>(std::fstream& fs, Rule& r) {
			fs >> r.input;
			if (!isspace(fs.peek())) fs.setstate(fs.badbit);
			fs >> r.output;
			if (r.output == "@") r.output = "";

			return fs;
		}

		friend bool operator==(const Rule& r1, const Rule& r2) {
			if (r1.input != r2.input) return false;
			if (r1.output != r2.output) return false;
			return true;	
		 }

	}; // of struct Rule

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
