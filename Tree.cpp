
//----------------------------------------------------------------

#include "Tree.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// Returns a pointer to the first node of the frontier and
	// removes it from it
	//
	// Inputs:
	//		-FrontierNode* frontierHead: A pointer to the first node of the frontier
	//		-FrontierNode* frontierTail: A pointer to the last node of the frontier
	//
	// Outputs:
	//
	TreeNode* get_front(FrontierNode** frontierHead, FrontierNode** frontierTail) {

		// If the frontier is empty return nullptr
		if (!(*frontierHead)) return nullptr;

		// Else extract the TreeNode
		TreeNode* current_node = (*frontierHead)->n;

		// Remove the extracted node from the frontier

		// If the frontier has only one node
		if (*frontierHead == *frontierTail) {
			delete *frontierHead;
			*frontierHead = nullptr;
			*frontierTail = nullptr;
		}
		else {
			FrontierNode* temp_node = (*frontierHead)->next;
			delete *frontierHead;
			*frontierHead = temp_node;
		}

		return current_node;
	}

//----------------------------------------------------------------

	// Add to the back of the frontier the new child
	//
	// Inputs:
	//		-FrontierNode* frontierHead: the head of the frontier that the child will be added
	//		-FrontierNode* frontierTail: the tail of the frontier that the child will be added
	//		-TreeNode* child: the child that will be added to the fronteir
	//
	// Outputs:
	//
	void add_to_back(FrontierNode** frontierHead, FrontierNode** frontierTail, TreeNode* child) {

		// Construct the new node
		FrontierNode* node = new FrontierNode(child);

		// If the frontier is empty
		if (!*frontierHead) { 
			*frontierHead = node;
			*frontierTail = node;
		} 
		else {
			(*frontierTail)->next = node;
			*frontierTail = (*frontierTail)->next;
		}
	}

//----------------------------------------------------------------

	// Check if the terminal symbols are in the right order
	//
	// Inputs:
	//		-const std::string& word: The word that we need to generate
	//		-const std::string& childWord: The current generated word
	//		-const std::unordered_set<char>& terminalSymbols: A map containing
	//			all the terminal symbols
	//
	// Outputs:
	//
	bool check_terminal_symbols(const std::string& word, const std::string& childWord,
		const std::unordered_set<char>& terminalSymbols) {

		// return false;

		//std::unordered_map<char, int> countTerminalsInChild;
		//std::unordered_map<char, int> countTerminalsInWord;

		// Count the occurences of every symbol in the word that we want to generate
		//for (const char symbol : word)
		//	if (!countTerminalsInWord.contains(symbol)) {
		//		countTerminalsInWord[symbol] = 1;
		//		countTerminalsInChild[symbol] = 0;
		//	}
		//	else
		//		countTerminalsInWord[symbol] += 1;

		// Construct a string having only the terminal symbols
		std::string onlyTerms;
		for (const char ch : childWord)
			if (terminalSymbols.contains(ch))
				onlyTerms.push_back(ch);

		if (onlyTerms.empty()) return false;
		if (onlyTerms.length() > word.length()) return true;

		// Count the occurences of every symbol in the current generated word
		// If a symbol has not been found in the word we want to generate prune it
		//for (const char symbol : onlyTerms)
		//	if (!countTerminalsInWord.contains(symbol))
		//		return true;
		//	else {
		//		countTerminalsInChild[symbol] += 1;

				// If a symbol appears more times in the generated word
				// we cant reach a solution so it needs pruning
		//		if (countTerminalsInChild[symbol] > countTerminalsInWord[symbol])
		//			return true;
		//	}

		// Check if all the symbols are in the final word 'word'
		// with the order generated in 'onlyTerms'
		// If it's not we can't reach a solution
		int onlyTermsIndex = 0;
		for (const char ch : word)
			if (ch == onlyTerms[onlyTermsIndex])
				++onlyTermsIndex;

		return onlyTermsIndex != onlyTerms.length();

	}

	bool check_non_terminal_symbols(const std::string& word, const std::string& childWord,
		const std::unordered_map<std::string, std::vector<char>>& reversedRules) {

		

		return false;
	}

	bool check_non_terminal_positions(const std::string& word, const std::string& childWord,
		const std::unordered_set<char>& nonTerminalSymbols) {

		return false;

		for (char symbol : childWord) {
			if (!nonTerminalSymbols.contains(symbol))
				if (std::count(word.begin(), word.end(), symbol) == 1);
		}

		
	}

//----------------------------------------------------------------

	// Prune the child if its word is already in the tree
	// or there is no way we can find a solution throught it
	//
	// Inputs:
	//		-TreeNode* child: the child to check
	//		-const std::unordered_set<std::string>& wordSet: The words that
	//			have been already generated
	//		-const std::unordered_set<char>& terminalSymbols: The terminal symbols
	//		-const std::unordered_set<char>& nonTerminalSymbols: The non-terminal symbols
	//		-const std::unordered_map<std::string, std::vector<char>>& reversedRules:
	//			The defined rules but reversed
	//
	// Outputs:
	//		-bool true: the child needs proning
	//		-bool false: the child does NOT need proning
	//
	bool prune(std::string word, TreeNode* child,
		const std::unordered_set<std::string>& wordSet,
		const std::unordered_set<char>& terminalSymbols,
		const std::unordered_set<char>& nonTerminalSymbols,
		const std::unordered_map<std::string, std::vector<char>>& reversedRules) {
		
		std::string childWord = child->word;

		// If 'wordSet' already contains the word that 'child' holds return true
		if (wordSet.contains(childWord)) return true;

		// Check if the terminal symbols are in the right order
		if (check_terminal_symbols(word, childWord, terminalSymbols))
			return true;

		// Check if a rule has expanded unnecessarily much
		if (check_non_terminal_symbols(word, childWord, reversedRules))
			return true;

		// Check if there is a reason for a non-terminal symbol to be in a certain position
		// Don't think this will make any real difference
		if (check_non_terminal_positions(word, childWord, nonTerminalSymbols))
			return true;

		return false;
	}

//----------------------------------------------------------------

	// Generate words using all the compinations of the given 'rules'
	//
	// We push_back to the vector 'words' only the generated words that all their
	// non-terminal symbols from the parent have been replaced. This happens since 
	// in the end all the non-terminal symbols will be replaced to reach the solution.
	// So generating children that not all non-terminal from their parent has been replaced
	// is unnecessary and it will add to the complexity of the whole program
	//
	//
	//								HOW THIS FUNCTION WORKS
	//==================================================================================
	// We have a word: a@a@a
	// In this word all the non-terminal symbols are indicated by '@' for the example
	// The size of the outer rules vector equals the number of '@'
	// So if there is only one '@' left (we know it because vIndex == rules.size()-1)
	// we replace it with the last set of rules
	// If there are more that one '@' we generate words by replacing the first '@'
	// and the function 'generate_words' is called with the new words and the vIndex
	// increased by one to point in the next set of rules
	//==================================================================================
	//
	// Inputs:
	//		-std::vector<std::string>& words: a vector for all the generated words
	//		-size_t& wordsIndex: index for the vector words
	//		-std::string word: the initial word to generate the new ones
	//		-const std::unordered_map<char, std::vector<std::string>>& ruleMap:
	//			A map containing all the replacements
	//		-std::vector<std::vector<std::string>> rules: the rules that will be applied
	//			divided for every non-terminal character
	//		-int startSearchIndex: Index to indicate where to start searching for the
	//			non-terminal symbol
	//		-int vIndex: index of the vector that will be used to apply the rules
	//
	//	Outputs:
	//
	void generate_words(std::vector<std::string>& words, size_t& wordsIndex, std::string word,
		const std::unordered_map<char, std::vector<std::string>>& ruleMap,
		const std::vector<std::vector<std::string>>& toBeUsedRules,
		int startSearchIndex, int vIndex) {


		// Find the position of the next non-terminal symbol
		int location = 0;
		for (int i = startSearchIndex; i < word.size(); ++i)
			if (ruleMap.contains(word[i])) {
				location = i;
				break;
			}

		// For every rule that applies to this non-terminal symbol
		for (int i = 0; i < toBeUsedRules[vIndex].size(); ++i) {

			// Copy the initial word
			std::string newWord = word;

			// And replace the non-terminal symbol with the std::string from the rule
			newWord.replace(newWord.begin() + location,
				newWord.begin() + location + 1, toBeUsedRules[vIndex][i]);

			// Push the new word to the vector
			// if the last non-terminal symbol has been reached
			if (vIndex == toBeUsedRules.size() - 1)
				words[wordsIndex++] = newWord;
			else
				// Generate words by changing the next non-terminal symbol
				generate_words(words, wordsIndex, newWord, ruleMap, toBeUsedRules,
					startSearchIndex + toBeUsedRules[vIndex][i].length(), vIndex + 1);
		}
	}


	// Generate children by using the rules in the non-terminal symbols
	//
	// Inputs:
	//		-TreeNode* node: the node that will be expanded
	//		-const std::unordered_map<char, std::vector<std::string>>& ruleMap: 
	//			a hash table for the rules
	//		-std::vector<TreeNode*>& children: the vector that the generated children
	//			will be put to
	//
	// Outputs:
	//
	void generate_children(TreeNode* node,
		std::unordered_map<char, std::vector<std::string>> ruleMap,
		std::vector<TreeNode*>& children) {

		// The rules that will be used to replace the non-terminal symbols
		std::vector<std::vector<std::string>> toBeUsedRules;

		// Fill the vector
		std::string word = node->word;
		unsigned long long generatedWordsSize = 0;
		for (int i = 0; i < word.size(); ++i)
			if (ruleMap.contains(word[i])) {
				toBeUsedRules.push_back(ruleMap[word[i]]);
				if (!generatedWordsSize)
					generatedWordsSize = ruleMap[word[i]].size();
				else
					generatedWordsSize *= ruleMap[word[i]].size();
			}

		// If there are no non-terminal symbols
		if (!toBeUsedRules.size()) return;

		// Generate all the new words
		std::vector<std::string> words(generatedWordsSize);
		size_t wordsIndex = 0;
		generate_words(words, wordsIndex, word, ruleMap, toBeUsedRules, 0, 0);

		// Create children
		for (const std::string& newWord : words)
			children.push_back(new TreeNode{node, newWord});

	}

//----------------------------------------------------------------

	// Print the solution found to the screen
	//
	// Inputs:
	//		-TreeNode* solutionNode: the node that holds the solution
	//
	// Outputs:
	//		
	void show_solution(TreeNode* solutionNode) {

		// A vector for the words generated to reach the solution
		std::vector<std::string> words;

		// Until we reach the root node fill 'words' with the words in the nodes
		while (solutionNode != nullptr) {
			words.push_back(solutionNode->word);
			solutionNode = solutionNode->parent;
		}

		// Print the words to the screen starting from the end of 'words' (the root node)
		for (int i = words.size() - 1; i >= 0; --i)
			if (i)
				std::cout << words[i] << " -> ";
			else
				std::cout << words[i];
	}

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
