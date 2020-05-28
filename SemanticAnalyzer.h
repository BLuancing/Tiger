/*
     Created by:    Braden Luancing
     Major:         Computer Science
     Creation Date: 11/28/19
     Due Date:      12/8/19
     Course:        CSC425
     Professor:     Dr. Schwesinger
     Assignment:    #3
     Filename:      SemanticAnalyzer.h
     Purpose:       Creates an abstract syntax tree from a parsed source file in
                    the Tiger language.

*/

/** @defgroup SEMANTIC Semantic Analyzer
 *  Everything for semantic analysis.
 *  @{
 */

#ifndef SEMANTIC_ANALYZER
#define SEMANTIC_ANALYZER

#include <string>
#include <vector>
#include <stack>
#include "ast.h"
#include "SymbolTable.h"

using namespace std;

/**
 * @brief Performs semantic analysis on an abstract syntac tree for a Tiger program.
 * This includes scope checking and type checking for all productions.
 * 
 */
class SemanticAnalyzer{
     public:
          /**
           * @brief Construct a new Semantic Analyzer object with an AST.
           * 
           * @param astRoot 
           */
          SemanticAnalyzer(node* astRoot);

          /**
           * @brief Starts analysis on the tree passed to this object.
           * 
           */
          void AnalyzeTree();

          /**
           * @brief The AST to perform semantic checks on.
           * 
           */
          node* astRoot;
};

/**
 * @brief A node visitor focused on performing semantic checks at each node.
 * 
 */
class nodeSAChecker : public nodeVisitor {
     public:

     /**
      * @brief Construct a new node semantic analysis node checker object.
      * 
      */
     nodeSAChecker();

     /**
      * @brief Verifies type of subnodes, performs semantic checks on itself,
      * then sets its own type once all are successful.
      * 
      * Executed recursively by every node in tree starting at top.
      * @param Node Node to act on.
      */
     void check(node* Node);

     /**
      * @brief Prints out an error message with a line number before exiting with error
      * code 3.
      * 
      * @param lineNumber Line number at which the error occurred.
      * @param errorMessage An informative message about the error.
      */
     void throwError(const int &lineNumber, const string &errorMessage);
     
     // Generic checks for ease
     // Borderline stolen from 
     // https://cs.lmu.edu/~ray/notes/writingacompiler/

     /**
      * @brief Returns true if the passed in type is an array type.
      * 
      * @return true If the passed in type is an array type.
      * @return false if the passed in type is not an array type.
      */
     bool isArrayType(Type*);

     /**
      * @brief Returns true if the passed in type is a record type.
      * 
      * @return true If the passed in type is a record type.
      * @return false if the passed in type is not a record type.
      */
     bool isRecordType(Type*);

     /**
      * @brief Returns true if the passed in node's current type is
      * an array type.
      * 
      * @return true If the passed in node's current type is an array type.
      * @return false If the passed in node's current type is not an array type.
      */
     bool isArray(node*);

     /**
      * @brief Returns true if the passed in node's current type is
      * a record type.
      * 
      * @return true If the passed in node's current type is a record type.
      * @return false If the passed in node's current type is not a record type.
      */
     bool isRecord(node*);

     /**
      * @brief Returns true if the passed in node's current type is
      * an integer type.
      * 
      * @return true If the passed in node's current type is an integer type.
      * @return false If the passed in node's current type is not an integer type.
      */
     bool isInteger(node*);

     /**
      * @brief Returns true if the passed in node's current type is
      * a unit type.
      * 
      * @return true If the passed in node's current type is a unit type.
      * @return false If the passed in node's current type is not a unit type.
      */
     bool isUnit(node*);

     /**
      * @brief Returns true if the passed in node's current type is
      * an integer or string type.
      * 
      * @return true If the passed in node's current type is an integer or string type.
      * @return false If the passed in node's current type is not an integer or string type.
      */
     bool isIntOrStr(node*);

     /**
      * @brief Returns true if two nodes are currently the same type.
      * 
      * @param left One node to check.
      * @param right Other node to check.
      * @return true If the two nodes are currently the same type.
      * @return false If the two nodes are not the same type.
      */
     bool sameType(node* left, node* right);

     /**
      * @brief Returne true if the passed CallExp's parameters match the required
      * parameters of the passed function symbol.
      * 
      * @param func The function symbol to check.
      * @param CallExp The function call to check.
      * @return true Function call parameters match function signature.
      * @return false Function call parameters match function signature.
      */
     bool legalArguments(FuncSymbol* func, callExp* CallExp);

     /**
      * @brief Retturns true if a node can be assigned a type.
      * 
      * @param Node Node to check.
      * @param type Type to check against.
      * @return true Ff a node can be assigned a type.
      * @return false Ff a node cannot be assigned a type.
      */
     bool isAssignableTo(node* Node, Type* type);

     /**
      * @brief Prematurely checks a function's signature to allow
      * for recursive functions.
      * 
      * @param FunDec The function declcaration to verify the signature of.
      */
     void checkFunctionSignature(funDec* FunDec);

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
      * @brief A symbol table holding all identifiers, types, and scopes
      * as semantic checks are run.
      * 
      */
     SymbolTable table;
};
/** @} */
#endif