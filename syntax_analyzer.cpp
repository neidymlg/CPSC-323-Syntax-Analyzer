#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER,
    REAL,
    OPERATOR,
    SEPARATOR,
    UNKNOWN, 
    EMPTY
};

struct Token {
    TokenType type;
    std::string value;
    Token(TokenType type, string value) : type(type), value(value) {}
};

// Redirects the output to a file
// changes cout to a file stream
class OutputRedirector {
private:
    std::ofstream file;
    std::streambuf* original_buf;
public:
    explicit OutputRedirector(const string& filename) {
        original_buf = cout.rdbuf();
        file.open(filename);
        if (file.is_open()) {
            cout.rdbuf(file.rdbuf());
        }
    }

    ~OutputRedirector() {
        if (file.is_open()) {
            file.close();
            cout.rdbuf(original_buf);
        }
    }
};

class SyntaxAnalyzer { 
private:

    vector<Token> tokens;
    size_t currentIndex = 0;

    // Error handling function
    // Prints the error message and exits the program
    void error(const string& message) {
        cout << message;
        exit(1);
    }

    // Converts string to TokenType
    TokenType stringToTokenType(const string& tokenType) {
        if (tokenType == "keyword") return TokenType::KEYWORD;
        if (tokenType == "identifier") return TokenType::IDENTIFIER;
        if (tokenType == "integer") return TokenType::INTEGER;
        if (tokenType == "real") return TokenType::REAL;
        if (tokenType == "operator") return TokenType::OPERATOR;
        if (tokenType == "separator") return TokenType::SEPARATOR;
        return TokenType::UNKNOWN;
    }
    
    // Converts TokenType to string for printing
    string tokenTypeToString(TokenType type) {
        switch (type) {
            case TokenType::KEYWORD: return "KEYWORD";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::INTEGER: return "INTEGER";
            case TokenType::REAL: return "REAL";
            case TokenType::OPERATOR: return "OPERATOR";
            case TokenType::SEPARATOR: return "SEPARATOR";
            case TokenType::UNKNOWN: return "UNKNOWN";
            case TokenType::EMPTY: return "EMPTY";
            default: return "UNKNOWN";
        }
    }

    Token lexer(bool print = false) {
        // Check if there are more tokens to read
        if (currentIndex < tokens.size()) {
            Token token = tokens[currentIndex++];

            //will print the token type and value if the print is true
            if(print){
                cout << "================================================================================" << endl;
                cout << "\t\t\tToken:" << tokenTypeToString(token.type) << "\tLexeme:" << token.value << endl;
                cout << "================================================================================" << endl;
            }

            return token;
        } else {
            // Handle the case where there are no more tokens
            return Token(TokenType::EMPTY, "");
        }
    }

    //returns true if the token is $$
    bool check$$(Token token){
        if (token.type == TokenType::SEPARATOR && token.value == "$$") {
            return true;
        }
        else{
            return false;
        }
    }

    bool Empty(){
        Token token = lexer();
        if (check$$(token)) {
            currentIndex--;
            return true;
        }
        else if (token.type == TokenType::EMPTY){
            return true;
        }
        else{
            currentIndex--;
            return false;
        }
    }

public: 

    //reads all values from the file
    void readFile(const string& input, const int& headerNumber) {
        ifstream file(input);
        string line;
        string tokenValue, tokenType;

        //skips the header information
        for(int i = 0; i < headerNumber; i++){
            getline(file, line);
        }

        //gets token type and token value from each line
        while (getline(file, line)) {
            istringstream lineStream(line);
            lineStream >> tokenType >> tokenValue;
            tokens.push_back(Token(stringToTokenType(tokenType), tokenValue));
        }

        file.close();
    }

    void Rat25S() {
        // $$ <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List>$$
        Token token = lexer(true);
        if (check$$(token)) {
            cout << "<Rat25S> -> $$ <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List>$$" << endl;
            cout << "<Rat25S> -> $$ <Opt Function Definitions>" << endl;
            Opt_Function_Definitions();
            token = lexer(true);
            if (check$$(token)) {
                cout << "$$ <Opt Declaration List>" << endl;
                Opt_Declaration_List();
                token = lexer(true);
                if (check$$(token)) {
                    cout << "$$ <Statement List>" << endl;
                    Statement_List();
                    token = lexer(true);
                    if (check$$(token)) {
                        cout << "$$" << endl;
                        cout << "Parse complete: Correct syntax" << endl;
                    } else {
                        error("Error: Expected '$$' at the end of Statement_List");
                    }
                } else {
                    error("Error: Expected '$$' at the end of Opt_Declaration_List");
                }
            } else {
                error("Error: Expected '$$' at the end of Opt_Function_Definitions");
            }
        } else {
            error("Error: Expected '$$' at the start of Opt_Function_Definitions");
        }
    }

    void Opt_Function_Definitions(){
        // <Function Definitions> | <Empty>
        if(!Empty()){
            cout << "<Opt Function Definitions> -> <Function Definitions>" << endl;
             Function_Definitions();
        }
        else{
            cout << "<Opt Function Definitions> -> <Empty>" << endl;
        }
    }

    void Function_Definitions(){
        //<Function> <fd>
        cout << "<Function Definitions> -> <Function> <FD>" << endl;
        Function();
        FD();
    }

    void FD(){
        //(ε | <Function Definitions>)
        if (!Empty()) {
            cout << "<FD> -> <Function Definitions>" << endl;
            Function_Definitions();
        }
        else{
            cout << "<FD> -> ε" << endl;
        }
    }

    void Function(){
        // function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
        Token token = lexer(true);
        if (token.type == TokenType::KEYWORD && token.value == "function") {
            cout << "<Function> -> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>" << endl;
            cout << "<Function> -> function <Identifier>" << endl;   
            Identifier();
            token = lexer(true);
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                cout << "( <Opt Parameter List>" << endl;   
                Opt_Parameter_List();
                token = lexer(true);
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    cout << ") <Opt Declaration List>" << endl;
                    Opt_Declaration_List();
                    cout << "<Body>" << endl;
                    Body();
                    cout << "End of Function" << endl;
                } else {
                    error("Error: Expected ')' at the end of Function");
                }
            } else {
                error("Error: Expected '(' at the start of Function");
            }

        } else {
            error("Error: Expected 'function' at the start of Function");
        }
    }

    void Opt_Parameter_List(){
        // Parameter_List() | Empty()
        Token token = lexer();
        currentIndex--;
        if(token.type != TokenType::SEPARATOR && (token.value != ")" || token.value != "$$")){
            cout << "<Opt Parameter List> -> <Parameter List>" << endl;
            Parameter_List();
        }
        else{
            cout << "<Opt Parameter List> -> <Empty>" << endl;
        }
    }

    void Parameter_List(){
        //<Parameter> <P>
        cout << "<Parameter List> -> <Parameter> <P>" << endl;
        Parameter();
        P();
    }

    void P(){
        // (ε |  , <Parameter List>)
        Token token = lexer();
        currentIndex--;
        if(token.type == TokenType::SEPARATOR && token.value == ","){
            Token token = lexer(true);
            cout << "<P> -> , <Parameter List>" << endl;
            Parameter_List();
        }
        else{
            cout << "<P> -> ε" << endl;
        }
    }

    void Parameter(){
        //<IDs> <Qualifier>
        cout << "<Parameter> -> <IDs> <Qualifier>" << endl;
        IDS();
        Qualifier();
    }

    void Qualifier(){
        // integer | boolean | real
        Token token = lexer(true);
        if(token.type == TokenType::KEYWORD && (token.value == "integer" || token.value == "boolean" || token.value == "real")){
            cout << "<Qualifier> -> integer | boolean | real" << endl;
        }
        else {
            error("Error: Invalid Qualifier. Expected token type of integer, boolean, or real");
        }
    }

    void Body(){
        // { < Statement List> }
        Token token = lexer(true);
        if (token.type == TokenType::SEPARATOR && token.value == "{") {
            cout << "<Body> -> { <Statement List> }" << endl;
            cout << "<Body> -> { <Statement List>" << endl;
            Statement_List();
            Token token = lexer(true);
            if (token.type == TokenType::SEPARATOR && token.value == "}") {
                cout << "}" << endl;
                cout << "End of Body" << endl;
            } else {
                error("Error: Expected '}' at the end of Body");
            }
        } else {
            error("Error: Expected '{' at the start of Body");
        }
    }

    void Opt_Declaration_List(){
        // Declaration_List() | Empty()
        Token token = lexer();
        currentIndex--;
        if(token.type == TokenType::KEYWORD && (token.value == "integer" || token.value == "real" || token.value == "boolean")){
            cout << "<Opt Declaration List> -> <Declaration List>" << endl;
            Declaration_List();
        }
        else{
            cout << "<Opt Declaration List> -> <Empty>" << endl;
        }
    }

    void Declaration_List(){
        // <Declaration> ; <D>
        cout << "<Declaration List> -> <Declaration> ; <D>" << endl;
        Declaration();
        Token token = lexer(true);
        if(token.type == TokenType::SEPARATOR && token.value == ";"){
            cout << "; <D>" << endl;
            D();
        } else {
            error("Error: Expected ';' at the end of Declaration_List");
        }
    }

    void D(){
        // (ε | <Declaration List>)
        Token token = lexer();
        currentIndex--;
        if(token.type != TokenType::SEPARATOR && (token.value != "{" || token.value != "$$")){
            cout << "<D> -> <Declaration List>" << endl;
            Declaration_List();
        }
        else{
            cout << "<D> -> ε" << endl;
        }
    }

    void Declaration(){
        // <Qualifier > <IDs>
        cout << "<Declaration> -> <Qualifier> <IDs>" << endl;
        Qualifier();
        IDS();
    }

    void IDS(){
        cout << "<IDs> -> <Identifier> <id>" << endl;
        // <Identifier> <id>
        Identifier();
        id();
    }

    void id(){
        //  (ε | , <IDs>)
        Token token = lexer();
        currentIndex--;
        if(token.type == TokenType::SEPARATOR && token.value == ","){
            Token token = lexer(true);
            cout << "<id> -> , <IDs>" << endl;
            IDS();
        }
        else{
            cout << "<id> -> ε" << endl;
        }
    }

    void Identifier(){
        // <Identifier> ::= <IDENTIFIER>
        Token token = lexer(true);
        if(token.type == TokenType::IDENTIFIER) {
            cout << "<Identifier> -> Identifier" << endl;
        } else {
            error("Error: Invalid Identifier. Expected token type of IDENTIFIER");
        }
    }

    void Statement_List(){
        //<Statement><S>
        cout << "<Statement List> -> <Statement> <S>" << endl;
        Statement();
        S();
    }

    void S(){
        //  (ε | <Statement List>)
        Token token = lexer();
        currentIndex--;
        if(token.type != TokenType::SEPARATOR && (token.value != "}" || token.value != "$$")){
            cout << "<S> -> <Statement List>" << endl;
            Statement_List();
        }
        else{
            cout << "<S> -> ε" << endl;
        }
    }

    void Statement(){
        // <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
        Token token = lexer(true);
        cout << "<Statement> -> ";

        // <Compound> ::= { <Statement List> }
        // <Assign> ::= <Identifier> = <Expression> ;
        // <If> ::= if ( <Condition> )  <Statement> <if> 
        // <Return> ::= return <r>
        // <Print> ::= print ( <Expression>)
        // <Scan> ::= scan ( <IDs> );
        // <While> ::= while ( <Condition> ) <Statement> endwhile
        if(token.type == TokenType::SEPARATOR && token.value == "{"){
            cout << "<Compound>" << endl;
            cout << "<Compound> -> { <Statement List> }" << endl;
            cout << "<Compound> -> { <Statement List>" << endl;
            Statement_List();
            token = lexer(true);
            if(token.type == TokenType::SEPARATOR && token.value == "}"){
                cout << "}" << endl;
                cout << "End of Compound" << endl;
            }
            else{
                error("Error in beggining '}' for <Compound>'");
            }

        }
        else if(token.type == TokenType::KEYWORD && token.value == "if"){
            cout << "<If>" << endl;
            cout << "<If> -> if ( <Condition> ) <Statement> <if>" << endl;
            cout << "<If> -> if" << endl;
            token = lexer(true);
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                cout << "( <Condition>" << endl;
                Condition();
                token = lexer(true);
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    cout << ") <Statement> <if>" << endl;
                    Statement();
                    _if();
                }
                else{
                    error("Error in beginning ')' for <If>");
                }
            }
            else{
                error("Error in beginning '(' for <If>");
            }
        }
        else if(token.type == TokenType::KEYWORD && token.value == "return"){
            cout << "<Return>" << endl;
            cout << "<Return> -> return <r>" << endl;
            cout << "<Return> -> return" << endl;
            r();
        }
        else if(token.type == TokenType::KEYWORD && token.value == "print"){
            cout << "<Print>" << endl;
            cout << "<Print> -> print ( <Expression> )" << endl;
            cout << "<Print> -> print" << endl;
            token = lexer(true);
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                cout << "( <Expression>" << endl;
                Expression();
                token = lexer(true);
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    cout << ")" << endl;
                    cout << "End of Print" << endl;
                }
                else{
                    error("Error in beginning ')' for <Print>");
                }
            }
            else{
                error("Error in beginning '(' for <Print>");
            }
        }
        else if(token.type == TokenType::KEYWORD && token.value == "scan"){
            cout << "<Scan>" << endl;
            cout << "<Scan> -> scan ( <IDs> );" << endl;
            cout << "<Scan> -> scan" << endl;
            token = lexer(true);
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                cout << "( <IDs>" << endl;
                IDS();
                token = lexer(true);
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    cout << ")" << endl;
                    token = lexer(true);
                    if(token.type == TokenType::SEPARATOR && token.value == ";"){
                        cout << ";" << endl;
                        cout << "End of Scan" << endl;
                    }
                    else{
                        error("Error in ';' for <Scan>");
                    }
                }
                else{
                    error("Error in beginning ')' for <Scan>");
                }
            }
            else{
                error("Error in beginning '(' for <Scan>");
            }
        }
        else if(token.type == TokenType::KEYWORD && token.value == "while"){
            cout << "<While>" << endl;
            cout << "<While> -> while ( <Condition> ) <Statement> endwhile" << endl;
            cout << "<While> -> while" << endl;
            token = lexer(true);
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                cout << "( <Condition>" << endl;
                Condition();
                token = lexer(true);
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                  cout << ") <Statement>" << endl;
                  Statement();
                  token = lexer(true);
                  if(token.type == TokenType::KEYWORD && token.value == "endwhile"){
                    cout << "endwhile" << endl;
                  }
                  else{
                    error("Error in 'endwhile' for <While>");
                  }
                }
                else{
                    error("Error in beginning ')' for <While>");
                }
            }
            else{
                error("Error in beginning '(' for <While>");
            }
        }
        else if(token.type == TokenType::IDENTIFIER){
            cout << "<Assign>" << endl;
            cout << "<Assign> -> <Identifier> = <Expression> ;" << endl;
            cout << "<Assign> -> <Identifier>" << endl;
            Token token = lexer(true);
                if(token.type == TokenType::OPERATOR && token.value == "="){
                    cout << "= <Expression> ;" << endl;
                    Expression();
                    token = lexer(true);
                    if(token.type == TokenType::SEPARATOR && token.value == ";"){
                        cout << ";" << endl;
                        cout << "End of Assign" << endl;
                    }
                    else{
                        error("Error in ';' for <Assign>");
                    }
                }
                else{
                    error("Error in '=' for <Assign>");
                }
        }
        else{
            error("Error: Invalid Statement. Expected statement type of <Compound>, <Assign>, <If>, <Return>, <Print>, <Scan>, or <While>");
        }

    }

    void _if(){
        //endif | else <Statement> endif
        Token token = lexer(true);
        if(token.type == TokenType::KEYWORD && token.value == "endif"){
            cout << "<if> -> endif" << endl;
        }
        else if(token.type == TokenType::KEYWORD && token.value == "else"){
            cout << "<if> -> else <Statement> endif" << endl;
            Statement();
            token = lexer(true);
            if(token.type == TokenType::KEYWORD && token.value == "endif"){
                cout << "endif" << endl;
                cout << "End of <If>" << endl;
            }
            else{
                error("Error in 'endif' for <if>");
            }
        }
        else{
            error("Error, expected 'endif' or 'else' for <if>");
        }

    }

    void r(){
        //  (; | <Expression> ;)
        Token token = lexer();
        currentIndex--;
        if(token.type == TokenType::SEPARATOR && token.value == ";"){
            token = lexer(true);
            cout << "<r> -> ;" << endl;
        }
        else{
            cout << "<r> -> <Expression> ;" << endl;
            Expression();
            token = lexer(true);
            if(token.type == TokenType::SEPARATOR && token.value == ";"){
                cout << ";" << endl;
                cout << "End of <Return>" << endl;
            }
            else{
                error("Error in ';' for <Return>");
            }
            
        }

    }

    void Condition(){
        //<Expression> <Relop> <Expression>
        cout << "<Condition> -> <Expression> <Relop> <Expression>" << endl;
        Expression();
        Relop();
        Expression();
    }

    void Relop(){
        // == | != | > | < | <= | =>
        Token token = lexer(true);
        if(token.type == TokenType::OPERATOR && 
            (token.value == "==" || token.value == "!=" || token.value == ">" || token.value == "<" || token.value == "<=" || token.value == "=>")){
            cout << "<Relop> -> == | != | > | < | <= | =>" << endl;
        }
        else{
            error("Error in Relop. Expected token type of OPERATOR with value ==, !=, >, <, <=, or =>");
        }
    }

    void Expression(){
        //<Term> <E>
        cout << "<Expression> -> <Term> <E>" << endl;
        Term();
        E();
    }

    void E() {
        //  + <Term> <E> | - <Term><E> | ɛ
        Token token = lexer();
        currentIndex--;
        if(token.type == TokenType::OPERATOR &&
            (token.value == "+" || token.value == "-")){
            token = lexer(true);
            cout << "<E> -> + <Term> <E> | - <Term><E>" << endl;
            Term();
            E();
        }
        else{
            cout << "<E> -> ε" << endl;
        }

    }

    void T(){
        // * <Factor> <T> | / <Factor> <T> | ɛ
        Token token = lexer();
        currentIndex--;
        if(token.type == TokenType::OPERATOR &&
            (token.value == "*" || token.value == "/")){
            token = lexer(true);
            cout << "<T> -> * <Factor> <T> | / <Factor> <T>" << endl;
            Factor();
            T();
        } 
        else{
            cout << "<T> -> ε" << endl;
        }
    }

    void Term(){
        // <Factor> <T>
        cout << "<Term> -> <Factor> <T>" << endl;
        Factor();
        T();
    }
    
    void Factor() {
        // - <Primary> | <Primary>
        Token token = lexer();
        currentIndex--;
        if(token.type == TokenType::OPERATOR && token.value == "-"){
            token = lexer(true);
            cout << "<Factor> -> - <Primary>" << endl;
            Primary();
        } else {
            cout << "<Factor> -> <Primary>" << endl;
            Primary();
        }
    }

    void Primary() {
       // <Primary> ::= <INTEGER> | <REAL> | <IDENTIFIER> | true, false
        Token token = lexer(true);
        if (token.type == TokenType::INTEGER || token.type == TokenType::REAL || token.type == TokenType::IDENTIFIER || 
            (token.type == TokenType::KEYWORD && (token.value == "true" || token.value == "false"))) {
            cout << "<Primary> -> <INTEGER> | <REAL> | <IDENTIFIER> | true, false" << endl;
        } 
        else{
            error("Error in Primary. Expected token type of INTEGER, REAL, IDENTIFIER, or KEYWORD true, or false");
        }
    }

};

int main(){
    string inputFile;
    int headerNumber;
    string outputFile;

    cout << "Enter the input file name (SA_input_1.txt , SA_input_2.txt, SA_input_3.txt): ";
    cin >> inputFile;
    cout << "Enter the number of header lines to skip (3, 0, 0): ";
    cin >> headerNumber;

    cout << "Enter the output file name (SA_output_1.txt , SA_output_2.txt, SA_output_3.txt): ";
    cin >> outputFile;

    SyntaxAnalyzer analyzer;
    OutputRedirector redirect(outputFile); // Redirect output to the specified file

    analyzer.readFile(inputFile, headerNumber); // Read the input file

    analyzer.Rat25S(); // Start the parsing process

    return 0;
}