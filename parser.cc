/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 * Submitted by - Rithik Kumar Goyal
 */
#include <iostream>
#include <cstdlib>
#include<set>
#include<algorithm>


#include "parser.h"
using namespace std;


bool semantics = false; //initially there are no semantics error
vector<string> sem; // vector of all semantic errors

set<REG_node*> myLexicalAnalyser::match_one_char(set<REG_node*> s, char c) //returns set of nodes which are reachable from a given sets of nodes with consuming a char a
{
    set<REG_node *> set1;
    for(REG_node* i: s)
    {
        if(i->first_label == c )
        {
            set1.insert(i->first_neighbor);
        }
        if(i->second_label == c )
        {
            set1.insert(i->second_neighbor);
        }
    }
    if(set1.empty())
    {
        return set1;
    }
    //set1 is not empty
    bool changed = true;
    set<REG_node *> set2;
    while(changed)
    {
        changed = false;
        for(REG_node* i: set1)
        {
            set2.insert(i);
            if(i->first_neighbor!=NULL && i->first_label == '_' )
            {
                set2.insert(i->first_neighbor);
            }
            if(i->second_neighbor!=NULL && i->second_label == '_')
            {
                set2.insert(i->second_neighbor);
            }
        }
        if(set1!=set2)
        {
            changed = true;
            set1 = set2;
            set2.clear();
        }
    }
    return set1;
} 

int myLexicalAnalyser::match(REG* r, string inputTxt, int pos)
{
    int n = inputTxt.length(); // length of the string
    int it = 0; // 
    int maxx = 0; // maximum length it can match and can reach the accept node of the REG r
    set<REG_node*> set1;
    set1.insert(r->start);
    set1 = myLexicalAnalyser::match_one_char(set1, '_');
    set1.insert(r->start);

    while(it+pos<n)
    {
        set1 = myLexicalAnalyser::match_one_char(set1, inputTxt[it+pos]);
        if(set1.empty()==true)
        {
            break; // there is no point in iterating further because it cannot consume that char, so we break out of the loop
        }

        it++;
        if(set1.find(r->accept)!=set1.end())
        {
            maxx = it; // updating the maxx
        }
    }
    return maxx;
}

int myLexicalAnalyser::my_get_token(int pos) // to get the token, lexeme and length of the lexeme and update the position value. 
{
    Token t; // for token having the max length
    int tempMax = 0; // length of the longest matching
    int iter = pos;
    for(auto x:token_and_REG)
    {
        int a =  myLexicalAnalyser::match(x.second, inputTxt, iter);
        if(a > tempMax)
        {
            t = x.first;
            tempMax = a;
        }
    }
    if(tempMax == 0) // basically no tokens were able to match anything
    {
        cout<<"ERROR"<<endl;
        exit(1); //code is exited
    }
    else
    {
        cout<<t.lexeme<<" , "<<" \""<< inputTxt.substr(pos, tempMax)<<"\" "<<endl;
    }
    pos = pos + tempMax;
    return pos;
}

bool myLexicalAnalyser::epsilonError (REG* n )
{
    set<REG_node*> s1;
    s1.insert(n->start);

    set<REG_node*> s =  match_one_char(s1, '_');

    if(s.find(n->accept)!=s.end())
    {
        return true;
    }
    return false;
}

void Parser::syntax_error_expr(Token t)
{
    cout << "SYNTAX ERROR IN EXPRESSION OF " << t.lexeme << endl;
    exit(1);
}

void Parser::syntax_error()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect_expr(TokenType expected_type, Token tn)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error_expr(tn);
    return t;
}


Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

REG* Parser::parse_expr(Token tn)
{

    Token t = lexer.peek(1);
    if(t.token_type == CHAR)
    {
        Token tx = expect_expr(CHAR, tn);
        
        return make_CHAR_reg(tx.lexeme[0]);
        //where to use this c pointer of REG ?  
    }
    else if(t.token_type == LPAREN)
    {
        expect_expr(LPAREN, tn);
        REG* reg1 = parse_expr(tn);
        expect_expr(RPAREN, tn);

        Token t2 = lexer.peek(1);

        if(t2.token_type == DOT)
        {
            expect_expr(DOT, tn);
            expect_expr(LPAREN, tn);
            REG* d = parse_expr(tn);

            expect_expr(RPAREN, tn);

            return make_DOT_reg(reg1, d);
        }
        else if(t2.token_type == OR)
        {
            expect_expr(OR, tn);
            expect_expr(LPAREN, tn);
            REG* u = parse_expr(tn);

            expect_expr(RPAREN, tn);

            return make_OR_reg(reg1, u);
        }
        else if(t2.token_type == STAR)
        {
            expect_expr(STAR, tn);
            return make_STAR_reg(reg1);
        }
        else
        {
            syntax_error_expr(tn);
        }
    }
    else if(t.token_type == UNDERSCORE)
    {
        expect_expr(UNDERSCORE, tn);
        return make_CHAR_reg('_');

    }
    else
    {
        syntax_error_expr(tn);
    }

    return NULL;
}

void Parser::parse_token()
{
    Token t = expect(ID);
    // string s = "Line ";
    if(hm.find(t.lexeme)!=hm.end())
    {
        semantics = true;
        string s = "Line "+to_string(t.line_no) + ": "+t.lexeme+" already declared on line "+to_string(hm[t.lexeme]);
        sem.push_back(s);
        // cout<<"Line "<< t.line_no<<": "<<t.lexeme<<" already declared on line " <<hm[t.lexeme]<<endl;
    }
    else
    {
        hm.insert({t.lexeme, t.line_no});
    }
    REG* n = parse_expr(t);
    token_and_REG.push_back({t, n});

    // epsilonCheck(token_and_REG);
}

void Parser::parse_token_list()
{
    parse_token(); 

    Token t = lexer.peek(1);
    if(t.token_type==COMMA)
    {
        expect(COMMA);
        parse_token_list();
    }
    else if(t.token_type==HASH)
    {
        return;
    }
    else
    {
        syntax_error();
    }
}

void Parser::parse_tokens_section()
{
    parse_token_list();
    expect(HASH);    
}

void Parser::parse_input() // saves the input text that needs to be parsed
{
    parse_tokens_section();
    Token t = expect(INPUT_TEXT);

    inputTxt = t.lexeme; //have declared inputTxt to be a global variable so that it can be used later
}

void Parser::parse_Input()
{
    parse_input();
    expect(END_OF_FILE);    
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does

void Parser::readAndPrintAllInput()
{
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE) 
    {
        t.Print();         	// pringt token
        t = lexer.GetToken();	// and get another one
    }
        
    // note that you should use END_OF_FILE and not EOF
}

int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object. 
    // You can access the lexer object in the parser functions as shown in 
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will 
    // not work correctly
    Parser parser;
    parser.parse_Input();
    // parser.readAndPrintAllInput();
    if(semantics == true) // print out the semantics and exit
    {
        for(auto x:sem)
        {
            cout<<x<<endl;
        }
        exit(1);
    }
    myLexicalAnalyser *t = new myLexicalAnalyser();
    vector<Token> epsErr; // vector that has all the tokens which generate eps Eroor
    bool changed = false; // if true then there is eps error and will exit the function
    for(int i= 0; i<token_and_REG.size(); i++)
    {
        if(t->epsilonError(token_and_REG[i].second))
        {
            epsErr.push_back(token_and_REG[i].first);
            changed = true;
        }
    }

    if(changed == true) // print the eps error and exit
    {
        cout<<"EPSILON IS NOOOOOOOT A TOKEN !!! ";
        for(int i=0; i<epsErr.size(); i++)
        {
            cout<<epsErr[i].lexeme<<" ";
        }
        cout<<endl;
        exit(1);
    }
    
	int n = inputTxt.length();
    int p=0;
    while(p<n)
    {
        if(inputTxt[p] == ' ' || inputTxt[p] == '\"')
        {
            p++; //we hit a empty character, we increment till there is no non-empty charcater
            continue; //continue from the next iteration
        }
        p = t->my_get_token(p); // we parse through  our inputTxt and get the tokens, //this also returns a val which is used to update the value of position in the string
    }



}
