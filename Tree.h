
//----------------------------------------------------------------

#pragma once

//----------------------------------------------------------------

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

//----------------------------------------------------------------

#define EMPTYSTRING "@"

#define STATICCASTWORDINDEX static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	/*struct TreeNode {

		// Default constructor
		TreeNode() :parent{ nullptr }, rulePos{ -1 }, rule{ Rule() },
			word{ std::string() }, depth{ -1 } {}

		// Constructor to initialize children
		TreeNode(TreeNode* p, int rPos, Rule r, std::string w, int d)
			:parent{ p }, rulePos{ rPos }, rule{ r }, word{ w }, depth{ d } {}

		TreeNode* parent;	// The parent node
		int rulePos;		// Position of the non-Terminal char...
		Rule rule;			// ...of the parent that changed with 'rule'
		std::string word;	// The word on the current node
		int depth;			// Depth of the node from the root node

	}; // of struct TreeNode*/

	struct TreeNode {

		// Default constructor
		TreeNode() :parent{ nullptr }, word{ std::string() } {}

		// Constructor to initialize children
		TreeNode(TreeNode* p, std::string w)
			:parent{ p }, word{ w } {}

		TreeNode* parent;	// The parent node
		std::string word;	// The word on the current node

	}; // of struct TreeNode

//----------------------------------------------------------------

	struct FrontierNode {

		// Constructor to add a new node to the frontier
		FrontierNode(TreeNode* node) : n{ node }, next{ nullptr } {}

		TreeNode* n;		 // The node that will be checked
		FrontierNode* next;  // Next node

	}; // of struct FrontierNode

//----------------------------------------------------------------

	// Get the first node that is in the frontier to check
	// if it is the solution or to expand it
	TreeNode* get_front(FrontierNode** frontierHead, FrontierNode** frontierTail);

	// Add the new child to the back of the frontier
	void add_to_back(FrontierNode** frontierHead, FrontierNode** frontierTail, TreeNode* child);

	// Prune any child that holds a word that is already on the tree
	// or any child that holds a word that cannot generate the solution
	bool prune(std::string finalWord, TreeNode* child,
		const std::unordered_set<std::string>& wordSet,
		const std::unordered_set<char>& terminalSymbols,
		const std::unordered_set<char>& nonTerminalSymbols,
		const std::unordered_map<std::string, std::vector<char>>& reversedRules);

	// Generate children by applying the rules to theirs parent's word
	void generate_children(TreeNode* node,
		std::unordered_map<char, std::vector<std::string>> ruleMap,
		std::vector<TreeNode*>& children);

	// Print the solution to the screen
	void show_solution(TreeNode* solutionNode);

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
