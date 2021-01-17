
//----------------------------------------------------------------

#pragma once

//----------------------------------------------------------------

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

//----------------------------------------------------------------

#include "Macros.h"

#include "Rule.h"

//----------------------------------------------------------------

namespace Grammars {

//----------------------------------------------------------------

	struct TreeNode {

		// Default constructor
		TreeNode() 
			:parent{ nullptr }, word{ std::string() }, depth{ 0 }, heuristic{ 0 } {}

		// Constructor to initialize children
		TreeNode(TreeNode* p, std::string w, unsigned int d, unsigned int h)
			:parent{ p }, word{ w }, depth{ d }, heuristic{ h } {}

		TreeNode* parent;	// The parent node
		std::string word;	// The word on the current node
		unsigned int depth;
		unsigned int heuristic;

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
	
	// Add the child in order in the frontier
	void add_in_order(FrontierNode** frontierHead, FrontierNode** frontierTail, TreeNode* child);

	// Prune any child that holds a word that is already on the tree
	// or any child that holds a word that cannot generate the solution
	bool prune(std::string finalWord, TreeNode* child,
		const std::unordered_set<std::string>& wordSet,
		const std::unordered_set<char>& terminalSymbols,
		const std::unordered_set<char>& nonTerminalSymbols,
		const size_t maxRuleGenLen);

	// Generate children by applying the rules to theirs parent's word
	void generate_children(TreeNode* node,
		std::unordered_map<char, std::vector<std::string>> ruleMap,
		std::vector<TreeNode*>& children,
		const std::unordered_set<char>& nonTermSymbols);

	// Clear the tree to avoid memory leaks
	void clear_tree(FrontierNode* head);

	// Print the solution to the screen
	void show_solution(TreeNode* solutionNode);

//----------------------------------------------------------------

} // of namespace Grammars

//----------------------------------------------------------------
