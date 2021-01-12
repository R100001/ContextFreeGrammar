
//----------------------------------------------------------------

#pragma once

//----------------------------------------------------------------

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <chrono>

//----------------------------------------------------------------

#include "GramErr.h"
#include "Tree.h"

//----------------------------------------------------------------

#define STATICCASTGRAMMAR static_cast<std::vector<Grammars::ContextFreeGrammar, std::allocator<Grammars::ContextFreeGrammar>>::size_type>

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// An implementation of context free grammars
	//
	class ContextFreeGrammar {
	public:

		// Define a grammar by reading its terminal,
		// non-Terminal symbols and rules
		ContextFreeGrammar(std::string infile);

		// Check if a word can be generated with 'this' grammar
		bool check_word(std::string word) const;

		// Get the name of the input file for 'this' grammar
		operator std::string() const { return filename; }

	private:

		std::string filename;

		char initialSymbol;

		std::unordered_set<char> termSymbols;
		std::unordered_set<char> nonTermSymbols;

		std::unordered_map<char, std::vector<std::string>> ruleMap;
		std::unordered_map<std::string, std::vector<char>> reversedRules;

	}; // of class ConFreeGrammar

	// Check if an automaton is already defined
	// by checking the name of the input file
	bool operator==(ContextFreeGrammar& grammar, std::string filename);

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
