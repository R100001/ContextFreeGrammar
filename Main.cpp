
//------------------------------------------------------------------------

#include <iostream>
#include <filesystem>

//------------------------------------------------------------------------

#include "ConFreeGr.h"
#include "GramErr.h"

//------------------------------------------------------------------------

// Define a new grammar from an input file and expand the vector 'grammars'
//
// Inputs:
//		std::vector<Grammars::ContextFreeGrammar>& grammar: A vector containing all the defined grammars
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

// Check words in an already defined grammar
//
// Inputs:
//		const std::vector<Grammars::ContextFreeGrammar>& grammar: Defined grammar
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

	// Check words
	while (true) {

		// Get a new word
		std::cout << "Enter a word: ";
		std::string word;
		std::cin >> word;

		// Check the word and show message
		if (grammars[STATICCASTGRAMMAR(grammarNum) - 1].check_word(word))
			std::cout << "\nThe word '" << word << "' can be generated!\n\n";
		else
			std::cout << "The word '" << word << "' cannot be generated!\n\n";

		// More words?
		std::cout << "Do you want to enter another word? (yes,no): ";

		// Get answer
		std::string answer;

		while (answer != "yes" && answer != "no") {
			std::cin >> answer;
			std::cin.clear();
		}

		std::cout << '\n';

		if (answer == "no") break;

	} // while (true) (check words)

}

//------------------------------------------------------------------------

int main()
try {

	std::vector<Grammars::ContextFreeGrammar> grammar;

	// Program loop
	while (true) {

		// Show appropriate message 
		std::cout << "What do you want to do?\n";
		std::cout << "1: Insert new grammar\n";
		std::cout << "2: Use grammar\n";
		std::cout << "3: Exit\n\n";
		std::cout << ">> ";

		// Get input
		std::string input;
		std::cin >> input;
		std::cout << '\n';

		if (input == "1") {

			std::cout << "0: Back\n";
			std::cout << "1: Insert grammar\n";
			std::cout << "2: Insert grammars from a folder\n\n";

			int answer = -1;
			while (std::cin.bad() || answer < 0 || answer > 2) {
				std::cout << ">> ";
				std::cin >> answer;
				std::cin.clear();
			}
			std::cout << '\n';

			if (!answer) continue;

			define_new_grammar(grammar, answer - 1);
		}
		else if (input == "2") {

			use_grammar(grammar);
		}
		else if (input == "3") break;

	} // while (true) (program loop)

}
catch (const std::exception& e) {
	std::cerr << e.what() << '\n';
}
catch (...) {
	std::cerr << "Default exception...\n";
}

//------------------------------------------------------------------------
