
//----------------------------------------------------------------

#include "Rule.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	std::fstream& operator>>(std::fstream& fs, Rule& r) {
		fs >> r.input;
		if (!isspace(fs.peek())) fs.setstate(std::fstream::badbit);
		fs >> r.output;
		if (r.output == "@") r.output = "";

		return fs;
	}

//----------------------------------------------------------------

	bool operator==(const Rule& r1, const Rule& r2) {
		if (r1.input != r2.input) return false;
		if (r1.output != r2.output) return false;
		return true;
	}

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
