
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include "lexer.h"
#include "project4.h"
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

LexicalAnalyzer lexer;
std::map<std::string, int> locationTable;
vector<InstructionNode *> a;

Token Parser::expect(TokenType expected_type) {
    Token t = lexer.GetToken();
    return t;
}

void Parser::program() {
  var_section();
  body();
  input_s();
}

void Parser::var_section() {
  id_list();
  expect(SEMICOLON);
}

void Parser::id_list() {
  Token token1;
  token1 = expect(ID);
  Token token;
  token = lexer.peek(1);

  locationTable.insert(std::pair<std::string, int>(token1.lexeme, next_available));
  mem[next_available] = 0;
  next_available++;

  if(token.token_type == COMMA) {
     expect(COMMA);
     id_list();
  } 
}

void Parser::body() {
  expect(LBRACE);
  stmt_list();
  expect(RBRACE);
}

void Parser::stmt_list() {
  stmt();
  Token token;
  token = lexer.peek(1);
  if(token.token_type == ID || token.token_type == WHILE || token.token_type == IF || token.token_type == SWITCH || 
  token.token_type == FOR || token.token_type == OUTPUT || token.token_type == INPUT) {
    stmt_list();
  } 
}

void Parser::stmt() {
  Token token;
  token = lexer.peek(1);
  if(token.token_type == ID) {
     struct InstructionNode * temp = assign_stmt();
     a.push_back(temp);
  } 
  if(token.token_type == WHILE) {
     while_stmt();
  } 
  if(token.token_type == IF) {
     if_stmt();
  } 
  if(token.token_type == SWITCH) {
     switch_stmt();
  } 
  if(token.token_type == FOR) {
     for_stmt();
  } 
  if(token.token_type == OUTPUT) {
     output_stmt();
  } 
  if(token.token_type == INPUT) {
     input_stmt();
  } 
}
struct InstructionNode * Parser::assign_stmt() {
  Token token2;
  token2 = expect(ID);
  expect(EQUAL);
  Token token;
  token = lexer.peek(1);
  Token token1;
  token1 = lexer.peek(2);

  struct InstructionNode * i3 = new InstructionNode;
  i3->type = ASSIGN;   
                 
  for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
    if((*it).first == token2.lexeme) {
      i3->assign_inst.left_hand_side_index = (*it).second;
    }
	} 
  if(token1.token_type != SEMICOLON) {
    expr(i3);
    expect(SEMICOLON);
  }else{
    i3->assign_inst.op = OPERATOR_NONE;
    primary(i3,0);
    expect(SEMICOLON);
  }
  return i3;
}

void Parser::expr(struct InstructionNode * b) {
  primary(b, 0);
  int temp = op();
  primary(b, 1);

  if(temp == 1) {
    b->assign_inst.op = OPERATOR_PLUS;
  }else if(temp == 2) {
    b->assign_inst.op = OPERATOR_MINUS;
  }else if(temp == 3) {
    b->assign_inst.op = OPERATOR_MULT;
  }else if(temp == 4) {
    b->assign_inst.op = OPERATOR_DIV;
  }
}

void Parser::primary(struct InstructionNode * b, int check) {
  Token token;
  token = lexer.peek(1);
  Token token2;
  if(token.token_type == ID) {
     token2 = expect(ID);    
     if(check == 0) {
       for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
          if((*it).first == token2.lexeme) {
            b->assign_inst.operand1_index = (*it).second;
          }
	     }
     }
     if(check == 1) {
       for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
          if((*it).first == token2.lexeme) {
            b->assign_inst.operand2_index = (*it).second;
          }
	     }
     }   
     //a.push_back(b);
  }
  if(token.token_type == NUM) {
     Token t = expect(NUM);
     locationTable.insert(std::pair<std::string, int>(t.lexeme, next_available));
     mem[next_available] = stoi(t.lexeme);
     next_available++;
     if(check == 0) {
       for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
        if((*it).first == t.lexeme) {
          b->assign_inst.operand1_index = (*it).second;
        }
	     }
     }
     if(check == 1) {
       for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
          if((*it).first == t.lexeme) {
            b->assign_inst.operand2_index = (*it).second;
          }
	     }
     }   
     //a.push_back(b);
  }
}
int Parser::op() {
  Token token;
  token = lexer.peek(1);
  int num = 0;
  if(token.token_type == PLUS) {
     expect(PLUS);
     num = 1;
  }
  if(token.token_type == MINUS) {
     expect(MINUS);
     num = 2;
  }
  if(token.token_type == MULT) {
     expect(MULT);
     num = 3;
  }
  if(token.token_type == DIV) {
     expect(DIV);
     num = 4;
  }
  return num;
}

void Parser::output_stmt() {                            
  expect(OUTPUT);
  Token t = expect(ID);
  struct InstructionNode * i2 = new InstructionNode;
  i2->type = OUT;  
  for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
    if((*it).first == t.lexeme) {
      i2->input_inst.var_index = (*it).second;
    }
	}            
  a.push_back(i2);
  expect(SEMICOLON);
}
void Parser::input_stmt() {
  expect(INPUT);
  Token t = expect(ID);
  struct InstructionNode * i1 = new InstructionNode;

  i1->type = IN;
  for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
    if((*it).first == t.lexeme) {
      i1->input_inst.var_index = (*it).second;
    }
	}                    
  a.push_back(i1);
  expect(SEMICOLON);
}
void Parser::while_stmt() {
  Token t = expect(WHILE);
  struct InstructionNode * i6 = new InstructionNode;
  
  i6->type = CJMP;              
  condition(i6);

  struct InstructionNode * i7 = new InstructionNode;
  i7->type = NOOP;
  i6->cjmp_inst.target = i7; 
  a.push_back(i6);
  body();

  struct InstructionNode * i8 = new InstructionNode;

  i8->type = JMP; 
  i8->jmp_inst.target = i6;

  a.push_back(i8);
  a.push_back(i7);

}
void Parser::if_stmt() {
  Token t = expect(IF);
  struct InstructionNode * i4 = new InstructionNode;
  i4->type = CJMP;              

  condition(i4);

  struct InstructionNode * i5 = new InstructionNode;
  i5->type = NOOP;
  i4->cjmp_inst.target = i5; 
  a.push_back(i4);
  body();
  a.push_back(i5);              
}
void Parser::condition(struct InstructionNode * temp) {
  primary(temp, 0);
  int var = relop();
  primary(temp, 1);

  if(var == 1) {
    temp->cjmp_inst.condition_op = CONDITION_GREATER;
  }else if(var == 2) {
    temp->cjmp_inst.condition_op = CONDITION_LESS;
  }else if(var == 3) {
    temp->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
  }

}
int Parser::relop() {
  Token token;
  token = lexer.peek(1);
  int num = 0;
  if(token.token_type == GREATER) {
     expect(GREATER);
     num = 1;
  }
  if(token.token_type == LESS) {
     expect(LESS);
     num = 2;
  }
  if(token.token_type == NOTEQUAL) {
     expect(NOTEQUAL);
     num = 3;
  }
  return num;
}
void Parser::switch_stmt() {
  expect(SWITCH);
  Token t = expect(ID);
  expect(LBRACE);

  struct InstructionNode * i3 = new InstructionNode;
  i3->type = NOOP; 

  case_list(t.lexeme, i3);
  
  a.push_back(i3);

  Token token;
  token = lexer.peek(1);
  if(token.token_type == DEFAULT) {
    default_case();
  }
  expect(RBRACE);
}
void Parser::for_stmt() {
  Token t = expect(FOR);
  expect(LPAREN);

  struct InstructionNode * tempOne = assign_stmt();
  a.push_back(tempOne);

  struct InstructionNode * i8 = new InstructionNode;
  i8->type = CJMP;   
  condition(i8);

  struct InstructionNode * i9 = new InstructionNode;
  i9->type = NOOP;
  i8->cjmp_inst.target = i9;
  a.push_back(i8);

  expect(SEMICOLON);

  struct InstructionNode * tempTwo = assign_stmt();
  
  expect(RPAREN);
  body();

  a.push_back(tempTwo);

  struct InstructionNode * i10 = new InstructionNode;
  i10->type = JMP; 
  i10->jmp_inst.target = i8;
  
  a.push_back(i10);
  a.push_back(i9);
}
void Parser::case_list(std::string s, struct InstructionNode * b) {
  case_s(s, b);
  Token token;
  token = lexer.peek(1);
  if(token.token_type == CASE) {
    case_list(s, b);
  }
}

void Parser::case_s(std::string s, struct InstructionNode * b) {
  expect(CASE);

  Token t = expect(NUM);
  
  expect(COLON);

  struct InstructionNode * i1 = new InstructionNode;
  i1->type = CJMP; 
  i1->cjmp_inst.condition_op = CONDITION_GREATER;

  for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
    if((*it).first == s) {
      i1->cjmp_inst.operand1_index = (*it).second;
    }
	}
  
  mem[next_available] = stoi(t.lexeme) - 1;
  i1->cjmp_inst.operand2_index = next_available;
  next_available++;

  struct InstructionNode * i5 = new InstructionNode;
  i5->type = NOOP;
  i1->cjmp_inst.target = i5; 
  a.push_back(i1);

  struct InstructionNode * i2 = new InstructionNode;
  i2->type = CJMP; 
  i2->cjmp_inst.condition_op = CONDITION_LESS;

  for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
    if((*it).first == s) {
      i2->cjmp_inst.operand1_index = (*it).second;
    }
	}

  mem[next_available] = stoi(t.lexeme) + 1;
  i2->cjmp_inst.operand2_index = next_available;
  next_available++;

  //struct InstructionNode * i6 = new InstructionNode;
  //i6->type = NOOP;
  i2->cjmp_inst.target = i5; 
  a.push_back(i2);

  body();
  
  struct InstructionNode * i7 = new InstructionNode;
  i7->type = JMP;
  i7->jmp_inst.target = b;
  
  a.push_back(i7);
  a.push_back(i5);  
  //a.push_back(i6);
}

void Parser::default_case() {
  expect(DEFAULT);
  expect(COLON);
  body();
}

void Parser::input_s() {
  num_list();
}

void Parser::num_list() {
  Token token1 = expect(NUM);
  Token token;
  token = lexer.peek(1);
  int placeHolder = stoi(token1.lexeme);
  inputs.push_back(placeHolder);

  if(token.token_type == NUM) {
    num_list();
  }
}

void connnectInstNode() {
  for(int i = 0; i < a.size() - 1; i++) {
    a[i]->next = a[i + 1];
  }
  a[a.size() - 1]->next = NULL;
}

struct InstructionNode* parse_generate_intermediate_representation() {
  Parser p;
  p.program();
  connnectInstNode();
  /*
  for (const auto& p : locationTable) {
    std::cout << p.first << std::endl; 
    std::cout << p.second << std::endl;
  }
  */
  /*
  for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
    cout << "h";
    cout << (*it).first;
	}
  */
  
  /*
  for (map<string, int>::iterator it = locationTable.begin(); it != locationTable.end(); ++it) {
    cout << (*it).first << ",";
    cout << (*it).second;
    cout << "\n";
	}
  */
  return a[0];
}
