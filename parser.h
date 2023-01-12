/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 * 
 * Submitted By  - Rithik Kumar Goyal
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <map>
#include <set>
// #include "reg.h"
using namespace std;

string inputTxt; // input text after the #, that needs to be parsed.

struct REG_node 
  {
      struct REG_node * first_neighbor;
      char first_label;
      struct REG_node * second_neighbor;
      char second_label;
      
      REG_node(struct REG_node * first_neighbor, char first_label, struct REG_node * second_neighbor, char second_label)
      {
        this->first_label = first_label;
        this->first_neighbor = first_neighbor;
        this->second_label = second_label;;
        this->second_neighbor = second_neighbor;
      }
  };

struct REG 
  {
      struct REG_node * start;
      struct REG_node * accept;
      REG(struct REG_node * start,struct REG_node * accept)
      {
        this->start = start;
        this->accept = accept;
      }
  };

REG* make_CHAR_reg(char x)
{
    REG_node *accept_node = new REG_node(NULL, '\0', NULL, '\0');
    REG_node * start_node = new REG_node(accept_node, x, NULL, '\0');
    REG * reg = new REG(start_node, accept_node);
    
    return reg; 
}

REG* make_DOT_reg(REG* first, REG* second)
{
    REG_node *accept_node = new REG_node(NULL, '\0', NULL, '\0');
    REG_node * start_node = new REG_node(first->start, '_', NULL, '\0');

    first->accept->first_neighbor = second->start;
    first->accept->first_label = '_';

    // second->start->first_neighbor = second->accept;
    second->accept->first_neighbor = accept_node;
    second->accept->first_label = '_';

    REG * reg = new REG(start_node, accept_node);
    
    return reg; 
}

REG* make_OR_reg(REG* first, REG* second)
{
    REG_node *accept_node = new REG_node(NULL, '\0', NULL, '\0');
    REG_node * start_node = new REG_node(first->start, '_', second->start, '_');
    REG * reg = new REG(start_node, accept_node);

    first->accept->first_neighbor = accept_node;
    first->accept->first_label = '_';

    second->accept->first_neighbor = accept_node;
    second->accept->first_label = '_';
    
    return reg;
}

REG* make_STAR_reg(REG* first)
{
    REG_node *accept_node = new REG_node(NULL, '\0', NULL, '\0');
    REG_node * start_node = new REG_node(first->start, '_', accept_node, '_');
    
    first->accept->first_neighbor = accept_node;
    first->accept->first_label = '_';

    first->accept->second_neighbor = first->start;
    first->accept->second_label = '_';


    REG * reg = new REG(start_node, accept_node);
    
    return reg;
}


class myLexicalAnalyser 
{
    public:
    set<REG_node*> match_one_char(set<REG_node*> s, char c);
    bool epsilonError (REG* n );
    int match(REG* r, string s, int p);
    int my_get_token(int pos);
};

vector<pair<Token, REG*>> token_and_REG; // it contains all the tokens and their respective REGs
class Parser {
  public:
    // REG* make_CHAR_reg(char a);
    // map<Token, int> hm; //token, folowed by line number
    map<string, int> hm;
    void parse_Input();
    void parse_input();
    void readAndPrintAllInput();
  private:
    
    LexicalAnalyzer lexer;
    void syntax_error_expr(Token t);
    void syntax_error();
    Token expect_expr(TokenType expected_type, Token t);
    Token expect(TokenType expected_type);
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    struct REG* parse_expr(Token t);

    
};

#endif

