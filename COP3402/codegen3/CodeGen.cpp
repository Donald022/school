#include <iostream>
#include <cmath>
#include <vector>

#include <stdio.h>

#include "antlr4-runtime.h"

#include "SimpleIRLexer.h"
#include "SimpleIRParser.h"
#include "SimpleIRBaseListener.h"

using namespace antlr4;
using namespace std;

const int bytewidth = 8;
static const char *registers[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

const char *filename;
map<string, int> symtab;
int stackoffset = 0;

static string operand_to_string(antlr4::Token *token) {
  stringstream operand;
  if (SimpleIRParser::NAME == token->getType()) {
    operand << symtab[token->getText()] << "(%rbp)";
  } else if (SimpleIRParser::NUM == token->getType()) {
    operand << "$" << token->getText();
  } else {
    assert(false);
  }
  return operand.str();
}

class CodeGen : public SimpleIRBaseListener {
public:
  virtual void enterUnit(SimpleIRParser::UnitContext * ctx) override {
    cout << "\t.file \"" << filename << "\"" << endl;
    cout << "\t.section .note.GNU-stack,\"\",@progbits" << endl;
    cout << "\t.text" << endl;
  }

  virtual void enterFunction(SimpleIRParser::FunctionContext * ctx) override {
    string name = ctx->functionName->getText();
    cout << "\t.globl " << name << endl;
    cout << "\t.type " << name << ", @function" << endl;
    cout << name << ":" << endl;
    cout << "\t# prologue, update stack pointer" << endl;
    cout << "\tpushq\t%rbp # save old base ponter" << endl;
    cout << "\tmovq\t%rsp, %rbp # set new base pointer" << endl;
    cout << "\tpush\t%rbx # %rbx is callee-saved" << endl;
  }

  virtual void enterEnd(SimpleIRParser::EndContext * ctx) override {
    cout << "\t# epilogue" << endl;
    cout << "\tadd\t$" << stackoffset << ", %rsp" << endl;
    cout << "\tpop\t%rbx # restore %rbx" << endl;
    cout << "\tpop\t%rbp # restore old base pointer" << endl;
    cout << "\tret" << endl;
  }

  virtual void enterLocalVariables(SimpleIRParser::LocalVariablesContext * ctx) override {
    auto variables = ctx->variables;
    int starting_offset = 2;
    for (int variable_i = 0; variable_i < variables.size(); variable_i++) {
      string variable = variables[variable_i]->getText();
      int offset = (starting_offset + variable_i) * bytewidth;
      symtab[variable] = -1 * offset;
    }
    int stackspace = variables.size() * bytewidth;
    stackoffset = std::ceil(stackspace / 8) * 8;
    stackoffset += (stackoffset + 8) % 16;
    cout << "\t# allocate stack space for locals" << endl;
    cout << "\tsub\t$" << stackoffset << ", %rsp" << endl;
  }

  virtual void enterParameters(SimpleIRParser::ParametersContext * ctx) override {
    auto formals = ctx->formals;
    for (int i = 0; i < formals.size(); i++) {
      string name = formals[i]->getText();
      if (i < 6) {
        cout << "\t# move register parameter " << name << " to local variable" << endl;
        cout << "\tmov\t" << registers[i] << ", " << symtab[name] << "(%rbp)" << endl;
      } else {
        int stackparam = 16 + (i - 6) * bytewidth;
        cout << "\t# move stack parameter " << name << " to local variable" << endl;
        cout << "\tmov\t" << stackparam << "(%rbp), %rax" << endl;
        cout << "\tmov\t%rax, " << symtab[name] << "(%rbp)" << endl;
      }
    }
  }

  virtual void enterReturnStatement(SimpleIRParser::ReturnStatementContext * ctx) override {
    string operand = operand_to_string(ctx->operand);
    cout << "\t# set return value" << endl;
    cout << "\tmov\t" << operand << ", %rax" << endl;
  }

  virtual void enterStatement(SimpleIRParser::StatementContext * ctx) override {
  }

  virtual void enterAssign(SimpleIRParser::AssignContext * ctx) override {
    string operand = operand_to_string(ctx->operand);
    cout << "\t# assign " << ctx->operand->getText() << " to " << ctx->variable->getText() << endl;
    cout << "\tmov\t" << operand << ", %rax" << endl;
    cout << "\tmov\t%rax, " << symtab[ctx->variable->getText()] << "(%rbp)" << endl;
  }

  virtual void enterOperation(SimpleIRParser::OperationContext * ctx) override {
    string operand1 = operand_to_string(ctx->operand1);
    string operand2 = operand_to_string(ctx->operand2);
    cout << "\tmov\t" << operand1 << ", %rax" << endl;
    cout << "\tmov\t" << operand2 << ", %rbx" << endl;
    switch (ctx->operatorKind->getType()) {
      case SimpleIRParser::PLUS:
        cout << "\tadd\t%rbx, %rax" << endl;
        break;
      case SimpleIRParser::MINUS:
        cout << "\tsub\t%rbx, %rax" << endl;
        break;
      case SimpleIRParser::STAR:
        cout << "\timul\t%rbx, %rax" << endl;
        break;
      case SimpleIRParser::SLASH:
        cout << "\tcqo" << endl;
        cout << "\tidiv\t%rbx" << endl;
        break;
      case SimpleIRParser::PERCENT:
        cout << "\tcqo" << endl;
        cout << "\tidiv\t%rbx" << endl;
        cout << "\tmov\t%rdx, %rax" << endl;
        break;
      default:
        assert(false);
    }
    cout << "\tmov\t%rax, " << symtab[ctx->variable->getText()] << "(%rbp)" << endl;
  }

  virtual void enterCall(SimpleIRParser::CallContext * ctx) override {
    auto actuals = ctx->actuals;
    int numstack = actuals.size() > 6 ? actuals.size() - 6 : 0;
    // pass stack-allocated arguments in reverse order
    for (int i = actuals.size() - 1; i >= 6; i--) {
      cout << "\tpush\t" << symtab[actuals[i]->getText()] << "(%rbp)" << endl;
    }
    // pass the first six parameters via the pre-defined set of registers
    for (int i = (actuals.size() < 6 ? actuals.size() : 6) - 1; i >= 0; i--) {
      cout << "\tmov\t" << symtab[actuals[i]->getText()] << "(%rbp), " << registers[i] << endl;
    }
    // make the call
    cout << "\tcall\t" << ctx->functionName->getText() << endl;
    // restore the stack pointer
    cout << "\tadd\t$" << (numstack * bytewidth) << ", %rsp" << endl;
    // save the return value
    cout << "\tmov\t%rax, " << symtab[ctx->variable->getText()] << "(%rbp)" << endl;
  }

  virtual void enterLabel(SimpleIRParser::LabelContext * ctx) override {
    cout << ctx->labelName->getText() << ":" << endl;
  }

  virtual void enterGotoStatement(SimpleIRParser::GotoStatementContext * ctx) override {
    cout << "\tjmp\t" << ctx->labelName->getText() << endl;
  }

  virtual void enterIfGoto(SimpleIRParser::IfGotoContext * ctx) override {
    string operand1 = operand_to_string(ctx->operand1);
    string operand2 = operand_to_string(ctx->operand2);
    cout << "\tmov\t" << operand1 << ", %rax" << endl;
    cout << "\tmov\t" << operand2 << ", %rbx" << endl;
    cout << "\tcmp\t%rbx, %rax" << endl;
    switch (ctx->operatorKind->getType()) {
      case SimpleIRParser::EQ:
        cout << "\tje\t" << ctx->labelName->getText() << endl;
        break;
      case SimpleIRParser::NEQ:
        cout << "\tjne\t" << ctx->labelName->getText() << endl;
        break;
      case SimpleIRParser::LT:
        cout << "\tjl\t" << ctx->labelName->getText() << endl;
        break;
      case SimpleIRParser::LTE:
        cout << "\tjle\t" << ctx->labelName->getText() << endl;
        break;
      case SimpleIRParser::GT:
        cout << "\tjg\t" << ctx->labelName->getText() << endl;
        break;
      case SimpleIRParser::GTE:
        cout << "\tjge\t" << ctx->labelName->getText() << endl;
        break;
      default:
        assert(false);
    }
  }

  virtual void enterDereference(SimpleIRParser::DereferenceContext * ctx) override {
    cout << "\tmov\t" << symtab[ctx->operand->getText()] << "(%rbp), %rax" << endl;
    cout << "\tmov\t(%rax), %rbx" << endl;
    cout << "\tmov\t%rbx, " << symtab[ctx->variable->getText()] << "(%rbp)" << endl;
  }

  virtual void enterReference(SimpleIRParser::ReferenceContext * ctx) override {
    cout << "\tmov\t%rbp, %rax" << endl;
    cout << "\tadd\t$" << symtab[ctx->operand->getText()] << ", %rax" << endl;
    cout << "\tmov\t%rax, " << symtab[ctx->variable->getText()] << "(%rbp)" << endl;
  }

  virtual void enterAssignDereference(SimpleIRParser::AssignDereferenceContext * ctx) override {
    string operand = operand_to_string(ctx->operand);
    cout << "\tmov\t" << symtab[ctx->variable->getText()] << "(%rbp), %rax" << endl;
    cout << "\tmov\t" << operand << ", %rbx" << endl;
    cout << "\tmov\t%rbx, (%rax)" << endl;
  }

};

int main(int argc, const char* argv[]) {
  ANTLRInputStream *input;

  if (argc > 1) {
    ifstream fstream;
    filename = argv[1];
    fstream.open(filename);
    ANTLRInputStream *finput = new ANTLRInputStream(fstream);
    input = finput;
  } else {
    filename = "stdin";
    input = new ANTLRInputStream(cin);
  }

  SimpleIRLexer lexer(input);
  CommonTokenStream tokens(&lexer);
  SimpleIRParser parser(&tokens);

  tree::ParseTree *tree = parser.unit();
  CodeGen listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  return 0;
}
