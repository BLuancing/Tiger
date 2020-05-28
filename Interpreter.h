/*
     Created by:    Braden Luancing
     Major:         Computer Science
     Creation Date: 12/3/19
     Due Date:      12/8/19
     Course:        CSC425
     Professor:     Dr. Schwesinger
     Assignment:    #4
     Filename:      Interpreter.h
     Purpose:       Given a semantically valid AST, will interpret
                    and execute Tiger code.

*/

/** @defgroup INTERPRET Interpreter
 *  Given a semantically valid AST, will interpret and execute Tiger code.
 *  @{
 */

#include <vector>
#include <map>
#include "ast.h"
#include "SemanticAnalyzer.h"
#include "SymbolTable.h"

using namespace std;

/**
 * @brief Given a semantically valid AST, will interpretand execute Tiger code.
 * 
 */
class Interpreter{
     public:
          /**
           * @brief Creates a new Interpreter with a reference to a Tiger AST.
           * 
           * @param astRoot 
           */
          Interpreter(node* astRoot);

          /**
           * @brief Begins interpretation of the Tiger AST.
           * 
           */
          void Interpret();

          /**
           * @brief The root of the AST to interpret.
           * 
           */
          node* astRoot;
};

/**
 * @brief A node visitor designed to interpret and set values on tree nodes.
 * 
 */
class nodeInterpreter : public nodeVisitor{
     public:
          /**
           * @brief Construct a new node Interpreter object with the default Tiger
           * symbol table.
           * 
           */
          nodeInterpreter();

          /**
           * @brief Needed by visitor pattern to go between nodes. Sets the value of the current node
           * and its children, when applicable.
           * 
           * @param Node The node to evaluate.
           */
          void evaluate(node* Node);

          /**
           * @brief Whether or not a break has been called while the node visitor
           * is traversing the tree.
           * 
           */
          bool breakCalled = false;



          /*******************
           * HELPER FUNCTIONS
           * ****************/

          /**
           * @brief Returns whether or not a value is 0
           * 
           * @return true If the value is anything other than 0.
           * @return false If the value is zero.
           */
          bool isNonZero(Value*);





          /************************
           * BUILT IN FUNCTIONS
           * *********************/

          /**
           * @brief Prints an integer value to std out. Used for builtin printi function.
           * 
           * @param value The integer value to print.
           */
          void Print(IntValue* value);

          /**
           * @brief Prints a string value to std out. Used for builtin print function.
           * 
           * @param value The string value to print.
           */
          void Print(StringValue* value);

          /**
           * @brief Returns 1 value if 0, and returns 0 value for anything else. 
           * Used for builtin not function.
           * 
           * @return IntValue* Integer value to not.
           */
          IntValue* Not(IntValue*);

           //Visitor functions
          void visitProgram(program* prog) override;
          void visitBreak(NBreak* Break) override;
          void visitNil(NNil* Nil) override;
          void visitID(NId* id) override;
          void visitTyID(NTyId* tyid) override;
          void visitSubscript(subscript* Subscript) override;
          void visitFieldExp(fieldExp* FieldExp) override;
          void visitSeqExp(seqExp*) override;
          void visitNegation(negation*) override;
          void visitCallExp(callExp*) override;
          void visitIntLit(NIntLit*) override;
          void visitStrLit(NStrLit*) override;
          void visitInfixExp(infixExp*) override;
          void visitArrCreate(arrCreate*) override;
          void visitRecCreate(recCreate*) override;
          void visitFieldCreate(fieldCreate*) override;
          void visitAssignment(assignment*) override;
          void visitIfThenElse(ifThenElse*) override;
          void visitWhileExp(whileExp*) override;
          void visitForExp(forExp*) override;
          void visitLetExp(letExp*) override;
          void visitDec(decc*) override;
          void visitTyDec(tyDec*) override;
          void visitTyDef(tyDef*) override;
          void visitRefTy(refTy*) override;
          void visitArrTy(arrTy*) override; 
          void visitRecTy(recTy*) override;   
          void visitFieldDec(fieldDec*) override;
          void visitFunDec(funDec*) override; 
          void visitVarDec(varDec*) override;

          /**
           * @brief Symbol table used during interpretation to track values and symbols.
           * 
           */
          SymbolTable table;

          /**
           * @brief Contains the body and name of all user defined functions.
           * 
           */
          map<string, node*> functionExpressions;

          /**
           * @brief Contains the name and all function parameters for user defined function.
           * Utilized during callExp to make new scope with identifiers.
           * 
           */
          map<string, vector < pair<string,Type*> > > functionParams; //Parallel to function expressions
};
/** @} */