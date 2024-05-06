#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "tree.h"

Node* GetGrammar (Tokens* tkns);

Node* GetAll (Tokens* tkns);

Node* GetFunc (Tokens* tkns);

Node* GetBlock (Tokens* tkns);

Node* GetLogicalExpression (Tokens* tkns);

Node* GetAdditiveExpression (Tokens* tkns);

Node* GetMultiplicativeExpression (Tokens* tkns);

Node* GetFuncArguments (Tokens* tkns);

Node* GetPrimaryExpression (Tokens* tkns);

Node* GetIfWhile    (Tokens* tkns);

Node* GetOp         (Tokens* tkns);
   
Node* GetAssign          (Tokens* tkns);
   
Node* GetUnaryOperation  (Tokens* tkns);



#endif
