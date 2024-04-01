#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>
#include <unordered_map>

using namespace std;

//  variables to organized tokens 
enum TokenType {
    KEYWORD,
    IDENTIFIER,
    OPERATOR,
    DELIMITER,
    LITERAL,
    UNKNOWN
};

// map containing tokens to be printed
const unordered_map<TokenType, string> typeName = {
    {KEYWORD, "Keyword"},
    {IDENTIFIER, "Identifier"},
    {OPERATOR, "Operator"},
    {DELIMITER, "Delimiter"},
    {LITERAL, "Literal"},
    {UNKNOWN, "Unknown"}
};

struct Token {
    string value;
    TokenType type;
};

// sets containing tokens 
unordered_set<string> keywords = {"int", "return", "if", "else", "while", "for", "do", "cout", "using", "namespace","endl"};
unordered_set<char> operators = {'+', '-', '*', '/', '=', '<', '>'};
unordered_set<char> delimiters = {';', '(', ')', '{', '}', ','};


//START OF categorize
/**
 * This function assists the "tokenize" function in categorizing tokens. Each token
 * is assigned a type which will be used as the key in the map containing all
 * tokens.
 * 
 * @param  token: token given by the "tokenize" function
 * 
 * @return TokenType to be added to the map of Tokens
 */
TokenType categorize(const string& token) {
    if (keywords.find(token) != keywords.end()) {
        return KEYWORD;
    } else if (operators.find(token[0]) != operators.end() && !(token[0] == '<' && token.back() == '>')) {
        return OPERATOR;
    } else if (delimiters.find(token[0]) != delimiters.end()) {
        return DELIMITER;
    } else if (isdigit(token[0]) || (token[0] == '"' && token.back() == '"')) {
        return LITERAL;
    } else {
        return IDENTIFIER;
    }
}
//END OF categorize


// START OF tokenize
/**
 * This function takes in the string of code given by the user and assigns each token
 * found in the code a token.
 * 
 * @param  input: string of code provided by the user
 * 
 * @return vector containing values of type Token
 */
vector<Token> tokenize(const string& input) {
    vector<Token> tokens;
    string token;

    bool inQuotes = false;
    bool inHeader = false;

    //parses through string parameter
    for (char ch : input) {

        // handles the end of a string literal or header
        if((inHeader && ((ch == '>') || (ch == '<'))) || (inQuotes && (ch == '"')))
        {
            if (ch == '"') {inQuotes = false;}

            if (ch == '>' || ch == '<'){inHeader = false;}
            
            token += ch;
            continue;
        }

        // marks the start of a string literal or a header
        if((!inHeader && (ch == '<')) || (!inQuotes && (ch == '"'))) 
        {
            if (ch == '"') {inQuotes = true;}

            if (ch == '<'){inHeader = true;}

            token += ch;
            continue;
        }

        // if the characters is a space or operator or delimiter
        if ((isspace(ch) || operators.find(ch) != operators.end() || delimiters.find(ch) != delimiters.end()) && !inQuotes && !inHeader) {

            // categorizing a token when a space appears, signifying the end of a token
            // that is not an operator or delimiter
            if (!token.empty()) {
                tokens.push_back({token, categorize(token)});
                token.clear();
            }

            // if the token isn't a space, it is categorizing an operator or delimiter
            // if it is a space, it means it just categorized anything else
            if (!isspace(ch)) {
                string op(1, ch);
                tokens.push_back({op, categorize(op)});
            }

        // adds character to string token if there is no space or delimiter or operator
        } 
        else {
            token += ch;
        }
    }



    // categorizes last token
    if (!token.empty()) {
        tokens.push_back({token, categorize(token)});
    }
    return tokens;
}
// END OF tokenize



// START OF printTokensByCategory
/**
 * This function prints each token found in the given vector of Tokens. This function
 * only prints on of each token found in the code and the total number
 * of tokens found in the code.
 * 
 * @param  tokens: vector of tokens to be printed
 * 
 */
void printTokensByCategory(const vector<Token>& tokens) {
    unordered_map<TokenType, vector<string> > categorizedTokens;
    for (const auto& token : tokens) {
        if (find(categorizedTokens[token.type].begin(), categorizedTokens[token.type].end(), token.value) == categorizedTokens[token.type].end())
        {
            categorizedTokens[token.type].push_back(token.value);
        }
    }

    for (const auto& category : categorizedTokens) {
        cout << typeName.at(category.first) << ":" << endl;
        for (const auto& value : category.second) {
            cout << "  " << value << endl;
        }
    }

    cout << "There are " << tokens.size() << " tokens." << endl;
}




// START OF removeExcessSpacesAndComments
/**
 * This function removes the spaces and comments from any string
 * passthat is passed as a parameter. 
 *
 * @param  input:  string of code that will be stripped of spaces and comments
 * 
 * @return returns a string of code without excess space and comments
 */
string removeExcessSpacesAndComments(const string& input) {
    string result;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;
    bool newLine = true; // Assume file starts with a "new line" for logic purposes
    bool lastCharWasNewLine = false; // Track if the last character added was a new line

    for (size_t i = 0; i < input.length(); ++i) {

        // Handles the end of single-line comment
        if (inSingleLineComment && input[i] == '\n') {
            inSingleLineComment = false;
            // Do not immediately add new line, wait for non-whitespace character

        // Handles the end of a multiline comment
        } else if (inMultiLineComment && input[i] == '*' && i + 1 < input.length() && input[i + 1] == '/') {
            inMultiLineComment = false;
            i++; // Skip the '/' of the ending comment marker
            continue;
        }

        // Start of comment
        if (!inSingleLineComment && !inMultiLineComment && input[i] == '/' && i + 1 < input.length()) {
            if (input[i + 1] == '/') {
                inSingleLineComment = true;
                i++; // Skip the second '/'
                continue;
            } else if (input[i + 1] == '*') {
                inMultiLineComment = true;
                i++; // Skip the '*'
                continue;
            }
        }

        // skips characters while parsing through a comment
        if (inSingleLineComment || inMultiLineComment) {
            continue; // Skip characters within comments
        }

        // Handle new lines and spaces
        if (input[i] == '\n') {
            if (!lastCharWasNewLine && !newLine) { // Add new line if previous character wasn't a new line
                result += input[i];
                lastCharWasNewLine = true; // Mark that a new line was just added
            }

            newLine = true; // Ready to accept non-whitespace characters
        
        
        } else if (isspace(input[i])) {
            if (newLine || (i + 1 < input.length() && isspace(input[i + 1]) && input[i + 1] != '\n')) {
                continue; // Skip leading and consecutive spaces, but allow for spaces before new line
            } else {
                if (!lastCharWasNewLine) { // Add a space if the last character wasn't a new line
                    result += ' ';
                }
            }
        } else {
            newLine = false;
            lastCharWasNewLine = false; // Reset since a non-whitespace character is added
            result += input[i];
        }
    }

    return result;
}



// START OF readFileIntoString
/**
 * This function reads a file given by the user and returns 
 * a file as a string without excess spaces and comments.
 * If the input could not be read, the function displays an error message
 * 
 * @param  path: name of file given by the user
 * 
 * @return returns a string of text found within the  file
 */
// reads file given by user and returns cpp file as a string
// gives error message if the file could not be read or opened
string readFileIntoString(const string& path) {
    ifstream input_file(path);

    // error message displays if the file could not be read
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '" << path << "'" << endl;
        return "";
    }

    // reads file as a string and places it into file_content
    stringstream buffer;
    buffer << input_file.rdbuf();
    string fileContent = buffer.str();

    return removeExcessSpacesAndComments(fileContent);
}
//END OF readFileIntoString





//START OF printProcessedInput
/**
 * This function prints the 
 * 
 * Long description
 * 
 * @param  value1:  description
 * @param  value2:  description
 * 
 * @return description
 */
// prints tokens found within the file given by the user
void printProcessedInput(const string& input) {
    cout << "Processed file content:\n" << input << endl;
}
//END OF printProcessedInput


// START of main
int main() {
    cout << "Please enter the file path for the C++ source file: ";
    string filePath;
    getline(cin, filePath); // Use getline to allow for spaces in file path


    // adds code as a string to be processed
    string input = readFileIntoString(filePath);

    // Print the processed input
    printProcessedInput(input);

    cout << "\n\n\n\n";

    vector<Token> tokens = tokenize(input);

    printTokensByCategory(tokens);

    return 0;
}
// END OF main




