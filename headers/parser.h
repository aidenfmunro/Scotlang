#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "tree.h"

Node* GetGrammar                   (Tokens* tkns);

Node* GetAll                       (Tokens* tkns);

Node* GetFunc                      (Tokens* tkns);

Node* GetBlock                     (Tokens* tkns);

Node* GetLogicalExpression         (Tokens* tkns);

Node* GetAdditiveExpression        (Tokens* tkns);

Node* GetMultiplicativeExpression  (Tokens* tkns);

Node* GetFuncArguments             (Tokens* tkns);
     
Node* GetPrimaryExpression         (Tokens* tkns);
     
Node* GetIfWhile                   (Tokens* tkns);
          
Node* GetOp                        (Tokens* tkns);
             
Node* GetAssign                    (Tokens* tkns);
          
Node* GetReturn                    (Tokens* tkns);
          
Node* GetInputPrint                (Tokens* tkns);
             
Node* GetUnaryOperation            (Tokens* tkns);

Node* GetFuncExpression            (Tokens* tkns);


#define curNode tkns->token[tkns->curTokenNum].elem

#define curToken tkns->token[tkns->curTokenNum]

#define prevNode tkns->token[tkns->curTokenNum - 1].elem

#define nextNode tkns->token[tkns->curTokenNum + 1].elem

#define REQUIRE(keyword) (curNode->data.id == keyword)

#define curPos  tkns->curTokenNum

#define PrintCurrentParserState() printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In %s...\n Current token: ", __PRETTY_FUNCTION__); \
                                  PrintToken(&curToken) 

#endif
