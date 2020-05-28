/*
     Created by:    Braden Luancing
     Major:         Computer Science
     Creation Date: 10/7/19
     Due Date:      12/8/19
     Course:        CSC425
     Professor:     Dr. Schwesinger
     Assignment:    #3
     Filename:      ast.h
     Purpose:       Creates an abstract syntax tree from a parsed source file in
                    the Tiger language.

*/

/** @defgroup AST Abstract Syntax Tree
 *  Everything for constructing the AST.
 *  @{
 */
#ifndef AST
#define AST

#include <string>
#include <vector>
#include "SymbolTable.h"

using namespace std;

//Forward declarations for visitor pattern
class program;
class exp;
class decc;
class tyDec;
class tyDef;
class refTy;
class arrTy;
class recTy;
class fieldDec;
class funDec;
class varDec;
class lValue;
class subscript;
class fieldExp;
class seqExp;
class negation;
class callExp;
class infixExp;
class arrCreate;
class recCreate;
class fieldCreate;
class assignment;
class ifThenElse;
class whileExp;
class forExp;
class letExp;

class NId;
class NTyId;
class NIntLit;
class NStrLit;
class NBreak;
class NNil;


/**
 * @brief An abstract class implementing the visitor pattern. Used for semantic analysis
 * and interpretation to traverse the tree easily while working with subclasses.
 * 
 */
class nodeVisitor{
     public:
          //I feel like these are self explanatory.
          virtual void visitProgram(program* prog) = 0;
          virtual void visitBreak(NBreak* Break) = 0;
          virtual void visitNil(NNil* Nil) = 0;
          virtual void visitSubscript(subscript* Subscript) = 0;
          virtual void visitFieldExp(fieldExp* FieldExp) = 0;
          virtual void visitSeqExp(seqExp*) = 0;
          virtual void visitNegation(negation*) = 0;
          virtual void visitCallExp(callExp*) = 0;
          virtual void visitInfixExp(infixExp*) = 0;
          virtual void visitArrCreate(arrCreate*) = 0;
          virtual void visitRecCreate(recCreate*) = 0;
          virtual void visitFieldCreate(fieldCreate*) = 0;
          virtual void visitAssignment(assignment*) = 0;
          virtual void visitIfThenElse(ifThenElse*) = 0;
          virtual void visitWhileExp(whileExp*) = 0;
          virtual void visitForExp(forExp*) = 0;
          virtual void visitLetExp(letExp*) = 0;
          virtual void visitDec(decc*) = 0;
          virtual void visitTyDec(tyDec*) = 0;
          virtual void visitTyDef(tyDef*) = 0;
          virtual void visitRefTy(refTy*) = 0;
          virtual void visitArrTy(arrTy*) = 0;
          virtual void visitRecTy(recTy*) = 0;
          virtual void visitFieldDec(fieldDec*) = 0;
          virtual void visitFunDec(funDec*) = 0;
          virtual void visitVarDec(varDec*) = 0;
          virtual void visitID(NId* id) = 0;
          virtual void visitTyID(NTyId*) = 0;
          virtual void visitIntLit(NIntLit*) = 0;
          virtual void visitStrLit(NStrLit*) = 0;
          
};

/**
 * @brief Represents a node in an abstract syntax tree. Contains line number, node Type,
 * and the value the node currently holds (during interpretation).
 * 
 */
class node
{
     public:
          /**
          * @brief Construct a new node object with a passed in line number.
          * 
          * @param lineNumber The line number of the code bit that the node represents.
          */
          node(const int &lineNumber);  

          /**
           * @brief Used in conjunction with visitor pattern to allow the visitor to traverse
           * the tree while grabbing the specific node subclass that represents it.
           * 
           * @param visitor 
           */
          virtual void accept(nodeVisitor* visitor) = 0;

          /**
           * @brief Default destructor with no implementation.
           * 
           */
          virtual ~node() {}

          /**
           * @brief Line number of the code bit that the node represents.
           * 
           */
          int lineNumber;
          
          /**
           * @brief Current type of the node; needed for semantic analysis.
           * 
           */
          Type* type;

          /**
           * @brief Current value of the node during interpretation
           * 
           */
          Value* value;
          
     
};

/**
 * @brief Represents the program production in the Tiger grammar. Top node of all Tiger programs.
 * 
 */
class program : public node
{
     public:
          /**
           * @brief Any expression.
           * 
           */
          node* Node;

          /**
           * @brief Construct a new program object with a line number and base expression.
           * 
           */
          program(const int &,node*);

          /**
           * @brief Required by visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};

/**
 * @brief Used to distinguish between types of declarations during semantic analysis.
 * Needed to determine which downcast from a DECC node to do.
 * 
 */
enum DecKind{
     /**
      * @brief A declaration for a type.
      * 
      */
     D_TY,
     /**
      * @brief A declaration for a variable.
      * 
      */
     D_VAR,
     /**
      * @brief A declaration for a function.
      * 
      */
     D_FUNC
};

/**
 * @brief Represents a dec production in the Tiger language. Dummy class used in semantic
 * analysis to allow for some polymorphism for declarations.
 * 
 */
class decc : public node
{
     public:
          /**
           * @brief Construct a new decc object with its line number.
           * 
           * @param lineNumber The line number of the code bit that the node represents.
           */
          decc(const int &lineNumber);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;

          /**
           * @brief The identifier node that gives name to this declaration.
           * 
           */
          node* id;

          /**
           * @brief The type of declaration this is (type, variable, or function)
           * 
           */
          DecKind kind;

};

/**
 * @brief Represents the tyDec production in the Tiger language. Used for declaring new types.
 * 
 */
class tyDec : public decc 
{
     public:
          /**
           * @brief The definition of the type, either a reference type, array
           * type, or record type.
           * 
           */
          node* tyDef;

          /**
           * @brief Construct a new tyDec object.
           * 
           * @param lineNumber The line number of the code bit that the node represents.
           * @param tyId The identifier for the new type.
           * @param tyDef The definition for the new type.
           */
          tyDec(const int &lineNumber, node* tyId, node* tyDef);

          /**
           * @brief Required by visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};

/**
 * @brief Used to distinguish between type definition kinds during semantic analysis.
 * 
 */
enum DefKind
{
     /**
      * @brief A type that references another type (such as 'type a = int')
      * 
      */
     DEF_REF,
     /**
      * @brief An array type.
      * 
      */
     DEF_ARR,
     /**
      * @brief A record type.
      * 
      */
     DEF_REC
};

/**
 * @brief Represents the tyDef production in the Tiger language. A dummy
 * type used for polymorphism among type definitions.
 * 
 */
class tyDef : public node
{
     public:
          /**
           * @brief Construct a new ty Def object.
           * 
           * @param lineNumber The line number of the code bit that the node represents.
           */
          tyDef(const int &lineNumber);
          /**
           * @brief Required by visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;

          /**
           * @brief Converts the DefKind on this node (e.g. DEF_REF) to a TypeKind (e.g. T_REF)
           * 
           * @return TypeKind The TypeKind equivalent of the DefKind on this node.
           */
          TypeKind kindToTypeKind();  

          /**
           * @brief The kind of type definition that this node is: reference, array, or record.
           * 
           */
          DefKind kind;
};

/**
 * @brief Represents a reference type declaration.
 * 
 */
class refTy : public tyDef
{
     public:
          /**
           * @brief Construct a new ref Ty object.
           * 
           * @param lineNumber The line number of the code bit that the node represents.
           * @param tyId The identifier of the type that this new type will reference.
           */
          refTy(const int &lineNumber, node* tyId);
          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
          /**
           * @brief The identifier of the type that this new type will reference.
           * 
           */
          node* tyId;
};

/**
 * @brief Represents an array type declaration.
 * 
 */
class arrTy : public tyDef 
{
     public:
          /**
           * @brief Construct a new arr Ty object.
           * 
           * @param lineNumber The line number of the code bit that the node represents.
           * @param tyId What type this new will be an array of (array of <blank>).
           */
          arrTy(const int &lineNumber, node* tyId);
          /**
           * @brief Required for visitor pattern
           * 
           */
          void accept(nodeVisitor*) override;
          /**
           * @brief What type this new will be an array of (array of <blank>).
           * 
           */
          node* tyId; 
              
};
/**
 * @brief Represents a record type declaration.
 * 
 */
class recTy : public tyDef 
{
     public:
          /**
           * @brief Collection of the field declarations that make up this record definition.
           * 
           */
          std::vector<node*>* fieldDecs;
          /**
           * @brief Construct a new rec Ty object.
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param fieldDecs Collection of the field declarations that make up this record definition.
           */
          recTy(const int &lineNumber, std::vector<node*>* fieldDecs);
          /**
           * @brief Required for visitor pattern
           * 
           */
          void accept(nodeVisitor*) override;
};
/**
 * @brief Represents a fieldDec production in the Tiger language. Used to declare fields for
 * record types.
 * 
 */
class fieldDec : public node 
{
     public:
          /**
           * @brief Name of the new field for the record.
           * 
           */
          node* id;

          /**
           * @brief Type of the new field for the record.
           * 
           */
          node* tyId;

          /**
           * @brief Construct a new field Dec object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param id Name of the new field for the record.
           * @param tyId Type of the new field for the record.
           */
          fieldDec(const int &lineNumber, node* id, node* tyId);

          /**
           * @brief Returns a pair with the name and Type of the entire field declaration.
           * 
           * @return pair<string, Type*> A pair  with the name and Type of the entire field declaration.
           */
          pair<string, Type*> FieldToPair();

          /**
           * @brief Required by visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;

};

/**
 * @brief Represent a funDec production in the Tiger language. Describes the prototype,
 * and definition of a new function.
 * 
 */
class funDec : public decc 
{
     public:
          /**
           * @brief The collection of FieldDecs that represent each formal parameter of the
           * function.
           * 
           */
          std::vector<node*>* params;

          /**
           * @brief The return type of the function.
           * 
           */
          node* returnType;

          /**
           * @brief The definition for the function.
           * 
           */
          node* exp;

          /**
           * @brief Construct a new fun Dec object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param id The identifier for the whole function.
           * @param params The collection of FieldDecs that represent each formal parameter of the function.
           * @param returnType The return type of the function.
           * @param exp The definition for the function.
           */
          funDec(const int &lineNumber, node* id, std::vector<node*>* params, node* returnType, node* exp);
          
          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;

          /**
           * @brief Constructs a list of name->type pairs for all of the parameters in the function.
           * 
           * @return vector< pair<string,Type*> > List of name->type pairs for all of the parameters in the function.
           */
          vector< pair<string,Type*> > getParams();

};
/**
 * @brief Represents a varDec production in the Tiger language. Defines a new variable with
 * a given type and initial value.
 * 
 */
class varDec : public decc 
{
     public: 
          /**
           * @brief The type of the variable.
           * 
           */
          node* tyId;

          /**
           * @brief The initial value of the variable.
           * 
           */
          node* exp;

          /**
           * @brief Construct a new var Dec object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param id The identifier of the new variable.
           * @param tyId The type of the variable.
           * @param exp The initial value of the variable.
           */
          varDec(const int &lineNumber, node* id, node* tyId, node* exp);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};
/**
 * @brief Represents the subscript production in the Tiger language. Describes
 * a subscript expression to index into an array.
 * 
 */
class subscript : public node 
{
     public:
          /**
           * @brief The array to operate on; could be a nested collection
           * of subscripts or other accesses but must come out to an array type.
           * 
           */
          node* lValue;

          /**
           * @brief The expression to evaluate for the index of the subscript.
           * 
           */
          node* exp;

          /**
           * @brief Construct a new subscript object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param pLValue The array to operate on; could be a nested collection
           * of subscripts or other accesses but must come out to an array type.
           * @param pExp The expression to evaluate for the index of the subscript.
           */
          subscript(const int &lineNumber, node* pLValue, node* pExp);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override; 
};
/**
 * @brief Represents the fieldExp production of the Tiger language. Acts as a field
 * accessor for a record (e.g. myRecord.field)
 * 
 */
class fieldExp : public node 
{
     public:
          /**
           * @brief The record to operate on; could be a nested collection
           * of subscripts or other accesses but must come out to a record type.
           * 
           */
          node* lValue;

          /**
           * @brief Identifier of the field to access.
           * 
           */
          node* ID;

          /**
           * @brief Construct a new field Exp object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param pLValue The record to operate on; could be a nested collection
           * of subscripts or other accesses but must come out to a record type.
           * @param ID Identifier of the field to access.
           */
          fieldExp(const int &lineNumber, node* pLValue, node* ID);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override; 
};
/**
 * @brief Represents the seqExp production in the Tiger language. Describes a sequence
 * expression in which there is a list of expressions to execute.
 * 
 */
class seqExp : public node 
{
     public:  
          /**
           * @brief All of the expressions contained within this sequence.
           * 
           */
          std::vector<node*> *exps;

          /**
           * @brief Construct a new seq Exp object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param exps All of the expressions contained within this sequence.
           */
          seqExp(const int &lineNumber, std::vector<node*>* exps);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};

/**
 * @brief Represents a negation in the Tiger language (e.g. -1)
 * 
 */
class negation : public node 
{
     public:
          /**
           * @brief The expression to be negated.
           * 
           */
          node* operand;

          /**
           * @brief Construct a new negation object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param operand The expression to be negated.
           */
          negation(const int &lineNumber, node* operand);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
     
};
/**
 * @brief Represents the callExp production in the Tiger language. Describes a call to a function
 * with parameters passed, if any
 * 
 */
class callExp : public node 
{
     public:
          /**
           * @brief Identifier of the function to call.
           * 
           */
          node* id;

          /**
           * @brief List of expressions to pass as parameters to the function, if any
           * 
           */
          std::vector<node*>* exps;

          /**
           * @brief Construct a new call Exp object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param id Identifier of the function to call.
           * @param exps List of expressions to pass as parameters to the function, if any
           */
          callExp(const int &lineNumber, node* id, std::vector<node*>* exps);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override; 
};
/**
 * @brief Different kinds of infix operators in the language. Stolen from Schwesinger's
 * own example for the compiler.
 * 
 */
enum Operator {
     /**
      * @brief Addition operation.
      * 
      */
     OP_ADD,
     /**
      * @brief Subtraction operation.
      * 
      */
     OP_SUBTRACT,
     /**
      * @brief Multiplication operation.
      * 
      */
     OP_MULTIPLY,
     /**
      * @brief Division operation.
      * 
      */
     OP_DIVIDE,
     /**
      * @brief Logical or operation.
      * 
      */
     OP_OR,
     /**
      * @brief Logical and operation.
      * 
      */
     OP_AND,
     /**
      * @brief Equals comparison.
      * 
      */
     OP_EQ,
     /**
      * @brief Not equals comparison.
      * 
      */
     OP_NEQ,
     /**
      * @brief Less than comparison.
      * 
      */
     OP_LT,
     /**
      * @brief Less than or equal to comparison.
      * 
      */
     OP_LEQ,
     /**
      * @brief Greater than comparison.
      * 
      */
     OP_GT,
     /**
      * @brief Greater than or equal to comparison.
      * 
      */
     OP_GEQ
};
/**
 * @brief Represents the infixExp production in the Tiger language. Describes infix operations
 * between two operands, such as addition(+), less than (<), etc.
 * 
 */
class infixExp : public node 
{
     public:
          /**
           * @brief Operator that goes with this expression.
           * 
           */
          Operator op;

          /**
           * @brief Left hand side of the expression.
           * 
           */
          node* leftNode;

          /**
           * @brief Right hand side of the expression.
           * 
           */
          node* rightNode;

          /**
           * @brief Construct a new infix Exp object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param op Operator that goes with this expression.
           * @param leftNode Left hand side of the expression.
           * @param rightNod Right hand side of the expression.
           */
          infixExp(const int &lineNumber, Operator op, node* leftNode, node* rightNod);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
     
};
/**
 * @brief Represents the arrCreate production in the Tiger language. Describes the
 * instantiation of an array type.
 * 
 */
class arrCreate : public node 
{
     public:
          /**
           * @brief The array type that is being instantiated.
           * 
           */
          node* tyId;

          /**
           * @brief Expression to describe size of array.
           * 
           */
          node* subscriptExp;
          
          /**
           * @brief Expression to initialize all values of array to.
           * 
           */
          node* postExp;

          /**
           * @brief Construct a new arr Create object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param tyId The array type that is being instantiated.
           * @param subscriptExp Expression to describe size of array.
           * @param postExp  Expression to initialize all values of array to.
           */
          arrCreate(const int &lineNumber, node* tyId, node* subscriptExp, node* postExp);

          /**
           * @brief Required for node visitor.
           * 
           */
          void accept(nodeVisitor*) override;
     
};
/**
 * @brief Represents a recCreate production in the Tiger language. Describes the instantiation
 * of a record type.
 * 
 */
class recCreate : public node 
{
     public:
          /**
           * @brief The record type that is being instantiated.
           * 
           */
          node* tyId;

          /**
           * @brief List of field initializations.
           * 
           */
          std::vector<node*>* fields;

          /**
           * @brief Construct a new recCreate object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param tyId The record type that is being instantiated.
           * @param fields List of field initializations.
           */
          recCreate(const int &lineNumber, node* tyId, std::vector<node*>* fields);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
     
};
/**
 * @brief Represents fieldCreate production in the Tiger language.
 * 
 */
class fieldCreate : public node 
{
     public:
          /**
           * @brief The identifier of the field.
           * 
           */
          node* id;

          /**
           * @brief Expression to initialize the field to.
           * 
           */
          node* exp;

          /**
           * @brief Construct a new field Create object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param id The identifier of the field.
           * @param exp Expression to initialize the field to.
           */
          fieldCreate(const int &lineNumber, node* id, node* exp);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
     
};
/**
 * @brief Represents an assignemnt production in the Tiger language. Describes the assignment
 * of a value to another.
 * 
 */
class assignment : public node 
{
     public:
          /**
           * @brief The variable to operate on; could be a nested collection
           * of subscripts or other accesses but must come out to some field of a record,
           * subscript of an array, or normal variable.
           * 
           */
          node* lVal;

          /**
           * @brief The expression to assign to the left value.
           * 
           */
          node* exp;

          /**
           * @brief Construct a new assignment object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param lVal The variable to operate on.
           * @param exp The expression to assign to the left value.
           */
          assignment(const int &lineNumber, node* lVal, node* exp);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};
/**
 * @brief Represents an if/Then/Else expression in Tiger.
 * 
 */
class ifThenElse : public node 
{
     public:
          /**
           * @brief Expression for the condition.
           * 
           */
          node* ifExp;

          /**
           * @brief Expression to execute if condition is true.
           * 
           */
          node* thenExp;

          /**
           * @brief Expression to execute if condition is false.
           * 
           */
          node* elseExp;

          /**
           * @brief Construct a new if Then Else object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param ifExp Expression for the condition.
           * @param thenExp Expression to execute if condition is true.
           * @param elseExp Expression to execute if condition is false.
           */
          ifThenElse(const int &lineNumber, node* ifExp, node* thenExp, node* elseExp);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};

/**
 * @brief Represents a while loop in the Tiger language.
 * 
 */
class whileExp : public node 
{
     public:
          /**
           * @brief Expression for the loop condition.
           * 
           */
          node* condition;

          /**
           * @brief Expression to execute while the condition is true.
           * 
           */
          node* action;

          /**
           * @brief Construct a new while Exp object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param condition Expression for the loop condition.
           * @param action Expression to execute while the condition is true.
           */
          whileExp(const int &lineNumber, node* condition, node* action);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
     
};
/**
 * @brief Represents a for loop in the Tiger language.
 * 
 */
class forExp : public node 
{
     public:
          /**
           * @brief Identifier for the loop sentinel.
           * 
           */
          node* id;

          /**
           * @brief Value to assign to loop sentinel.
           * 
           */
          node* assign;

          /**
           * @brief Expression that is the upper bound of the loop.
           * 
           */
          node* condition;

          /**
           * @brief Expression to execute each run of the loop.
           * 
           */
          node* action;

          /**
           * @brief Construct a new for Exp object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param id Identifier for the loop sentinel.
           * @param assign Value to assign to loop sentinel.
           * @param condition Expression that is the upper bound of the loop.
           * @param action Expression to execute each run of the loop.
           */
          forExp(const int &lineNumber, node* id, node* assign, node* condition, node* action);
          void accept(nodeVisitor*) override;
     
};
/**
 * @brief Describes a Let/In/End expression for the Tiger language.
 * 
 */
class letExp : public node 
{
     public:
          /**
           * @brief All of the type,variable, and function definitions for the
           * current block's scope.
           * 
           */
          std::vector<node*>* decs;

          /**
           * @brief All of the expressions to be executed in the block.
           * 
           */
          std::vector<node*>* exps;

          /**
           * @brief Construct a new let Exp object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param decs All of the type,variable, and function definitions for the
           * current block's scope.
           * @param exps All of the expressions to be executed in the block.
           */
          letExp(const int &lineNumber, std::vector<node*>* decs, std::vector<node*>* exps);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};

/*************/
/* TERMINALS */
/*************/

/**
 * @brief Represents an Identifier terminal.
 * 
 */
class NId : public node
{
     
     public:
          /**
           * @brief Name of the identifier.
           * 
           */
          string name;
          
          /**
           * @brief Construct a new NId object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param value Name of the identifier.
           */
          NId(const int &lineNumber, string* value);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};
/**
 * @brief Represent a TyId (Type Identifier) terminal.
 * 
 */
class NTyId : public node
{
 
     public:
          /**
           * @brief Name of the type identifier.
           * 
           */
          string name;

          /**
           * @brief Construct a new NTyId object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param value Name of the type identifier.
           */
          NTyId(const int &lineNumber, string* value);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override; 
};
/**
 * @brief Represents a integer literal terminal.
 * 
 */
class NIntLit : public node
{
 
     public:
          /**
           * @brief The integer value of the node.
           * 
           */
          int val;

          /**
           * @brief Construct a new NIntLit object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param value The integer value of the node.
           */
          NIntLit(const int &lineNumber, int value);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};
/**
 * @brief Represents a string literal terminal.
 * 
 */
class NStrLit : public node
{
 
     public:
          /**
           * @brief The string literal this node represents.
           * 
           */
          string val;

          /**
           * @brief Construct a new NStrLit object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           * @param value The string literal this node represents.
           */
          NStrLit(const int &lineNumber, string* value);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override; 
};

/**
 * @brief Represents a break in a loop.
 * 
 */
class NBreak : public node
{
     public:
          /**
           * @brief Construct a new NBreak object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           */
          NBreak(const int &lineNumber);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};
/**
 * @brief Represents the nil terminal.
 * 
 */
class NNil : public node
{
     public:
          /**
           * @brief Construct a new NNil object
           * 
           * @param lineNumber The line number of the code bit that the node represents
           */
          NNil(const int &lineNumber);

          /**
           * @brief Required for visitor pattern.
           * 
           */
          void accept(nodeVisitor*) override;
};

/************************
 *   NODE PRINTER
 * *********************/

/**
 * @brief A node visitor focused on visiting every node and printing out
 * what kind of node it is. Primarily used for debugging the AST and parse tree.
 * 
 */
class nodePrinter : public nodeVisitor {
     public:
     /**
      * @brief Prompts a node in the tree to trigger the visit function for its class.
      * 
      * @param Node 
      */
     void print(node* Node);
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
};
/** @} */
#endif
