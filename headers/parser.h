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

Node* GetFuncArguments (Tokens* tkns);

Node* GetE       (Tokens* tkns);

Node* GetIfWhile (Tokens* tkns);

Node* GetT       (Tokens* tkns);

Node* GetOp      (Tokens* tkns);

Node* GetA       (Tokens* tkns);

Node* GetName    (Tokens* tkns);

Node* GetP       (Tokens* tkns);


#endif
