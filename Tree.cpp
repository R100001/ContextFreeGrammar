
//----------------------------------------------------------------

#include "Tree.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// Returns a pointer to the first node of the frontier and
	// removes it from it
	//
	// Inputs:
	//		- FrontierNode* frontierHead: A pointer to the first node of the frontier
	//		- FrontierNode* frontierTail: A pointer to the last node of the frontier
	//
	// Outputs:
	//		- TreeNode* current_node: A pointer to the first TreeNode of the frontier
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
	//		- FrontierNode* frontierHead: the head of the frontier that the child will be added
	//		- FrontierNode* frontierTail: the tail of the frontier that the child will be added
	//		- TreeNode* child: the child that will be added to the fronteir
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

	// Add in order the new child to the frontier depending on the heuristic score
	// and the depth of the node in the tree
	//
	// Inputs:
	//		- FrontierNode* frontierHead: the head of the frontier that the child will be added
	//		- FrontierNode* frontierTail: the tail of the frontier that the child will be added
	//		- TreeNode* child: the child that will be added to the frontier
	//
	// Outputs:
	//
	void add_in_order(FrontierNode** frontierHead, FrontierNode** frontierTail, TreeNode* child) {
		
		// Construct the new node
		FrontierNode* node = new FrontierNode(child);

		// If the frontier is empty
		if (!*frontierHead) {
			*frontierHead = node;
			*frontierTail = node;
		}
		else {

			FrontierNode* temp = *frontierHead;
			FrontierNode* previous = nullptr;

			// Less is better for both heuristic score and depth of the node
			while (temp) {
				if (temp->n->heuristic > node->n->heuristic ||
					(temp->n->heuristic == node->n->heuristic &&
						temp->n->depth >= node->n->depth)) {
					if(!previous) {
						node->next = *frontierHead;
						*frontierHead = node;
						return;
					}
					else {
						node->next = previous->next;
						previous->next = node;
						return;
					}
				}
				previous = temp;
				temp = temp->next;
			}

			previous->next = node;
			*frontierTail = node;
		}
	}

//----------------------------------------------------------------

	// Check if the terminal symbols are in the right order
	//
	// Inputs:
	//		- const std::string& word: The word that we need to generate
	//		- const std::string& childWord: The current generated word
	//		- const std::unordered_set<char>& terminalSymbols: A map containing
	//			all the terminal symbols
	//
	// Outputs:
	//		- bool true: the child needs pruning
	//		- bool false: the child does NOT need pruning
	//
	bool check_terminal_symbols(const std::string& word, const std::string& childWord,
		const std::unordered_set<char>& terminalSymbols) {

		
		// Check if all terminals are the same in 'word' and 'childWord'
		// from both start and end of the childWord until a non-terminal is reached
		long long wordBegin = 0;
		long long childWordBegin = 0;
		for (; childWordBegin < childWord.length(); ++childWordBegin, ++wordBegin) {
			if (!terminalSymbols.contains(childWord[childWordBegin]))
				break;

			if (childWord[childWordBegin] != word[wordBegin])
				return true;
		}

		long long wordEnd = word.length() - 1;
		long long childWordEnd = childWord.length() - 1;
		for (; childWordEnd >= 0; --childWordEnd, --wordEnd) {
			if (!terminalSymbols.contains(childWord[childWordEnd]))
				break;

			if (childWord[childWordEnd] != word[wordEnd])
				return true;
		}

		// Construct a string having only the rest(the ones that were not checked before)
		// terminal symbols
		std::string onlyTerms;
		for (long long i = childWordBegin; i <= childWordEnd; ++i)
			if (terminalSymbols.contains(childWord[i]))
				onlyTerms.push_back(childWord[i]);

		if (onlyTerms.empty()) return false;
		if (onlyTerms.length() > word.length()) return true;

		// Check if all the symbols are in the final word 'word'
		// with the order generated in 'onlyTerms'
		// If it's not we can't reach a solution
		long long onlyTermsIndex = 0;
		for (long long i = wordBegin; i <= wordEnd; ++i)
			if (word[i] == onlyTerms[onlyTermsIndex])
				++onlyTermsIndex;

		return onlyTermsIndex != onlyTerms.length();

	} // of function check_terminal_symbols

//----------------------------------------------------------------

	// Check if the concurrent non-terminal symbols have expanded unnecessarily much between two terminals
	// This function needs check_terminal_symbols to be called first to make sure the terminal symbols
	// is in the right order
	// Inputs:
	//		- const std::string& word: the word we want to generate
	//		- const std::string& childWord: the current generated word
	//		- const std::unordered_set<char>& nonTerminalSymbols: the non-terminal symbols of the grammar
	// 
	// Outputs:
	//		- bool true: the child needs pruning
	//		- bool false: the child does NOT need pruning
	//
	bool check_non_terminal_positions(const std::string& word, const std::string& childWord,
		const std::unordered_set<char>& nonTerminalSymbols) {

		if (childWord.length() < 2) return false;

		// Search for the first non-terminal from the start of after terminals
		for (int i = 0; i < childWord.length(); ++i) {

			// If the current symbol is a non-terminal
			if (nonTerminalSymbols.contains(childWord[i])) {

				// Start searching for the last concurrent non-terminal
				int j = i;
				for (; j < childWord.length(); ++j)
					if (!nonTerminalSymbols.contains(childWord[j]))
						break;

				// If the whole childWord has non terminals
				if (i == 0 && j == childWord.length()) return false;

				// If the concurrent non-terminals start from the start
				if (i == 0) {

					// Count the occurences of the first terminal after the concurrent non-terminals in 'childWord'
					int countChildWordJInChildWord = 0;
					for (int k = j; k < childWord.length(); ++k)
						if (childWord[j] == childWord[k])
							++countChildWordJInChildWord;

					// Check if the position of the first terminal after the concurrent non-terminals in 'childWord' is greater
					// than the position of the same symbol in 'word' after ignoring countChildWordJInChildWord - 1 symbols from the end
					for (int k = word.length() - 1; k >= 0; --k)
						if (word[k] == childWord[j])
							if (countChildWordJInChildWord == 1) {
								if (k < j)
									return true;
								else
									break;
							}
							else --countChildWordJInChildWord;
				}
				// If the concurrent non-terminals end when the childWord ends
				else if (j == childWord.length()) {
					
					// Adjusting 'i' so it will point on the first terminal before the non-terminals
					--i;

					// Count the occurences of the first terminal before the concurrent non-terminals in 'childWord'
					int countChildWordIInChildWord = 0;
					for (int k = 0; k <= i; ++k)
						if (childWord[i] == childWord[k])
							++countChildWordIInChildWord;

					// Check if the position of the first terminal before the concurrent non-terminals in 'childWord' is lower
					// than the position of the same symbol in 'word' after ignoring countChildWordJInChildWord - 1 symbols from the end
					for (int k = 0; k < word.length(); ++k)
						if (word[k] == childWord[i])
							if (countChildWordIInChildWord == 1) {
								if (word.length() - k < childWord.length() - i)
									return true;
								else
									break;
							}
							else --countChildWordIInChildWord;
				}
				// If the concurrent non-terminals are somewhere in between
				else {

					// Adjusting 'i' so it will point on the first terminal before the non-terminals
					--i;

					// Count the occurences of the first terminal before the concurrent non-terminals in 'childWord'
					int countChildWordIInChildWord = 0;
					for (int k = 0; k <= i; ++k)
						if (childWord[i] == childWord[k])
							++countChildWordIInChildWord;

					// Count the occurences of the first terminal after the concurrent non-terminals in 'childWord'
					int countChildWordJInChildWord = 0;
					for (int k = j; k < childWord.length(); ++k)
						if (childWord[j] == childWord[k])
							++countChildWordJInChildWord;

					// Find the position of the countChildWordIInChildWord'th (1st, 2nd, 3rd, 4->th<- ...) terminal in 'word'
					int posOfChildWordIInWord = 0;
					for (int k = 0; k < word.length(); ++k) {
						if (word[k] == childWord[i])
							if (countChildWordIInChildWord == 1) {
								posOfChildWordIInWord = k;
								break;
							}
							else --countChildWordIInChildWord;
					}

					// Find the position of the countChildWordJInChildWord'th (1st, 2nd, 3rd, 4->th<- ...) terminal in 'word'
					// starting from the end of 'word'
					int posOfChildWordJInWord = 0;
					for (int k = word.length() - 1; k >= 0; --k) {
						if (word[k] == childWord[j])
							if (countChildWordJInChildWord == 1) {
								posOfChildWordJInWord = k;
								break;
							}
							else --countChildWordJInChildWord;
					}

					if (j - i > posOfChildWordJInWord - posOfChildWordIInWord)
						return true;

				}
				i = j;
			}
		}
		return false;

	} // of function check_non_terminal_symbols

//----------------------------------------------------------------

	// Check if a rule generated output with two or more non-terminals that are the same
	// as the input and this non-terminals outputed the same thing
	// Example: E -> ExE -> ExE x ExE
	// We can prune this word because at some point some other rule will be used
	// to progress to the final word
	// This means that when the new words are generated there is a word that has already
	// used another rule at some place and we can still generate the wanted word
	//
	// I'm not sure if this function will make any real difference because
	// it will reduce the degree of the tree by 1/rules_with_the_same_input^2 and
	// it will add a lot to the complexity when it comes to keeping information
	// about the rule generation when generating the words
	// The expression 1/rules_with_the_same_input^2 indicates that if we have
	// 10 rules with the same input it will only reduce the degree of the tree by 1%
	//
	bool check_rule_generation(const std::string& word, const std::string& childWord) {



		return false;
	}

//----------------------------------------------------------------

	// Prune the child if its word is already in the tree
	// or there is no way we can find a solution throught it
	//
	// Inputs:
	//		- TreeNode* child: the child to check
	//		- const std::unordered_set<std::string>& wordSet: The words that
	//			have been already generated
	//		- const std::unordered_set<char>& terminalSymbols: The terminal symbols
	//		- const std::unordered_set<char>& nonTerminalSymbols: The non-terminal symbols
	//
	// Outputs:
	//		- bool true: the child needs proning
	//		- bool false: the child does NOT need proning
	//
	bool prune(std::string word, TreeNode* child,
		const std::unordered_set<std::string>& wordSet,
		const std::unordered_set<char>& terminalSymbols,
		const std::unordered_set<char>& nonTerminalSymbols,
		const size_t maxRuleGenLen) {

		std::string childWord = child->word;

		// The least length of a rule output is 1 so we can prune any childWord that has
		// more symbols than word
		if (childWord.length() > word.length()) return true;

		// If 'wordSet' already contains the word that 'child' holds return true
		if (wordSet.contains(childWord)) return true;

		// Check if the terminal symbols are in the right order
		if (check_terminal_symbols(word, childWord, terminalSymbols))
			return true;

		// Check if concurrent non-terminals have expanded to unnecessarily much
		if (check_non_terminal_positions(word, childWord, nonTerminalSymbols))
			return true;

		// Check if a rule has expanded unnecessarily much
		if (check_rule_generation(word, childWord))
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
	// So if there is only one '@' left (we know it because rulesIndex == rules.size()-1)
	// we replace it with the last set of rules
	// If there are more that one '@' we generate words by replacing the first '@'
	// and the function 'generate_words' is called with the new words and the rulesIndex
	// increased by one to point in the next set of rules
	//==================================================================================
	//
	// Inputs:
	//		- std::string word: the initial word to generate the new ones
	//		- int location: Index to indicate where to start searching for the
	//			non-terminal symbol and where the replacement will take place
	//		- std::unordered_map<char, std::vector<std::string>>& ruleMap:
	//			A map containing all the replacements
	//		- lastRuleIndex: index of the last replacement to indicate when to stop generating
	//		- std::vector<std::string>& words: a vector for all the generated words
	//		- size_t& wordsIndex: index for the vector words
	//
	//	Outputs:
	//
	void generate_words(std::string word, int location,
		std::unordered_map<char, std::vector<std::string>>& ruleMap,
		int lastRuleIndex, std::vector<std::string>& words, size_t& wordsIndex) {

		// Find the position of the next non-terminal symbol
		for (int i = location; i < word.size(); ++i)
			if (ruleMap.contains(word[i])) {
				location = i;
				break;
			}

		// For every rule that applies to this non-terminal symbol
		for (int i = 0; i < ruleMap[word[location]].size(); ++i) {

			// Copy the initial word
			std::string newWord = word;

			// And replace the non-terminal symbol with the std::string from the rule
			newWord.replace(newWord.begin() + location,
				newWord.begin() + location + 1, ruleMap[word[location]][i]);

			// Push the new word to the vector
			// if the last non-terminal symbol has been reached
			if (location == lastRuleIndex)
				words[wordsIndex++] = newWord;
			else
				// Generate words by changing the next non-terminal symbol
				generate_words(newWord, location + ruleMap[word[location]][i].length(),
					ruleMap, lastRuleIndex + ruleMap[word[location]][i].length() - 1,
					words, wordsIndex);
		}
	}


	// Generate children by using the rules in the non-terminal symbols
	//
	// Inputs:
	//		- TreeNode* node: the node that will be expanded
	//		- const std::unordered_map<char, std::vector<std::string>>& ruleMap: 
	//			a hash table for the rules
	//		- std::vector<TreeNode*>& children: the vector that the generated children
	//			will be put to
	//		- const std::unordered_set<char>& nonTermSymbols: the non-terminal symbols of the grammar
	//
	// Outputs:
	//
	void generate_children(TreeNode* node,
		std::unordered_map<char, std::vector<std::string>> ruleMap,
		std::vector<TreeNode*>& children,
		const std::unordered_set<char>& nonTermSymbols) {

		// Fill the vector
		std::string word = node->word;
		int lastRulePos = -1;
		unsigned long long generatedWordsSize = 0;
		for (int i = 0; i < word.size(); ++i)
			if (ruleMap.contains(word[i])) {
				lastRulePos = i;
				if (!generatedWordsSize)
					generatedWordsSize = ruleMap[word[i]].size();
				else
					generatedWordsSize *= ruleMap[word[i]].size();
			}

		// If there are no non-terminal symbols
		if (lastRulePos == -1) return;

		// Generate all the new words
		std::vector<std::string> words(generatedWordsSize);
		//std::vector<std::vector<int>> rulesPos(generatedWordsSize);
		size_t wordsIndex = 0;
		generate_words(word, 0, ruleMap, lastRulePos, words, wordsIndex);

		// Create children
		children = std::vector<TreeNode*>(generatedWordsSize);
		for (size_t i = 0; i < words.size(); ++i) {
			unsigned int countNonTerms = 0;
			for (char ch : words[i])
				if (nonTermSymbols.contains(ch))
					++countNonTerms;
			children[i] = new TreeNode{ node, words[i], node->depth + 1, countNonTerms };
		}

	}

	// Clear the tree to avoid memory leaks by deleting all the nodes
	// Inputs:
	//		- FrontierNode* head: The head of the frontier
	// Outputs:
	//
	void clear_tree(FrontierNode* head)
	{
		FrontierNode* temp = head;
		while (head) {
			
			delete head->n;
			temp = head->next;
			delete head;
			head = temp;
		}
	}

//----------------------------------------------------------------

	// Print the solution found to the screen
	//
	// Inputs:
	//		- TreeNode* solutionNode: the node that holds the solution
	//		- const std::unordered_set<char>& nonTermSymbols: the non-terminal symbols
	//			of the grammar
	//
	// Outputs:
	//		
	void show_solution(TreeNode* solutionNode, const std::unordered_set<char>& nonTermSymbols) {

		// A vector for the words generated to reach the solution
		std::vector<std::string> words;

		// Until we reach the root node fill 'words' with the words in the nodes
		while (solutionNode != nullptr) {
			//if (solutionNode->parent) {
			//	std::string newWord;
			//	for (char ch : solutionNode->parent->word) {
			//		if(ch)
			//	}
			//}
			//else
			words.push_back(solutionNode->word);
			solutionNode = solutionNode->parent;
		}

		// Print the words to the screen starting from the end of 'words' (the root node)
		for (int i = words.size() - 1; i >= 0; --i)
			if (i)
				std::cout << words[i] << " ->\n";
			else
				std::cout << words[i];
	}

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
