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

class SyntaxAnalyzer { 
private:
// print statement:
    bool printswitch = true;
    void printstatement(const std::string& statement){
        if(printswitch){
            std::cout << statement << std::endl;
        }
    }
    
    std::vector<Token> tokens;
    size_t currentIndex = 0;

    TokenType stringToTokenType(const std::string& tokenType) {
        if (tokenType == "keyword") return TokenType::KEYWORD;
        if (tokenType == "identifier") return TokenType::IDENTIFIER;
        if (tokenType == "integer") return TokenType::INTEGER;
        if (tokenType == "real") return TokenType::REAL;
        if (tokenType == "operator") return TokenType::OPERATOR;
        if (tokenType == "separator") return TokenType::SEPARATOR;
        return TokenType::UNKNOWN;
    }
    
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
    void readFile(const string& input, const int& headerNumber) {
        ifstream file(input);
        string line;
        string tokenValue, tokenType;

        for(int i = 0; i < headerNumber; i++){
            getline(file, line);
        }

        while (getline(file, line)) {
            istringstream lineStream(line);
            lineStream >> tokenType >> tokenValue;
            tokens.push_back(Token(stringToTokenType(tokenType), tokenValue));
        }

        file.close();

        // Output the vector to the console for debugging
        // For testing, uncomment
        // for (const auto& token : tokens) {
        //     cout << "Token Type: " << static_cast<int>(token.type) << ", Token Value: " << token.value << endl;
        // }
    }

    Token lexer() {
        if (currentIndex < tokens.size()) {
            return tokens[currentIndex++];
        } else {
            // Handle the case where there are no more tokens
            return Token(TokenType::EMPTY, "");
        }
    }

    //the code below is not tested at all and is not going to work without changes
    void Rat25S() {
        // $$ <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List>$$
        Token token = lexer();
        if (check$$(token)) {
            Opt_Function_Definitions();
            token = lexer();
            if (check$$(token)) {
                Opt_Declaration_List();
                token = lexer();
                if (check$$(token)) {
                    Statement_List();
                    token = lexer();
                    if (check$$(token)) {
                        // End of parsing
                    } else {
                        cout << "Error: Expected '$$' at the end of Statement_List" << endl;
                    }
                } else {
                    cout << "Error: Expected '$$' at the end of Opt_Declaration_List" << endl;
                }
            } else {
                cout << "Error: Expected '$$' at the end of OPT_Function_Definitions" << endl;
            }
        } else {
            cout << "Error: No more tokens" << endl;
        }
    }

    void Opt_Function_Definitions(){
        // <Function Definitions> | <Empty>
        if(Empty()){
            return;
        }
        else{
            Function_Definitions();
        }
    }

    void Function_Definitions(){
        //<Function> <fd>
        Function();
        FD();
    }

    void FD(){
        //(ε | <Function Definitions>)
        if (!Empty()) {
            Function_Definitions();
        }
    }

    void Function(){
        // function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
        Token token = lexer();
        if (token.type == TokenType::KEYWORD && token.value == "function") {
            Identifier();
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                Opt_Parameter_List();
                token = lexer();
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    Opt_Declaration_List();
                    Body();
                } else {
                    cout << "Error: Expected ')' at the end of Function" << endl;
                }
            } else {
                cout << "Error: Expected '(' at the start of Function" << endl;
            }

        } else {
            cout << "Error: Expected 'function' at the start of Function" << endl;
        }
    }
    
    void Opt_Parameter_List(){
        // Parameter_List() | Empty()
        if(Empty()){
            return;
        }
        else{
            Parameter_List();
        }
    }

    void Parameter_List(){
        //<Parameter> <P>
        Parameter();
        P();
    }

    void P(){
        // (ε |  , <Parameter List>)
        Token token = lexer();
        if(token.type == TokenType::SEPARATOR && token.value == ","){
            Parameter_List();
        }
        else{
           currentIndex--;
        }
    }

    void Parameter(){
        //<IDs > <Qualifier>
        IDS();
        Qualifier();
    }

    void Qualifier(){
        // integer | boolean | real
        Token token = lexer();
        if(token.type == TokenType::KEYWORD && (token.value == "integer" || token.value == "real" || token.value == "boolean")){

        } else {
            cout << "Error: Invalid Keyword" << endl;
        }
    }

    void Body(){
        // { < Statement List> }
        Token token = lexer();
        if (token.type == TokenType::SEPARATOR && token.value == "{") {
            Statement_List();
            Token token = lexer();
            if (token.type == TokenType::SEPARATOR && token.value == "}") {
                // End of Body
            } else {
                cout << "Error: Expected '}' at the end of Body" << endl;
            }
        } else {
            cout << "Error: Expected '{' at the start of Body" << endl;
        }
    }

    void Opt_Declaration_List(){
        // Declaration_List() | Empty()
        // if(Empty()){
        //     return;
        // }
        // else{
        //     Declaration_List();
        // }
        Token token = lexer();
        if(token.type == TokenType::KEYWORD){
            currentIndex--;
            Declaration_List();
        }
        else{
            currentIndex--;
        }
    }

    void Declaration_List(){
        Declaration();
        Token token = lexer();
        if(token.type == TokenType::SEPARATOR && token.value == ";"){
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                D();
                token = lexer();
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    // End of Declaration_List
                } else {
                    cout << "Error: Expected ')' at the end of Declaration_List" << endl;
                }

            } else {
                cout << "Error: Expected ';' at the end of Declaration_List" << endl;
            }
        } else {
            cout << "Error: Expected ';' at the end of Declaration_List" << endl;
        }
    }

    void D(){
        // (ε | <Declaration List>)
        if (!Empty()) {
            Declaration_List();
        }
    }

    void Declaration(){
        // <Qualifier > <IDs>
        Qualifier();
        IDS();
    }

    void IDS(){
        // <Identifier> <id>
        Identifier();
        id();
    }

    void id(){
        //  (ε | , <IDs>)
        Token token = lexer();
        if(token.type == TokenType::SEPARATOR && token.value == ","){
            IDS();
        }
        else{
            currentIndex--;
        }
    }

    void Identifier(){
        Token token = lexer();
        if(token.type == TokenType::IDENTIFIER){
            // Valid Identifier
        } else {
            cout << "Error: Invalid Identifier" << endl;
        }
    }

    void Statement_List(){
        //<Statement><S>
        Statement();
        S();
    }

    void S(){
        //  (ε | <Statement List>)
        if (!Empty()) {
            Statement_List();
        }
    }

    void Statement(){
        // <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
        Token token = lexer();

        // <Compound> ::= { <Statement List> }
        // <Assign> ::= <Identifier> = <Expression> ;
        // <If> ::= if ( <Condition> )  <Statement> <if>  <Statement> endif
        // <Return> ::= return <r>
        // <Print> ::= print ( <Expression>)
        // <Scan> ::= scan ( <IDs> );
        // <While> ::= while ( <Condition> ) <Statement> endwhile
        if(token.type == TokenType::SEPARATOR && token.value == "{"){
            Statement_List();
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == "}"){

            }
            else{
                cout << "Error in Compound" << endl;
            }

        }
        else if(token.type == TokenType::KEYWORD && token.value == "if"){
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                Condition();
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    Statement();
                    _if();
                    Statement();
                    if(token.type == TokenType::KEYWORD && token.value == "endif"){

                    }
                    else{
                        cout << "Error" << endl;
                    }
                }
                else{
                    cout << "Error" << endl;
                }
            }
            else{
                cout << "Error" << endl;
            }
        }
        else if(token.type == TokenType::KEYWORD && token.value == "return"){
            r();
        }
        else if(token.type == TokenType::KEYWORD && token.value == "print"){
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                Expression();
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                  
                }
                else{
                    cout << "Error in Return" << endl;
                }
            }
            else{
                cout << "Error in Return" << endl;
            }
        }
        else if(token.type == TokenType::KEYWORD && token.value == "scan"){
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                IDS();
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                  
                }
                else{
                    cout << "Error in Return" << endl;
                }
            }
            else{
                cout << "Error in Return" << endl;
            }
        }
        else if(token.type == TokenType::KEYWORD && token.value == "while"){
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == "("){
                Condition();
                if(token.type == TokenType::SEPARATOR && token.value == ")"){
                  Statement();
                  if(token.type == TokenType::KEYWORD && token.value == "endwhile"){

                  }
                  else{
                    cout << "Error in Return" << endl;
                  }
                }
                else{
                    cout << "Error in Return" << endl;
                }
            }
            else{
                cout << "Error in Return" << endl;
            }
        }
        else if(token.type == TokenType::KEYWORD){
            token = lexer();
                if(token.type == TokenType::SEPARATOR && token.value == "="){
                    Expression();
                    if(token.type == TokenType::SEPARATOR && token.value == ";"){

                    }
                    else{
                        cout << "Error in Assign" << endl;
                    }
                }
                else{
                    cout << "Error in Assign" << endl;
                }
        }
        else{
            cout << "Error: Invalid Statement" << endl;
        }

    }

    void _if(){
        //( endif | else )
        Token token = lexer();
        if(token.type == TokenType::SEPARATOR && token.value == "("){
                token = lexer();
                if(token.type == TokenType::KEYWORD && (token.value == "if" || token.value == "endif")){
                    token = lexer();
                    if(token.type == TokenType::SEPARATOR && token.value == ")"){
                    }
                    else{
                        cout << "Error" << endl;
                    }
                }
                else{
                    cout << "Error" << endl;
                }
        }
        else{
            cout << "Error" << endl;
        }
    }

    void r(){
        //  (; | <Expression> ;)
        Token token = lexer();
        if(token.type == TokenType::SEPARATOR && token.value == ";"){

        }
        else{
            currentIndex--;
            Expression();
            token = lexer();
            if(token.type == TokenType::SEPARATOR && token.value == ";"){

            }
            else{
                cout << "Error" << endl;
            }
            
        }

    }

    void Condition(){
        //<Expression> <Relop> <Expression>
        Expression();
        Relop();
        Expression();
    }

    void Relop(){
        // == | != | > | < | <= | =>
        Token token = lexer();
        if(token.type == TokenType::OPERATOR && (token.value == "==" || token.value == "!=" || token.value == ">" 
            || token.value == "<" || token.value == "<=" || token.value == "=>" )){
                
        }
        else{
            cout << "Error" << endl;
        }
    }

    void Expression(){
        //<Term> <E>
        Term();
        E();
    }

    void E() {
        //  + <Term> <E> | - <Term><E> | ɛ

        Token token = lexer();
        if (token.type == TokenType::OPERATOR && (token.value == "+" || token.value == "-")) {
            Term();
            E();
        } else {
            currentIndex--; // Move back one token
        }

    }

    void T(){
        // * <Factor> <T> | / <Factor> <T> | ɛ
        Token token = lexer();
        if (token.type == TokenType::OPERATOR && (token.value == "*" || token.value == "/")) {
            Factor();
            T();
        } else {
            currentIndex--; // Move back one token
        }
    }

    void Term(){
        // <Factor> <T>
        Factor();
        T();
    }
    
    void Factor() {
        // - <Primary> | <Primary>
        Token token = lexer();
        if (token.type == TokenType::OPERATOR && token.value == "-") {
            Primary();
        } else {
            currentIndex--; // Move back one token
            Primary();
        }
    }

    void Primary() {
        Token token = lexer();
        if (token.type == TokenType::INTEGER || token.type == TokenType::REAL || token.type == TokenType::IDENTIFIER) {
            // Valid primary
        } else {
            currentIndex--;
        }
    }

};

int main(){
    SyntaxAnalyzer analyzer;

    analyzer.readFile("SA_input_1.txt", 3); // Adjust the parameters as needed

    analyzer.Rat25S();

    // check whether or not the parse was completed (whether or not if the syntax is correct)
    // try {
    //     analyzer.Rat25s();
    //     cout << "parse complete = correct syntax" << endl;
    // } catch (const expection& e) {
    //     cerr << "error: failed parse" << e.what() << endl;
    // }

    return 0;
}