#include "ast.h"
#include <iostream>

/************
 * node
 * *********/

node::node(const int &lineNumber):lineNumber(lineNumber){} 

/********************************************
 * prog node
 * ******************************************/
program::program(const int &lineNumber, node* inNode):node(lineNumber), Node(inNode){}

void program::accept(nodeVisitor* visitor){
     visitor->visitProgram(this);
}

/********************************************
 * subscript node
 * ******************************************/
subscript::subscript(const int &lineNumber, node* pLValue, node* pExp):node(lineNumber), lValue(pLValue),exp(pExp){}

void subscript::accept(nodeVisitor* visitor){
     visitor->visitSubscript(this);
}

/********************************************
 * fieldExp node
 * ******************************************/
fieldExp::fieldExp(const int &lineNumber, node* pLValue, node* ID):node(lineNumber), lValue(pLValue),ID(ID){}

void fieldExp::accept(nodeVisitor* visitor){
     visitor->visitFieldExp(this);
}

/********************************************
 * seqExp node
 * ******************************************/
seqExp::seqExp(const int &lineNumber, std::vector<node*>* exps):node(lineNumber), exps(exps){}

void seqExp::accept(nodeVisitor* visitor){
     visitor->visitSeqExp(this);
}

/********************************************
 * negation node
 * ******************************************/
negation::negation(const int &lineNumber, node* operand):node(lineNumber), operand(operand){}

void negation::accept(nodeVisitor* visitor){
     visitor->visitNegation(this);
}

/********************************************
 * callExp node
 * ******************************************/
callExp::callExp(const int &lineNumber, node* id, std::vector<node*>* exps):node(lineNumber), id(id),exps(exps){}

void callExp::accept(nodeVisitor* visitor){
     visitor->visitCallExp(this);
}

/********************************************
 * inFix node
 * ******************************************/
infixExp::infixExp(const int &lineNumber, Operator op, node* leftNode, node* rightNode):node(lineNumber), op(op),leftNode(leftNode),rightNode(rightNode){}

void infixExp::accept(nodeVisitor* visitor){
     visitor->visitInfixExp(this);
}

/********************************************
 * arrCreate node
 * ******************************************/
arrCreate::arrCreate(const int &lineNumber, node* tyId, node* subscriptExp, node* postExp):
node(lineNumber), tyId(tyId),subscriptExp(subscriptExp),postExp(postExp){}

void arrCreate::accept(nodeVisitor* visitor){
     visitor->visitArrCreate(this);
}

/********************************************
 * recCreate node
 * ******************************************/
recCreate::recCreate(const int &lineNumber, node* tyId, std::vector<node*>* fields):node(lineNumber), tyId(tyId),fields(fields){}

void recCreate::accept(nodeVisitor* visitor){
     visitor->visitRecCreate(this);
}

/********************************************
 * fieldCreate node
 * ******************************************/
fieldCreate::fieldCreate(const int &lineNumber, node* id, node* exp):node(lineNumber), id(id),exp(exp){}

void fieldCreate::accept(nodeVisitor* visitor){
     visitor->visitFieldCreate(this);
}

/********************************************
 * assignment node
 * ******************************************/
assignment::assignment(const int &lineNumber, node* lVal, node* exp):node(lineNumber), lVal(lVal),exp(exp){}

void assignment::accept(nodeVisitor* visitor){
     visitor->visitAssignment(this);
}

/********************************************
 * iFThenElse node
 * ******************************************/
ifThenElse::ifThenElse(const int &lineNumber, node* ifExp, node* thenExp, node* elseExp):
node(lineNumber), ifExp(ifExp),thenExp(thenExp), elseExp(elseExp){}

void ifThenElse::accept(nodeVisitor* visitor){
     visitor->visitIfThenElse(this);
}

/********************************************
 * whileExp node
 * ******************************************/
whileExp::whileExp(const int &lineNumber, node* condition, node* action):node(lineNumber), condition(condition),action(action){}

void whileExp::accept(nodeVisitor* visitor){
     visitor->visitWhileExp(this);
}

/********************************************
 * forExp node
 * ******************************************/
forExp::forExp(const int &lineNumber, node* id, node* assign, node* condition, node* action):
node(lineNumber), id(id), assign(assign),condition(condition),action(action){}

void forExp::accept(nodeVisitor* visitor){
     visitor->visitForExp(this);
}

/********************************************
 * letExp node
 * ******************************************/
letExp::letExp(const int &lineNumber, std::vector<node*>* decs, std::vector<node*>* exps):node(lineNumber), decs(decs),exps(exps){}

void letExp::accept(nodeVisitor* visitor){
     visitor->visitLetExp(this);
}

/********************************************
 * dec node
 * ******************************************/
decc::decc(const int &lineNumber):node(lineNumber){}

void decc::accept(nodeVisitor* visitor){
     visitor->visitDec(this);
}

/********************************************
 * tyDec node
 * ******************************************/
tyDec::tyDec(const int &lineNumber, node* tyId, node* tyDef):decc(lineNumber), tyDef(tyDef)
{
     this->id = tyId;
     kind = D_TY;
}

void tyDec::accept(nodeVisitor* visitor){
     visitor->visitTyDec(this);
}

/********************************************
 * tyDef node
 * ******************************************/
tyDef::tyDef(const int &lineNumber):node(lineNumber){}

void tyDef::accept(nodeVisitor* visitor){
     visitor->visitTyDef(this);
}

TypeKind tyDef::kindToTypeKind()
{
     switch(kind)
     {
          case DEF_REF:
               return T_REF;
          case DEF_ARR:
               return T_ARR;
          case DEF_REC:
               return T_REC;
     };
}

/********************************************
 * refTy node
 * ******************************************/
refTy::refTy(const int &lineNumber, node* tyId):tyDef(lineNumber), tyId(tyId)
{
     this->tyId = tyId;
     kind = DEF_REF;
}

void refTy::accept(nodeVisitor* visitor){
     visitor->visitRefTy(this);
}

/********************************************
 * arrTy node
 * ******************************************/
arrTy::arrTy(const int &lineNumber, node* tyId):tyDef(lineNumber), tyId(tyId)
{

     kind = DEF_ARR;
}

void arrTy::accept(nodeVisitor* visitor){
     visitor->visitArrTy(this);
}

/********************************************
 * recTy node
 * ******************************************/
recTy::recTy(const int &lineNumber, std::vector<node*>* fieldDecs):tyDef(lineNumber), fieldDecs(fieldDecs)
{
     kind = DEF_REC;
}

void recTy::accept(nodeVisitor* visitor){
     visitor->visitRecTy(this);
}

/********************************************
 * fieldDec node
 * ******************************************/
fieldDec::fieldDec(const int &lineNumber, node* id, node* tyId):node(lineNumber), id(id), tyId(tyId){}

void fieldDec::accept(nodeVisitor* visitor){
     visitor->visitFieldDec(this);
}

pair< string, Type*> fieldDec::FieldToPair()
{
     string name = ((NId*) id)->name;
     Type* ty = tyId->type;
     return pair<string,Type*>(name, ty);
}

/********************************************
 * funDec node
 * ******************************************/
funDec::funDec(const int &lineNumber, node* id, std::vector<node*>* params, node* returnType, node* exp):
decc(lineNumber), params(params),returnType(returnType), exp(exp)
{
     this->id = id;
     kind = D_FUNC;
}

void funDec::accept(nodeVisitor* visitor){
     visitor->visitFunDec(this);
}

vector< pair<string,Type*> > funDec::getParams()
{
     vector< pair<string,Type*> > allParams;
     for(int i = 0; i < params->size(); i++)
     {
          fieldDec* param = (fieldDec*)((*params)[i]);
          string name = ((NId*)(param->id))->name;
          allParams.push_back(pair<string,Type*>(name,(*params)[i]->type));
     }
     return allParams;
}
/*
void funDec::AddToScope(Scope* scope)
{
     string name = ((NId*) id)->name;

     FuncSymbol* sym = new FuncSymbol(id->name, SYM_FUNC, name,);
     scope.AddSymbol()
}*/

/********************************************
 * varDec node
 * ******************************************/
varDec::varDec(const int &lineNumber, node* id, node* tyId, node* exp):
decc(lineNumber), tyId(tyId),exp(exp){
     this->id = id;
     kind = D_VAR;
}

void varDec::accept(nodeVisitor* visitor){
     visitor->visitVarDec(this);
}

/********************************************
 * break node
 * ******************************************/
NBreak::NBreak(const int &lineNumber):node(lineNumber){}

void NBreak::accept(nodeVisitor* visitor){
     visitor->visitBreak(this);
}

/********************************************
 * nil node
 * ******************************************/
NNil::NNil(const int &lineNumber):node(lineNumber){}

void NNil::accept(nodeVisitor* visitor){
     visitor->visitNil(this);
}

/********************************************
 * ID node
 * ******************************************/
NId::NId(const int &lineNumber, string* value):node(lineNumber), name(*value){
}

void NId::accept(nodeVisitor* visitor){
     visitor->visitID(this);
}

/********************************************
 * TYID node
 * ******************************************/
NTyId::NTyId(const int &lineNumber, string* value):node(lineNumber), name(*value){
}

void NTyId::accept(nodeVisitor* visitor){
     visitor->visitTyID(this);
}

/********************************************
 * INTLIT node
 * ******************************************/
NIntLit::NIntLit(const int &lineNumber, int value):node(lineNumber), val(value){
}

void NIntLit::accept(nodeVisitor* visitor){
     visitor->visitIntLit(this);
}

/********************************************
 * STRLIT node
 * ******************************************/
NStrLit::NStrLit(const int &lineNumber, string* value):node(lineNumber){
     //Process the string a wee bit to fix'er up
     string inString = *value;
     inString = inString.substr(1, inString.size()-2); //Cleave off the quotes
     inString += "\0"; //Add a null terminator since I don't think the lexer adds one
     val = inString.data();
}

void NStrLit::accept(nodeVisitor* visitor){
     visitor->visitStrLit(this);
}

/*******************************************
 * NODE PRINTER
 * ****************************************/

void nodePrinter::print(node* Node){
     Node->accept(this);
}
void nodePrinter::visitProgram(program* Program)
{
     std::cout << "( Program )\n";
     print(Program->Node);
     std::cout << "\n\n";
}
void nodePrinter::visitBreak(NBreak* Break)
{
     std::cout << "( Break )\n";
}
void nodePrinter::visitNil(NNil* Nil)
{
     std::cout << "( Nil )";
}
void nodePrinter::visitID(NId* id)
{
     std::cout << "( Identifier: " << id->name << " )";
}
void nodePrinter::visitTyID(NTyId* tyid)
{
     std::cout << "( Type Id: " << tyid->name << " )";
}
void nodePrinter::visitIntLit(NIntLit* intLit)
{
     std::cout << "( INTLIT[" << intLit->value << "])";
}
void nodePrinter::visitStrLit(NStrLit* strLit)
{
     std::cout << "( STRLIT[" << strLit->value << "])";
}
void nodePrinter::visitSubscript(subscript* Subscript)
{
     print(Subscript->lValue);
     std::cout << "( Subscript: ";
     print(Subscript->exp);
     std::cout << ")";
}
void nodePrinter::visitFieldExp(fieldExp* FieldExp)
{
     print(FieldExp->lValue);
     std::cout << "( FieldExp: ";
     print(FieldExp->ID);
     std::cout << ")";
}

void nodePrinter::visitSeqExp(seqExp* SeqExp)
{
     for(int i = 0; i < SeqExp->exps->size(); i++)
     {
          print((*(SeqExp->exps))[i]);

     }

     std::cout << ")\n";
}

void nodePrinter::visitNegation(negation* neg)
{
     std::cout << "Negation (";
     print(neg->operand);
     std::cout << ")";
}

void nodePrinter::visitCallExp(callExp* CallExp)
{
     std::cout << "( CallExp ";
     print(CallExp->id);
     for(int i = 0; i < CallExp->exps->size(); i++)
     {
          print((*(CallExp->exps))[i]);
          std::cout << "\n";
     }

     std::cout << ")\n";
}

//Copied from Dr. Schwesinger because I'm lazy
void nodePrinter::visitInfixExp(infixExp* InfixExp) {
    std::cout << "( INFIX: ";
    switch (InfixExp->op) {
        case OP_ADD:
            std::cout << "ADD ";
            break;
        case OP_SUBTRACT:
            std::cout << "SUBTRACT ";
            break;
        case OP_MULTIPLY:
            std::cout << "MULTIPLY ";
            break;
        case OP_DIVIDE:
            std::cout << "DIVIDE ";
            break;
        case OP_OR:
            std::cout << "OR ";
            break;
        case OP_AND:
            std::cout << "AND ";
            break;
        case OP_EQ:
            std::cout << "EQ ";
            break;
        case OP_NEQ:
            std::cout << "NEQ ";
            break;
        case OP_LT:
            std::cout << "LT ";
            break;
        case OP_LEQ:
            std::cout << "LEQ ";
            break;
        case OP_GT:
            std::cout << "GT ";
            break;
        case OP_GEQ:
            std::cout << "GEQ ";
            break;
        default:
            std::cout << "UNKNOWN ";
            break;
    }
    print(InfixExp->leftNode);
    std::cout << " ";
    print(InfixExp->rightNode);
    std::cout << " )";
}

void nodePrinter::visitArrCreate(arrCreate* ArrCreate)
{
     std::cout << "( ArrCreate: ";
     print(ArrCreate->tyId);
     std::cout << "[";
     print(ArrCreate->subscriptExp);
     std::cout<< "] ";
     print(ArrCreate->postExp);
     std::cout << ")";
}

void nodePrinter::visitRecCreate(recCreate* RecCreate)
{
     std::cout << "( RecCreate: ";
     print(RecCreate->tyId);
     std::cout << "{";
     for(int i = 0; i < RecCreate->fields->size(); i++)
     {
          print((*(RecCreate->fields))[i]);
          std::cout << "\n";
     }
     std::cout << "})";
}

void nodePrinter::visitFieldCreate(fieldCreate* FieldCreate)
{
     std::cout << "( FieldCreate: ";
     print(FieldCreate->id);
     std::cout << " = ";
     print(FieldCreate->exp);
     std::cout << ")";
}

void nodePrinter::visitAssignment(assignment* Assign)
{
     std::cout << "( Assignment: ";
     print(Assign->lVal);
     std::cout << " = ";
     print(Assign->exp);
     std::cout << ")";
}

void nodePrinter::visitIfThenElse(ifThenElse* iTE)
{
     std::cout << "( If: ";
     print(iTE->ifExp);
     std::cout << "\nThen: ";
     print(iTE->thenExp);
     if(iTE->elseExp != NULL)
     {
          std::cout << "\nElse: ";
          print(iTE->elseExp);
     }
     std::cout << ")";   
}

void nodePrinter::visitWhileExp(whileExp* While)
{
     std::cout << "( While: ";
     print(While->condition);
     std::cout << "\nDo: ";
     print(While->action);
     std::cout << ")";   
}

void nodePrinter::visitForExp(forExp* forEx)
{
     std::cout << "( For: ";
     print(forEx->id);
     std::cout << " = ";
     print(forEx->assign);
     std::cout << "\nFor(condition): ";
     print(forEx->condition);
     std::cout << "\nFor(action): ";
     print(forEx->action);
     std::cout << ")";   
}

void nodePrinter::visitLetExp(letExp* LetExp)
{
     std::cout << "( LetExp:\nLET: \n";
     for(int i = 0; i < LetExp->decs->size(); i++)
     {
          print((*(LetExp->decs))[i]);
          std::cout << "\n";
     }
          std::cout << "IN: \n";
     for(int i = 0; i < LetExp->exps->size(); i++)
     {
          print((*(LetExp->exps))[i]);
          std::cout << "\n";
     }

     std::cout << ")\n";
}

void nodePrinter::visitDec(decc* Dec)
{
     std::cout << "Dec";
}

void nodePrinter::visitTyDec(tyDec* TyDec)
{
     std::cout << "( TyDec: ";
     print(TyDec->id);
     std::cout << " = ";
     print(TyDec->tyDef); 
}

void nodePrinter::visitTyDef(tyDef* TyDef)
{
     std::cout << "TyDef";
}

void nodePrinter::visitRefTy(refTy* RefTy)
{
     std::cout << "( RefTy: ";
     print(RefTy->tyId);
}

void nodePrinter::visitArrTy(arrTy* ArrTy)
{
     std::cout << "( ArrTy: ";
     print(ArrTy->tyId);
}

void nodePrinter::visitRecTy(recTy* RecTy)
{
     std::cout << "( RecTy: ";
     for(int i = 0; i < RecTy->fieldDecs->size(); i++)
     {
          print((*(RecTy->fieldDecs))[i]);
          std::cout << "\n";
     }
     std::cout << ")";
}

void nodePrinter::visitFieldDec(fieldDec* FieldDec)
{
     std::cout << "( FieldDec: ";
     print(FieldDec->id);
     std::cout << " : ";
     print(FieldDec->tyId);
     std::cout << ")";
}

void nodePrinter::visitFunDec(funDec* FunDec)
{
     std::cout << "( FunDec: ";
     print(FunDec->id);
     std::cout << "(";
     for(int i = 0; i < FunDec->params->size(); i++)
     {
          print((*(FunDec->params))[i]);
          std::cout << "\n";
     }
     std::cout << ") : ";
     print(FunDec->returnType);
     std::cout << " = ";
     print(FunDec->exp);
}

void nodePrinter::visitVarDec(varDec* VarDec)
{
     std::cout << "( VarDec: ";
     print(VarDec->id);
     std::cout << " : ";
     print(VarDec->tyId);
     std::cout << " = ";
     print(VarDec->exp);
}

