
%{
  #include <stdio.h>
  int yylex(void);
  void yyerror(char *);
%}


%%

List    : List RelExpr ';' {printf("parsed expresion\n");}
        | RelExpr ';' {printf("parsed expresion\n");}
        ;

RelExpr : Expr '<' Expr
        | Expr '>' Expr
        | Expr '=' Expr
        | Expr

Expr    : Expr '+' Term
        | Expr '-' Term
        | Term
        ;

Term    : Term '*' Factor
        | Term '/' Factor
        | Factor
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
