# Grammar Rules
This document defines the rules of grammar for Iliad. Terms in *italics* are non-terminal, while terms in
ALL_CAPS are terminals. Literals are enclosed in either single ('') or double ("") quotes.

-----

### Declarations

Term | Rule
---  | ---:  
program | *declaration*\* EOF 
declaration  | *classDec* \| *functionDec* \| *varDec* \| *statement*
classDec | "class" IDENTIFIER "{" *memberDec*\* "}"
functionDec | *type* IDENTIFIER "(" *parameters*? ")" *block*
varDec | *type* IDENTIFIER ( "=" *expression* )? ";"
memberDec | ACCESS_SPECIFIER? ( *functionDec* \| *varDec* )

### Statements

Term | Rule
--- | ---:
statement | *expressionStmt* \| *forStmt* \| *ifStmt* \| *whileStmt* \| *returnStmt* \| *block*
expressionStmt | *expression* ";"
forStmt | "for" "(" ( *varDec* \| *expressionStmt* \| ";" ) *expression*? ";" *expression*? ")" *block*
ifStmt | "if" "(" *expression* ")" ( *block* \| *returnStmt* ";" )
whileStmt | "while" "(" *expression* ")" *block*
returnStmt | "return" *expression*? ";"
block | "{" *declaration*\* "}"

### Expressions

Term | Rule
--- | ---:
expression | *assignment*
assignment | ( *call* "." )? IDENTIFIER "=" *expression* \| *logicOr* 
logicOr | *logicAnd* ( "\|\|" *logicAnd* )\*
logicAnd | *equality* ( "&&" *logicAnd* )\*
equality | *comparison* ( ( "==" \| "!=" )  *comparison* )\*
comparison | *addition* ( ( "<" \| ">" \| "<=" \| ">=" ) *addition* )\*
addition | *multiplication* ( ( "+" \| "-" ) *multiplication* )\*
multiplication | *unary* ( ( "/" \| "*" ) *unary* )\*
unary | ( "-" \| "!" ) *unary* \| *call*
call | *primary* ( "(" *arguments*? ")" \| "." IDENTIFIER )\*
primary | "true" \| "false" \| "this" \| NUMBER \| STRING \| IDENTIFIER \| "(" expression ")" \| "super" "." IDENTIFIER

### Others

Term | Rule
--- | ---:
type | "int" \| "float" \| "bool" \| "var" \| IDENTIFIER
parameters | *type* IDENTIFIER ( "," *type* IDENTIFIER )\*
arguments | *expression* ( "," *expression* )\*

### Terminals

Term | Rule
---  | ---:
NUMBER | INTEGER \| FLOAT
INTEGER | DIGIT+
FLOAT | DIGIT+ "." DIGIT+
STRING | '"' .\* '"'
IDENTIFIER | ALPHA ( ALPHADIGIT )\*
ALPHA | [a-zA-Z]
DIGIT | [0-9]
ALPHADIGIT | ALPHA \| DIGIT \| "_"
