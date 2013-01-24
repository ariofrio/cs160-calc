#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <stack>

using namespace std;


/*** Enums and Print Functions for Terminals and Non-Terminals  **********************/

#define MAX_SYMBOL_NAME_SIZE 25

//all of the terminals in the language
typedef enum {
  T_eof = 0,   // 0: end of file
  T_num,       // 1: numbers
  T_plus,      // 2: +
  T_minus,     // 3: -
  T_times,     // 4: *
  T_div,       // 5: /
  T_lt,        // 6: <
  T_gt,        // 7: >
  T_eq,        // 8: =
  T_semicolon, // 9: ;
  T_openparen, // 10: (
  T_closeparen // 11: )
} token_type;

// This function returns a string for the token. It is used in the parsetree_t
// class to actually dump the parsetree to a dot file (which can then be turned
// into a picture).  Note that the return char* is a reference to a local copy
// and it needs to be duplicated if you are a going require multiple instances
// simultaneously.
//
// No need to do anything here
char* token_to_string(token_type c) {
  static char buffer[MAX_SYMBOL_NAME_SIZE];
  switch( c ) {
    case T_eof: strncpy(buffer,"eof",MAX_SYMBOL_NAME_SIZE); break;
    case T_num: strncpy(buffer,"num",MAX_SYMBOL_NAME_SIZE); break;
    case T_plus: strncpy(buffer,"+",MAX_SYMBOL_NAME_SIZE); break;
    case T_minus: strncpy(buffer,"-",MAX_SYMBOL_NAME_SIZE); break;
    case T_times: strncpy(buffer,"*",MAX_SYMBOL_NAME_SIZE); break;
    case T_div: strncpy(buffer,"/",MAX_SYMBOL_NAME_SIZE); break;
    case T_lt: strncpy(buffer,"<",MAX_SYMBOL_NAME_SIZE); break;
    case T_gt: strncpy(buffer,">",MAX_SYMBOL_NAME_SIZE); break;
    case T_eq: strncpy(buffer,"=",MAX_SYMBOL_NAME_SIZE); break;
    case T_semicolon: strncpy(buffer,";",MAX_SYMBOL_NAME_SIZE); break;
    case T_openparen: strncpy(buffer,"(",MAX_SYMBOL_NAME_SIZE); break;
    case T_closeparen: strncpy(buffer,")",MAX_SYMBOL_NAME_SIZE); break;
    default: strncpy(buffer,"unknown_token",MAX_SYMBOL_NAME_SIZE); break;
  }
  return buffer;
}

// All of the non-terminals in the grammar. You need to add these in
// according to your grammar. These are used for printing the thing out, so
// please follow the convention we set up so that we can tell what the hack
// you are doing when I grade.
typedef enum {
  epsilon = 100,
  NT_List,
  NT_ListP,
  NT_RelExpr,
  NT_RelExprP,
  NT_Expr,
  NT_ExprP,
  NT_Term,
  NT_TermP,
  NT_Factor
} nonterm_type;

// This function returns a string for the non-terminals. It is used in the
// parsetree_t class to actually dump the parsetree to a dot file (which can
// then be turned into a picture). Note that the return char* is a reference
// to a local copy and it needs to be duplicated if you are a going require
// multiple instances simultaneously.
//
// Add entries for all the non-terminals of your grammar following the same
// convention.
char* nonterm_to_string(nonterm_type nt)
{
  static char buffer[MAX_SYMBOL_NAME_SIZE];
  switch( nt ) {
    case epsilon: strncpy(buffer,"e",MAX_SYMBOL_NAME_SIZE); break;
    case NT_List: strncpy(buffer,"List",MAX_SYMBOL_NAME_SIZE); break;
    case NT_ListP: strncpy(buffer,"ListP",MAX_SYMBOL_NAME_SIZE); break;
    case NT_RelExpr: strncpy(buffer,"RelExpr",MAX_SYMBOL_NAME_SIZE); break;
    case NT_RelExprP: strncpy(buffer,"RelExprP",MAX_SYMBOL_NAME_SIZE); break;
    case NT_Expr: strncpy(buffer,"Expr",MAX_SYMBOL_NAME_SIZE); break;
    case NT_ExprP: strncpy(buffer,"ExprP",MAX_SYMBOL_NAME_SIZE); break;
    case NT_Term: strncpy(buffer,"Term",MAX_SYMBOL_NAME_SIZE); break;
    case NT_TermP: strncpy(buffer,"TermP",MAX_SYMBOL_NAME_SIZE); break;
    case NT_Factor: strncpy(buffer,"Factor",MAX_SYMBOL_NAME_SIZE); break;
    default: strncpy(buffer,"unknown_nonterm",MAX_SYMBOL_NAME_SIZE); break;
  }
  return buffer;
}

/*** Scanner Class ***********************************************/

class scanner_t {
  public:

    // eats the next token and prints an error if it is not of type c
    void eat_token(token_type c);

    // peeks at the lookahead token
    token_type next_token();

    // return line number for errors
    int get_line();

    // constructor - inits g_next_token
    scanner_t();

  private:

    bool peek_token_available = false;
    token_type peek_token;

    int line_no = 1;

    void scan_error(char x);
    void mismatch_error(token_type c); // error message and exit for mismatch

};

token_type scanner_t::next_token() {
  if(peek_token_available) return peek_token;

  int c = cin.get();
  switch(c) {
    case '\n': line_no++; return next_token(); break;
    case ' ' : return next_token(); break;
    case '\t': return next_token(); break;

    case EOF: peek_token = T_eof; break;
    case '+': peek_token = T_plus; break;
    case '-': peek_token = T_minus; break;
    case '*': peek_token = T_times; break;
    case '/': peek_token = T_div; break;
    case '<': peek_token = T_lt; break;
    case '>': peek_token = T_gt; break;
    case '=': peek_token = T_eq; break;
    case ';': peek_token = T_semicolon; break;
    case '(': peek_token = T_openparen; break;
    case ')': peek_token = T_closeparen; break;

    default:
      // long won't overflow when checking if number fits in 32 bits
      long number = c - '0';
      long coefficient = 10;
      if(c >= '0' && c <= '9') {
        peek_token = T_num;
        while(true) {
          int c = cin.get();
          if(!(c >= '0' && c <= '9')) break;

          number += coefficient * (c - '0');
          coefficient *= 10;

          if(number > 2147483647) scan_error(c);
        }
        cin.unget(); // put back unused character
      } else {
        scan_error(c);
      }
      break;
  }
  peek_token_available = true;
  return peek_token;
}

void scanner_t::eat_token(token_type c)
{
  // if we are supposed to eat token c, and it does not match
  // what we are supposed to be reading from file, then it is a
  // mismatch error ( call - mismatch_error(c) )
  if(c != next_token()) mismatch_error(c);

  peek_token_available = false;
}

scanner_t::scanner_t() {}

int scanner_t::get_line()
{
  return line_no;
}

void scanner_t::scan_error (char x)
{
  printf("scan error: unrecognized character '%c' -line %d\n",x, get_line());
  exit(1);

}

void scanner_t::mismatch_error (token_type x)
{
  printf("syntax error: found %s ",token_to_string(next_token()) );
  printf("expecting %s - line %d\n", token_to_string(x), get_line());
  exit(1);
}


/*** ParseTree Class **********************************************/

// This class is used to dump the parsed tree as a dot file. As you parse,
// you should dump the results here and not worry about it. This class is
// complete and you should not have to touch anything (if everything goes
// according to plan).
//
// While you don't have to modify it, you will have to call it from your
// recursive decent parser so read about the interface below.
class parsetree_t {
  public:
    void push(token_type t);
    void push(nonterm_type nt);
    void pop();
    void drawepsilon();
    parsetree_t();
  private:
    enum stype_t{
      TERMINAL=1,
      NONTERMINAL=0,
      UNDEF=-1
    };

    struct stuple {
      nonterm_type nt;
      token_type t;
      stype_t stype;
      int uniq;
    };
    void printedge(stuple temp); //prints edge from TOS->temp
    stack<stuple> stuple_stack;
    char* stuple_to_string(const stuple& s);
    int counter;
};


// the constructer just starts by initializing a counter (used to uniquely
// name all the parse tree nodes) and by printing out the necessary dot commands
parsetree_t::parsetree_t()
{
  counter = 0;
  printf("digraph G { page=\"8.5,11\"; size=\"7.5, 10\"\n");
}

// This push function taken a non terminal and keeps it on the parsetree
// stack. The stack keeps trace of where we are in the parse tree as
// we walk it in a depth first way. You should call push when you start
// expanding a symbol, and call pop when you are done. The parsetree_t
// will keep track of everything, and draw the parse tree as you go.
//
// This particular function should be called if you are pushing a non-terminal
void parsetree_t::push(nonterm_type nt)
{
  counter ++;
  stuple temp;
  temp.nt = nt;
  temp.stype = NONTERMINAL;
  temp.uniq = counter;
  printedge( temp );
  stuple_stack.push( temp );
}

//same as above, but for terminals
void parsetree_t::push(token_type t)
{
  counter ++;
  stuple temp;
  temp.t = t;
  temp.stype = TERMINAL;
  temp.uniq = counter;
  printedge( temp );
  stuple_stack.push( temp );
}

// When you are finished parsing a symbol, pop it. That way the parsetree_t
// will know that you are now working on a higher part of the tree.
void parsetree_t::pop()
{
  if ( !stuple_stack.empty() ) {
    stuple_stack.pop();
  }

  if ( stuple_stack.empty() ) {
    printf( "};\n" );
  }
}

// Draw an epsilon on the parse tree hanging off of the top of stack
void parsetree_t::drawepsilon()
{
  push(epsilon);
  pop();
}

// This private print function is called from push. Basically it
// just prints out the command to draw an edge from the top of the stack (TOS)
// to the new symbol that was just pushed. If it happens to be a terminal
// then it makes it a snazzy blue color so you can see your program on the leaves
void parsetree_t::printedge(stuple temp)
{
  if ( temp.stype == TERMINAL ) {
    printf("\t\"%s%d\" [label=\"%s\",style=filled,fillcolor=powderblue]\n",
        stuple_to_string(temp),
        temp.uniq,
        stuple_to_string(temp));
  } else {
    printf("\t\"%s%d\" [label=\"%s\"]\n",
        stuple_to_string(temp),
        temp.uniq,
        stuple_to_string(temp));
  }

  //no edge to print if this is the first node
  if ( !stuple_stack.empty() ) {
    //print the edge
    printf( "\t\"%s%d\" ", stuple_to_string(stuple_stack.top()), stuple_stack.top().uniq );
    printf( "-> \"%s%d\"\n", stuple_to_string(temp), temp.uniq );
  }
}

// just a private utility for helping with the printing of the dot stuff
char* parsetree_t::stuple_to_string(const stuple& s)
{
  static char buffer[MAX_SYMBOL_NAME_SIZE];
  if ( s.stype == TERMINAL ) {
    snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", token_to_string(s.t) );
  } else if ( s.stype == NONTERMINAL ) {
    snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", nonterm_to_string(s.nt) );
  } else {
    assert(0);
  }

  return buffer;
}


/*** Parser Class ***********************************************/

// Now on to the big stuff!
//
// The parser_t class handles everything. It has an instance of scanner_t
// so it can peek at tokens and eat them up. It also has access to the
// parsetree_t class so it can print out the parse tree as it figures it out.
// To make the recursive decent parser work, you will have to add some
// methods to this class. The helper functions are described below

class parser_t {
  private:
    scanner_t scanner;
    parsetree_t parsetree;
    void eat_token(token_type t);
    void syntax_error(nonterm_type);
    void div_by_zero_error();

    void List();
    void ListP();
    void RelExpr();
    void RelExprP();
    void Expr();
    void ExprP();
    void Term();
    void TermP();
    void Factor();

  public:
    void parse();
};


// This function not only eats the token (moving the scanner forward one
// token), it also makes sure that token is drawn in the parse tree
// properly by calling push and pop.
void parser_t::eat_token(token_type t)
{
  parsetree.push(t);
  scanner.eat_token(t);
  parsetree.pop();
}

// Call this syntax error when you are trying to parse the
// non-terminal nt, but you fail to find a token that you need
// to make progress.  You should call this as soon as you can know
// there is a syntax_error.
void parser_t::syntax_error(nonterm_type nt)
{
  printf("syntax error: found %s in parsing %s - line %d\n",
      token_to_string( scanner.next_token()),
      nonterm_to_string(nt),
      scanner.get_line() );
  exit(1);
}

// throw a div by zero error, call this function when you detect
// division by zero (you would use this if you are evaluating the
// expressions for extra credit)
void parser_t::div_by_zero_error()
{
  printf("div by zero error: line %d\n", scanner.get_line() );
  exit(0);
}


// Once the recursive decent parser is set up, you simply call parse()
// to parse the entire input, all of which can be derived from the start
// symbol
void parser_t::parse()
{
  List();
}


// WRITEME: the List() function is not quite right. Right now
// it is made to parse the grammar: List -> '+' List | EOF
// which is not a very interesting language. It has been included
// so you can see the basics of how to structure your recursive
// decent code.

void parser_t::List() {
  parsetree.push(NT_List);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_num:
    case T_openparen:
      RelExpr();
      eat_token(T_semicolon);
      ListP();
      break;
    case T_eof:
      parsetree.drawepsilon();
      break;
    default:
      syntax_error(NT_List);
      break;
  }
  parsetree.pop();
}

void parser_t::RelExpr() {
  parsetree.push(NT_RelExpr);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_num:
    case T_openparen:
      Expr();
      RelExprP();
      break;
    default:
      syntax_error(NT_RelExpr);
      break;
  }
  parsetree.pop();
}

void parser_t::Expr() {
  parsetree.push(NT_Expr);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_num:
    case T_openparen:
      Term();
      ExprP();
      break;
    default:
      syntax_error(NT_Expr);
      break;
  }
  parsetree.pop();
}

void parser_t::Term() {
  parsetree.push(NT_Term);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_num:
    case T_openparen:
      Factor();
      TermP();
      break;
    default:
      syntax_error(NT_Term);
      break;
  }
  parsetree.pop();
}

void parser_t::Factor() {
  parsetree.push(NT_Factor);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_num:
      eat_token(T_num);
      break;
    case T_openparen:
      eat_token(T_openparen);
      Expr();
      eat_token(T_closeparen);
      break;
    default:
      syntax_error(NT_Factor);
      break;
  }
  parsetree.pop();
}

void parser_t::TermP() {
  parsetree.push(NT_TermP);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_times:
    case T_div:
      eat_token(token);
      Term();
      break;
    case T_plus:
    case T_minus:
    case T_closeparen:
    case T_lt:
    case T_gt:
    case T_eq:
    case T_semicolon:
      break;
    default:
      syntax_error(NT_TermP);
      break;
  }
  parsetree.pop();
}

void parser_t::ExprP() {
  parsetree.push(NT_ExprP);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_plus:
    case T_minus:
      eat_token(token);
      Expr();
      break;
    case T_closeparen:
    case T_lt:
    case T_gt:
    case T_eq:
    case T_semicolon:
      break;
    default:
      syntax_error(NT_ExprP);
      break;
  }
  parsetree.pop();
}

void parser_t::RelExprP() {
  parsetree.push(NT_RelExprP);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_lt:
    case T_gt:
    case T_eq:
      eat_token(token);
      Expr();
      break;
    case T_semicolon:
      break;
    default:
      syntax_error(NT_RelExprP);
      break;
  }
  parsetree.pop();
}

void parser_t::ListP() {
  parsetree.push(NT_ListP);
  token_type token = scanner.next_token();
  switch(token)
  {
    case T_num:
    case T_openparen:
      List();
      break;
    case T_eof:
      break;
    default:
      syntax_error(NT_ListP);
      break;
  }
  parsetree.pop();
}

/*** Main ***********************************************/

int main(int argc, char* argv[])
{
  // just scanner
  if (argc > 1) {
    if (strcmp(argv[1], "-s") == 0) {
      scanner_t scanner;
      token_type tok = scanner.next_token();
      while(tok != T_eof){
        scanner.eat_token(tok);
        printf("%s", token_to_string(tok));
        tok = scanner.next_token();
      }
      printf("%s\n", token_to_string(tok));
    } else {
      cout << "Usage: " << argv[0] << " [-s]" << endl;
      exit(1);
    }
  }
  else {
    parser_t parser;
    parser.parse();
  }
  return 0;
}
