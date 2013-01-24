
%{
  #include <stdio.h>
  int yylex(void);
  void yyerror(char *);
%}


%%

List    : RelExpr ';' List2 {printf("parsed expression\n");}
        ;

List2   : /* empty */
        | List
        ;

RelExpr : Expr RelExpr2
        ;

RelExpr2: /* empty */
        | '<' Expr
        | '>' Expr
        | '=' Expr
        ;

Expr    : Term Expr2
        ;

Expr2   : /* empty */
        | '+' Expr
        | '-' Expr
        ;

Term    : Factor Term2
        ;

Term2   : /* empty */
        | '*' Term
        | '/' Term
        ;

Factor  : 'n'
        | '(' Expr ')'
        ;

%%

void yyerror(char *s) {
  fprintf(stderr, "%s\n", s);
  return;
}

int main(void) {
  yyparse();
  return 0;
}
