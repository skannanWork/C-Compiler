
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
using namespace std;

class Parser {
  public:
    void program();
  private:
    Token expect(TokenType expected_type);
    void var_section();
    void id_list();
    void body();
    void stmt_list();
    void stmt();
    struct InstructionNode * assign_stmt();
    void expr(struct InstructionNode * b);
    void primary(struct InstructionNode * b, int a);
    int op();
    void output_stmt();
    void input_stmt();
    void while_stmt();
    void if_stmt();
    void condition(struct InstructionNode * a);
    int relop();
    void switch_stmt();
    void for_stmt();
    void case_list(std::string sk, struct InstructionNode * a);
    void case_s(std::string sk2, struct InstructionNode * a);
    void default_case();
    void input_s();
    void num_list(); 
};
#endif
