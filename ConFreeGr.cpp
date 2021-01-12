
//----------------------------------------------------------------

#include "ConFreeGr.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// Read a file and construct the Grammar corresponding the that file
	//
	// Inputs:
	//		-std::string infile: the name of the file to read
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

		char tempSymbol = ' ';
		// Read the terminal symbols and check for duplicates
		for (int i = 0; i < nTermSymbols; ++i)
			if (!termSymbols.contains(fin.peek())) {
				fin >> tempSymbol;
				termSymbols.insert(tempSymbol);
			}
			else
				throw Errors(2, Errors::ErrorType::duplicateTermSymbol);


		// Read number of non-terminal symbols
		int nNonTermSymbols;
		fin >> nNonTermSymbols;
		if (fin.bad() || nNonTermSymbols < 1)
			throw Errors(3, Errors::ErrorType::nNonTermSymbolsError);

		// Read the non-terminal symbols and check for duplicates
		// both in nonTermSymbols vector and in termSymbols vector
		for (int i = 0; i < nNonTermSymbols; ++i)
			if (!nonTermSymbols.contains(fin.peek()) || !termSymbols.contains(fin.peek())) {
				fin >> tempSymbol;
				nonTermSymbols.insert(tempSymbol);
			}
			else
				throw Errors(4, Errors::ErrorType::duplicateNonTermSymbol);


		// Read the initial symbol and check if it is defined in the non-terminal symbols
		fin >> initialSymbol;
		if(fin.bad() || !nonTermSymbols.contains(initialSymbol))
			throw Errors(5, Errors::ErrorType::initialSymbolError);


		// Read the number of rules
		int nRules;
		fin >> nRules;
		if (fin.bad() || nRules < 1)
			throw Errors(6, Errors::ErrorType::nRulesError);

		// Read rules and check for duplicates
		char ruleInput = ' ';
		std::string ruleOutput;
		for (int i = 0; i < nRules; ++i) {
			
			fin >> ruleInput;
			if (!isspace(fin.peek())) fin.setstate(std::fstream::badbit);
			fin >> ruleOutput;

			if (fin.bad() || std::find(ruleMap[ruleInput].begin(), ruleMap[ruleInput].end(),
								ruleOutput) != ruleMap[ruleInput].end())
				throw Errors(7 + i, Errors::ErrorType::rulesError);

			ruleMap[ruleInput].push_back(ruleOutput);

			// Will be used to check the generated non-terminal symbols
			// between terminal oness
			reversedRules[ruleOutput].push_back(ruleInput);
		}

	} // of constructor ContextFreeGrammar

//----------------------------------------------------------------

	// Check if a word can be generated from 'this' grammar
	//
	// Inputs:
	//		-std::string word: the given word
	//
	// Outputs:
	//		-bool true: 'word' was accepted
	//		-bool false: 'word' was NOT accepted
	//
	bool ContextFreeGrammar::check_word(std::string word) const {

		// Check if any symbol from 'word' is not part of the terminal symbols
		for (char ch : word)
			if (!termSymbols.contains(ch))
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

			{
				//using namespace std::chrono;

				//auto time = system_clock::now();

				// Generate children nodes and add them to frontier
				//std::cout << currNode->word << '\n';
				generate_children(currNode, ruleMap, children); 

				//std::cout << "Generation time: "
				//	<< duration_cast<milliseconds>(system_clock::now() - time).count()
				//	<< " ms\n\n";

				//time = system_clock::now();

				// Prune the node if it is already in the tree
				// or if there is no possible way to find a solution throught it
				for (int i = 0; i < children.size(); ++i)
					if (prune(word, children[i], wordSet, termSymbols, nonTermSymbols, reversedRules)) {
						delete children[i];
						children[i] = nullptr;
					}
					else {
						//std::cout << children[i]->word << '\n';
						wordSet.insert(children[i]->word);
					}

				//std::cout << "Prune time: "
				//	<< duration_cast<milliseconds>(system_clock::now() - time).count()
				//	<< " ms\n\n";
			}

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
	//		-std::string grammarName: the filename of the grammar
	//		-std::string filename: the input file to check
	//
	// Outputs:
	//		-bool true: The grammar in 'filename' is already defined
	//		-bool false: The grammar in 'filename' is NOT already defined
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
