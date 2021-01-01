
//----------------------------------------------------------------

#include "ConFreeGr.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// Read a file and construct the Grammar corresponding the that file
	//
	// Inputs:
	//		std::string infile: the name of the file to read
	//
	// Outputs:
	//
	ContextFreeGrammar::ContextFreeGrammar(std::string infile) {

		// Open file input stream
		std::fstream fin{ infile };
		if (!fin) throw Errors(0, Errors::ErrorType::fileNotFound);

		filename = infile;

		// Read number of terminal symbols
		int nTermSymbols;
		fin >> nTermSymbols;
		if (fin.bad() || nTermSymbols < 1)
			throw Errors(1, Errors::ErrorType::nTermSymbolsError);

		// Create a vector for terminal symbols
		termSymbols = std::vector<char>(nTermSymbols);

		// Read the terminal symbols and check for duplicates
		for (int i = 0; i < nTermSymbols; ++i)
			if (std::find(termSymbols.begin(), termSymbols.end(), fin.peek()) == termSymbols.end())
				fin >> termSymbols[i];
			else
				throw Errors(2, Errors::ErrorType::duplicateTermSymbol);


		// Read number of non-terminal symbols
		int nNonTermSymbols;
		fin >> nNonTermSymbols;
		if (fin.bad() || nNonTermSymbols < 1)
			throw Errors(3, Errors::ErrorType::nNonTermSymbolsError);

		// Create a vector for non-terminal symbols
		nonTermSymbols = std::vector<char>(nNonTermSymbols);

		// Read the non-terminal symbols and check for duplicates
		// both in nonTermSymbols vector and in termSymbols vector
		for (int i = 0; i < nNonTermSymbols; ++i)
			if (std::find(nonTermSymbols.begin(), nonTermSymbols.end(), fin.peek()) == nonTermSymbols.end() ||
				std::find(termSymbols.begin(), termSymbols.end(), fin.peek()) != termSymbols.end())
				fin >> nonTermSymbols[i];
			else
				throw Errors(4, Errors::ErrorType::duplicateNonTermSymbol);


		// Read the initial symbol and check if it is defined in the non-terminal symbols
		fin >> initialSymbol;
		if(fin.bad() || 
			std::find(nonTermSymbols.begin(), nonTermSymbols.end(), initialSymbol) == nonTermSymbols.end())
			throw Errors(5, Errors::ErrorType::initialSymbolError);


		// Read the number of rules
		int nRules;
		fin >> nRules;
		if (fin.bad() || nRules < 1)
			throw Errors(6, Errors::ErrorType::nRulesError);

		// Create a vector for rules
		rules = std::vector<Rule>(nRules);

		// Read rules and check for duplicates
		for (int i = 0; i < nRules; ++i) {
			Rule rule;
			fin >> rule;
			if (fin.bad() || std::find(rules.begin(), rules.end(), rule) != rules.end())
				throw Errors(7 + i, Errors::ErrorType::rulesError);
			rules[i] = rule;
		}

		// Create a map with all the rules for faster search
		for (Rule& rule : rules) ruleMap[rule.input].push_back(rule.output);

	} // of constructor ContextFreeGrammar

//----------------------------------------------------------------

	// Check if a word can be generated from 'this' grammar
	//
	// Inputs:
	//		std::string word: the given word
	//
	// Outputs:
	//		bool true: 'word' was accepted
	//		bool false: 'word' was NOT accepted
	//
	bool ContextFreeGrammar::check_word(std::string word) const {

		// Check if any symbol from 'word' is not part of the terminal symbols
		for (char ch : word)
			if (std::find(termSymbols.begin(), termSymbols.end(), ch) == termSymbols.end())
				return false;

		// Creating the root node for the tree
		TreeNode* root = new TreeNode{ nullptr, std::string{initialSymbol} };
		
		// Adding the node to the frontier
		FrontierNode* frontierHead = new FrontierNode{ root };
		FrontierNode* frontierTail = frontierHead;

		// Creating a set for the words
		std::unordered_set<std::string> wordSet{ root->word };

		// A TreeNode-pointer vector to store the children generated in every loop
		std::vector<TreeNode*> children;

		// A variable to store the TreeNode that the solution will be found
		TreeNode* solutionNode = nullptr;

		// Loop until frontierHead variable "is empty" or if a solution was found
		while (true) {

			// Get the next to be expanded leef node
			TreeNode* currNode = get_front(&frontierHead, &frontierTail);
			if (!currNode) break;

			// Check if it holds the solution
			if (currNode->word == word) {
				solutionNode = currNode;
				break;
			}

			// Generate children nodes and add them to frontier
			generate_children(currNode, ruleMap, children);

			// Prune the node if it is already in the tree
			// or if there is no possible way to find a solution throught it
			for (int i = 0; i < children.size(); ++i)
				if (prune(word, children[i], wordSet)) {
					delete children[i];
					children[i] = nullptr;
				}
				else wordSet.insert(children[i]->word);

			// Add children to the back of the frontier
			for (int i = 0; i < children.size(); ++i)
				if (children[i])
					add_to_back(&frontierHead, &frontierTail, children[i]);
			children.clear();
		}

		// If a solution was found print it
		if (solutionNode) {
			show_solution(solutionNode);
			return true;
		}
		else return false;

	} // of function check_word

//----------------------------------------------------------------

	// Check if 'filename' is the same as 'this->filename'
	//
	// Inputs:
	//		std::string grammarName: the filename of the grammar
	//		std::string filename: the input file to check
	//
	// Outputs:
	//		bool true: The grammar in 'filename' is already defined
	//		bool false: The grammar in 'filename' is NOT already defined
	//
	bool operator==(ContextFreeGrammar& grammar, std::string filename) {

		std::string grammarName = grammar;

		for (char& ch : grammarName)
			if (ch == '\\' || ch == '/')
				ch = ' ';

		for (char& ch : filename)
			if (ch == '\\' || ch == '/')
				ch = ' ';

		return grammarName == filename;

	} // of operator==

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
