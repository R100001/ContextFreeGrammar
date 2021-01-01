
//----------------------------------------------------------------

#include "Tree.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	// Returns a pointer to the first node of the frontier and
	// removes it from it
	//
	// Inputs:
	//		FrontierNode* frontierHead: A pointer to the first node of the frontier
	//		FrontierNode* frontierTail: A pointer to the last node of the frontier
	//
	// Outputs:
	//
	TreeNode* get_front(FrontierNode** frontierHead, FrontierNode** frontierTail) {

		// If the frontier is empty return nullptr
		if (!frontierHead) return nullptr;

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
	//		FrontierNode* frontierHead: the head of the frontier that the child will be added
	//		FrontierNode* frontierTail: the tail of the frontier that the child will be added
	//		TreeNode* child: the child that will be added to the fronteir
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

	// Prune the child if its word is already in the tree
	// or there is no way we can find a solution throught it
	//
	// Inputs:
	//		TreeNode* child: the child to check
	//		const std::unordered_set<std::string>& wordSet: the already generated words
	//
	// Outputs:
	//		bool true: the child needs proning
	//		bool false: the child does NOT need proning
	//
	bool prune(std::string finalWord, TreeNode* child, const std::unordered_set<std::string>& wordSet) {

		std::string word = child->word;

		// If 'wordSet' already contains the word that 'child' holds return true
		if (wordSet.contains(word)) return true;



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
	// In this word all the non-terminal symbols have been replaced with '@'
	// The size of the outer rules vector equals the number of '@'
	// So if there is only one '@' left (we know it because vIndex == rules.size()-1)
	// we replace it with the last set of rules
	// If there are more that one '@' we generate words by replacing the first '@'
	// and the function 'generate_words' is called with the new words and the vIndex
	// increased by one to point in the next set of rules
	//==================================================================================
	//
	// Inputs:
	//		std::vector<std::string>& words: a vector for all the generated words
	//		std::string word: the initial word to generate the new ones
	//		std::vector<std::vector<std::string>> rules: the rules that will be applied
	//			divided for every non-terminal character
	//		int vIndex: index of the vector that will be used to apply the rules
	//
	//	Outputs:
	//
	void generate_words(std::vector<std::string>& words, std::string word,
		std::vector<std::vector<std::string>> rules, int vIndex) {

		// If the last non-terminal symbol was reached
		if (vIndex == rules.size() - 1) {

			// Find the position of the next non-terminal symbol
			int location = word.find(replaceIndicator);

			// For every rule that applies to this position
			for (int i = 0; i < rules[vIndex].size(); ++i) {

				// Copy the initial word
				std::string newWord = word;

				// And replace the non-terminal symbol with the std::string from the rule
				newWord.replace(newWord.begin() + location,
					newWord.begin() + location + 1, rules[vIndex][i]);

				// We are on the last non-terminal symbol so we push the new word to the vector
				words.push_back(newWord);
			}
		}
		else { // If the non-terminal symbols from the parent have not been replaced yet

			// Find the position of the next non-terminal symbol
			int location = word.find(replaceIndicator);

			// For every rule that applies to this position
			for (int i = 0; i < rules[vIndex].size(); ++i) {

				// Copy the initial word
				std::string newWord = word;

				// And replace the non-terminal symbol with the std::string from the rule
				newWord.replace(newWord.begin() + location,
					newWord.begin() + location + 1, rules[vIndex][i]);

				// Generate words by changing the next non-terminal symbol
				generate_words(words, newWord, rules, vIndex + 1);
			}
		}
	}


	// Generate children by using the rules in the non-terminal symbols
	//
	// Inputs:
	//		TreeNode* node: the node that will be expanded
	//		const std::unordered_map<char, std::vector<std::string>>& ruleMap: a hash table for the rules
	//		std::vector<TreeNode*>& children: the vector that the generated children will be put to
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
		for (int i = 0; i < word.size(); ++i) {
			if (ruleMap.contains(word[i])) {
				toBeUsedRules.push_back(ruleMap[word[i]]);
				word.replace(word.begin() + i, word.begin() + i + 1, replaceIndicator);
			}
		}

		// If there are no non-terminal symbols
		if (!toBeUsedRules.size()) return;

		// Generate all the new words
		std::vector<std::string> words;
		generate_words(words, word, toBeUsedRules, 0);

		// Create children
		for (std::string newWord : words)
			children.push_back(new TreeNode{node, newWord});

	}

//----------------------------------------------------------------

	// Print the solution found to the screen
	//
	// Inputs:
	//		TreeNode* solutionNode: the node that holds the solution
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
