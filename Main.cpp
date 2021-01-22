
//------------------------------------------------------------------------

#include <iostream>
#include <filesystem>
#include <sstream>

//------------------------------------------------------------------------

#include "Macros.h"

#ifdef SHOW_TIME
#include <chrono>
#endif // SHOW_TIME

//------------------------------------------------------------------------

#include "ConFreeGr.h"
#include "GramErr.h"


//------------------------------------------------------------------------

// Define a new grammar from an input file
//
// Inputs:
//		- std::vector<Grammars::ContextFreeGrammar>& grammar: 
//			A vector containing all the defined grammars
//
// Outputs:
//
void define_new_grammar(std::vector<Grammars::ContextFreeGrammar>& grammars, bool folder) {

	// Get input filename
	std::cout << "Type 'back' to go back\n";
	if (folder)
		std::cout << "Folder name: ";
	else
		std::cout << "Filename: ";
	std::string name;
	std::cin >> name;
	std::cout << '\n';
	if (name == "back") return;

	// Push the new grammar to the vector
	try {
		if (folder) {

			// For every file in the folder 'name'
			for (const auto& infile : std::filesystem::directory_iterator(name)) {

				// Check if any grammar is already defined
				bool alreadyDefined = false;
				for (Grammars::ContextFreeGrammar& grammar : grammars) {
					if (grammar == infile.path().string()) {
						alreadyDefined = true;
						break;
					}
				}

				// If it not defined define it
				if (!alreadyDefined)
					grammars.push_back(Grammars::ContextFreeGrammar{ infile.path().string() });
			}
		}
		else {

			// Check if grammar is already defined
			for (Grammars::ContextFreeGrammar& grammar : grammars)
				if (name == grammar) return;

			// If it not defined define it
			grammars.push_back(Grammars::ContextFreeGrammar{ name });
		}
	}
	catch (const Grammars::Errors& e) {
		std::cerr << e.what() << "\n\n";
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << "\n\n";
	}

}

//------------------------------------------------------------------------

// Delete grammars
// Inputs:
//		- std::vector<Grammars::ContextFreeGrammar>& grammars: The defined grammars
//
// Outputs:
//		
void delete_grammar(std::vector<Grammars::ContextFreeGrammar>& grammars) {

	// Check if any grammar is defined
	if (!grammars.size()) {
		std::cout << "There is no grammar to delete\n\n";
		return;
	}

	// Show appropriate message
	std::cout << "Choose grammars to delete (1-" << grammars.size() << "):\n";
	std::cout << "You can delete multiple grammars by typing 'all' to delete all of them\n";
	std::cout << "or by typing the numbers of the grammars seperated with space\n";
	std::cout << "0: Back\n";
	for (unsigned int i = 0; i < grammars.size(); ++i)
		std::cout << i + 1 << ": " << (std::string)grammars[i] << '\n';
	std::cout << '\n';

	// Get answer
	std::string answer;
	std::cout << ">> ";

	// Check answer
	if (isalpha(std::cin.peek())) {

		// Delete all grammars
		std::cin >> answer;
		if (answer == "all")
			grammars.clear();
	}
	else if (isdigit(std::cin.peek())) {

		// Delete specific grammars
		int grammarNum = 0;
		std::vector<int> toBeDeletedGrammars;

		std::getline(std::cin, answer);
		std::stringstream ssanswer{ answer };

		while (ssanswer) {
			ssanswer >> grammarNum;
			if (std::find(toBeDeletedGrammars.begin(), toBeDeletedGrammars.end(), grammarNum) == toBeDeletedGrammars.end() &&
				grammarNum > 0 && grammarNum <= grammars.size())
				toBeDeletedGrammars.push_back(grammarNum - 1);
			if (ssanswer.bad())
				break;
		}

		std::sort(toBeDeletedGrammars.begin(), toBeDeletedGrammars.end(), std::greater<int>());
		for (int num : toBeDeletedGrammars)
			grammars.erase(grammars.begin() + num);
	}

	std::cout << '\n';
}

//------------------------------------------------------------------------

// Check words in an already defined grammar
//
// Inputs:
//		- const std::vector<Grammars::ContextFreeGrammar>& grammar: Defined grammar
//
// Outputs:
//
void use_grammar(const std::vector<Grammars::ContextFreeGrammar>& grammars) {

	// Check if any grammar is defined
	if (!grammars.size()) {
		std::cout << "There is no grammar to use. You need to define one!\n\n";
		return;
	}

	// Show appropriate message
	std::cout << "Choose an grammar to use (1-" << grammars.size() << "):\n";
	std::cout << "0: Back\n";
	for (unsigned int i = 0; i < grammars.size(); ++i)
		std::cout << i + 1 << ": " << (std::string)grammars[i] << '\n';
	std::cout << '\n';

	// Get answer
	int grammarNum = -1;
	while (std::cin.bad() || grammarNum < 0 || grammarNum > grammars.size()) {
		std::cout << ">> ";
		std::cin >> grammarNum;
		std::cin.clear();
	}
	std::cout << '\n';
	if (!grammarNum) return;
	std::cin.ignore();

	// Check words
	while (true) {

		// Get a new word
		std::cout << "Enter a word: ";
		std::string word;
		std::getline(std::cin, word);

		std::string noSpaces;
		for (const char ch : word)
			if (!isspace(ch))
				noSpaces += ch;
		
#ifdef SHOW_TIME
		{
			using namespace std::chrono;

			auto time = system_clock::now();
#endif // SHOW_TIME

			// Check the word and show message
			if (grammars[STATICCASTGRAMMAR(grammarNum) - 1].check_word(noSpaces))
				std::cout << "\nThe word '" << word << "' can be generated!\n\n";
			else
				std::cout << "The word '" << word << "' cannot be generated!\n\n";

#ifdef SHOW_TIME
			std::cout << "Time: " 
				<< duration_cast<milliseconds>(system_clock::now() - time).count()
				<< " ms\n\n";
		}
#endif // SHOW_TIME

		// More words?
		std::cout << "Do you want to enter another word? (yes,no): ";

		// Get answer
		std::string answer;

		while (answer != "yes" && answer != "no") {
			std::cin >> answer;
			std::cin.clear();
		}
		std::cout << '\n';
		std::cin.ignore();

		if (answer == "no") break;

	} // while (true) (check words)

}

//------------------------------------------------------------------------

int main()
try {

	std::vector<Grammars::ContextFreeGrammar> grammars;

	// Program loop
	while (true) {

		// Show appropriate message 
		std::cout << "What do you want to do?\n";
		std::cout << "1: Insert new grammar\n";
		std::cout << "2: Delete a grammar\n";
		std::cout << "3: Use a grammar\n";
		std::cout << "4: Exit\n\n";
		std::cout << ">> ";

		// Get input
		std::string input;
		std::cin >> input;
		std::cin.ignore();
		std::cout << '\n';

		if (input == "1") {

			std::cout << "0: Back\n";
			std::cout << "1: Insert a grammar\n";
			std::cout << "2: Insert grammars from a folder\n\n";

			int answer = -1;
			while (std::cin.bad() || answer < 0 || answer > 2) {
				std::cout << ">> ";
				std::cin >> answer;
				std::cin.clear();
			}
			std::cout << '\n';

			if (!answer) continue;

			define_new_grammar(grammars, answer - 1);
		}
		else if (input == "2") {
			delete_grammar(grammars);
		}
		else if (input == "3") {

			use_grammar(grammars);
		}
		else if (input == "4") break;

	} // while (true) (program loop)

}
catch (const std::exception& e) {
	std::cerr << e.what() << '\n';
}
catch (...) {
	std::cerr << "Default exception...\n";
}

//------------------------------------------------------------------------
