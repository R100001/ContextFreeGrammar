
//----------------------------------------------------------------

#include "ConFreeGr.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// Read a file and construct the Grammar corresponding the that file
	//
	// Inputs:
	//		- std::string infile: the name of the file to read
	//
	// Outputs:
	//
	ContextFreeGrammar::ContextFreeGrammar(std::string infile) {

		// Open file input stream
		std::fstream fin{ infile };
		if (!fin) throw Errors(infile, 0, Errors::ErrorType::fileNotFound);

		filename = infile;

		// Read number of terminal symbols
		int nTermSymbols;
		fin >> nTermSymbols;
		if (fin.bad() || nTermSymbols < 1)
			throw Errors(filename, 1, Errors::ErrorType::nTermSymbolsError);

		char tempSymbol = ' ';
		// Read the terminal symbols and check for duplicates
		for (int i = 0; i < nTermSymbols; ++i)
			if (!termSymbols.contains(fin.peek())) {
				fin >> tempSymbol;
				termSymbols.insert(tempSymbol);
			}
			else
				throw Errors(filename, 2, Errors::ErrorType::duplicateTermSymbol);


		// Read number of non-terminal symbols
		int nNonTermSymbols;
		fin >> nNonTermSymbols;
		if (fin.bad() || nNonTermSymbols < 1)
			throw Errors(filename, 3, Errors::ErrorType::nNonTermSymbolsError);

		// Read the non-terminal symbols and check for duplicates
		// both in nonTermSymbols vector and in termSymbols vector
		for (int i = 0; i < nNonTermSymbols; ++i)
			if (!nonTermSymbols.contains(fin.peek()) || !termSymbols.contains(fin.peek())) {
				fin >> tempSymbol;
				nonTermSymbols.insert(tempSymbol);
			}
			else
				throw Errors(filename, 4, Errors::ErrorType::duplicateNonTermSymbol);


		// Read the initial symbol and check if it is defined in the non-terminal symbols
		fin >> initialSymbol;
		if(fin.bad() || !nonTermSymbols.contains(initialSymbol))
			throw Errors(filename, 5, Errors::ErrorType::initialSymbolError);


		// Read the number of rules
		int nRules;
		fin >> nRules;
		if (fin.bad() || nRules < 1)
			throw Errors(filename, 6, Errors::ErrorType::nRulesError);

		// Read rules and check for duplicates
		char ruleInput = ' ';
		std::string ruleOutput;
		maxRuleGenLen = 0;
		for (int i = 0; i < nRules; ++i) {
			
			fin >> ruleInput;
			if (!isspace(fin.peek())) fin.setstate(std::fstream::badbit);
			std::getline(fin, ruleOutput);

			std::string noSpaces;
			for (const char ch : ruleOutput)
				if (!isspace(ch))
					noSpaces += ch;
			ruleOutput = noSpaces;
			if (ruleOutput == EMPTYSTRING) ruleOutput = "";

			if (fin.bad() || std::find(ruleMap[ruleInput].begin(), ruleMap[ruleInput].end(),
								ruleOutput) != ruleMap[ruleInput].end())
				throw Errors(filename, 7 + i, Errors::ErrorType::rulesError);

			// Discard rules that won't make a difference
			if (std::string{ ruleInput } == ruleOutput) continue;

			ruleMap[ruleInput].push_back(ruleOutput);
			
			// Define the max length of the rule outputs
			bool onlyNonTerms = true;
			for (char ch : ruleOutput)
				if (termSymbols.contains(ch)) {
					onlyNonTerms = false;
					break;
				}
			if (onlyNonTerms)
				if (!maxRuleGenLen)
					maxRuleGenLen = ruleOutput.length();
				else
					if (maxRuleGenLen < ruleOutput.length())
						maxRuleGenLen = ruleOutput.length();

		}

		// Delete all the rules with empty outputs and create new rules from the
		// existing ones deleting the non-terminal symbol that outputs empty string
		// Example:

		// THIS IS A MESS (but it works (atleast for small inputs))
		// IT NEEDS MANY IMPROVEMENTS
		bool searchAgain = true;
		while (searchAgain) {
			searchAgain = false;
			std::unordered_map<char, std::vector<std::string>> dummyRuleMap;
			for (const auto& pairI : ruleMap) {
				for (int i = 0; i < pairI.second.size(); ++i) {
					if (!pairI.second[i].length()) {
						searchAgain = true;
						dummyRuleMap.clear();
						dummyRuleMap[pairI.first].push_back(std::string{ pairI.first });
						dummyRuleMap[pairI.first].push_back("");
						{
							std::vector<std::string> tempV = pairI.second;
							tempV.erase(tempV.begin() + i);
							ruleMap[pairI.first] = tempV;
						}
						for (const auto& pairJ : ruleMap) {
							for (int j = 0; j < pairJ.second.size(); ++j)
							{
								int newOutputsSize = 0;
								int lastRuleIndex = -1;
								for (int k = 0; k < pairJ.second[j].length(); ++k)
									if (pairJ.second[j][k] == pairI.first) {
										if (!newOutputsSize)
											newOutputsSize = 2;
										else
											newOutputsSize *= 2;
										lastRuleIndex = k;
									}
								if (lastRuleIndex == -1) continue;
								std::vector<std::string> newOutputs(newOutputsSize);
								size_t vIndex = 0;
								generate_words(pairJ.second[j], 0, dummyRuleMap, lastRuleIndex, newOutputs, vIndex);
								std::vector<std::string> tempV = pairJ.second;
								for (int k = 0; k < newOutputs.size(); ++k)
									if (!newOutputs[k].empty() &&
										std::find(tempV.begin(), tempV.end(), newOutputs[k]) == tempV.end())
										tempV.push_back(newOutputs[k]);
								ruleMap[pairJ.first] = tempV;
							}
						}
					}
				}
			}
		}

#ifdef SHOW_RULES
		std::cout << filename << '\n';
		for (const auto& pair : ruleMap) {
			std::cout << pair.first << ": ";
			for (std::string output : pair.second)
				std::cout << output << ' ';
			std::cout << '\n';
		}
		std::cout << '\n';
#endif // SHOW_RULES

	} // of constructor ContextFreeGrammar

//----------------------------------------------------------------

	// Check if a word can be generated from 'this' grammar
	//
	// Inputs:
	//		- std::string word: the given word
	//
	// Outputs:
	//		- bool true: 'word' was accepted
	//		- bool false: 'word' was NOT accepted
	//
	bool ContextFreeGrammar::check_word(std::string word) const {

		// Check if any symbol from 'word' is not part of the terminal symbols
		for (char ch : word)
			if (!termSymbols.contains(ch))
				return false;

		// Creating the root node for the tree
		TreeNode* root = new TreeNode{ nullptr, std::string{initialSymbol}, 0, 1 };
		
		// Adding the node to the frontier
		FrontierNode* frontierHead = new FrontierNode{ root };
		FrontierNode* frontierTail = frontierHead;

		// Creating a vector for the nodes that will be used to delete them
		// after searcing for a solution
		std::vector<TreeNode*> to_be_deleted_nodes;

		// Creating a set for the words added to the tree
		std::unordered_set<std::string> wordSet{ root->word };

		// A TreeNode-pointer vector to store the children generated in every loop
		std::vector<TreeNode*> children;

		// A variable to store the TreeNode that the solution will be found
		TreeNode* solutionNode = nullptr;

		// Loop until frontierHead variable "is empty" or if a solution was found
		while (true) {

			// Get the next to be expanded leef node
			TreeNode* currNode = get_front(&frontierHead, &frontierTail);
			to_be_deleted_nodes.push_back(currNode);
			if (!currNode) break;

			// Check if it holds the solution
			if (currNode->word == word) {
				solutionNode = currNode;
				break;
			}

#ifdef SHOW_DETAILS
			{
				using namespace std::chrono;

				auto time = system_clock::now();

				std::cout << currNode->word << '\n';
#endif // SHOW_DETAILS

				// Generate children nodes and add them to frontier
				generate_children(currNode, ruleMap, children, nonTermSymbols);

#ifdef SHOW_DETAILS
				std::cout << "Generation time: "
					<< duration_cast<milliseconds>(system_clock::now() - time).count()
					<< " ms\n";
#endif // SHOW_DETAILS

#ifdef SHOW_DETAILS
				time = system_clock::now();
#endif // SHOW_DETAILS

				// Prune the node if it is already in the tree
				// or if there is no possible way to find a solution throught it
				for (int i = 0; i < children.size(); ++i)
					if (prune(word, children[i], wordSet, termSymbols, nonTermSymbols, maxRuleGenLen)) {
#ifdef SHOW_PRUNED
						std::cout << children[i]->word << '\n';
#endif // SHOW_PRUNED
						delete children[i];
						children[i] = nullptr;
					}
					else {
#ifdef SHOW_GENERATED
						std::cout << children[i]->word << '\n';
#endif // SHOW_GENERATED
						wordSet.insert(children[i]->word);
					}

#ifdef SHOW_DETAILS
				std::cout << "Prune time: "
					<< duration_cast<milliseconds>(system_clock::now() - time).count()
					<< " ms\n\n";
			}
#endif // SHOW_DETAILS

			// Add children to the back of the frontier
			for (int i = 0; i < children.size(); ++i)
				if (children[i]) {
#ifdef HEURISTIC
					add_in_order(&frontierHead, &frontierTail, children[i]);
#else
					add_to_back(&frontierHead, &frontierTail, children[i]);
#endif // HEURISTIC
				}
			children.clear();

		} // while(true) (generation loop)

		// If a solution was found print it
		bool solutionFound = solutionNode;
		if(solutionNode)
			show_solution(solutionNode, nonTermSymbols);

		clear_tree(frontierHead);
		for (size_t i = 0; i < to_be_deleted_nodes.size(); ++i)
			delete to_be_deleted_nodes[i];
		return solutionFound;

	} // of function check_word

//----------------------------------------------------------------

	// Check if 'filename' is the same as 'this->filename'
	//
	// Inputs:
	//		- std::string grammarName: the filename of the grammar
	//		- std::string filename: the input file to check
	//
	// Outputs:
	//		- bool true: The grammar in 'filename' is already defined
	//		- bool false: The grammar in 'filename' is NOT already defined
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
