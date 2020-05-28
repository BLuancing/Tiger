#include <string>
#include <vector>
#include "ast.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"

using namespace std;

SemanticAnalyzer::SemanticAnalyzer(node* astRoot):astRoot(astRoot)
{

}

void SemanticAnalyzer::AnalyzeTree()
{
     nodeSAChecker scopeChecker;
     //cout << "Conducting semantic analysis..." << endl;
     scopeChecker.check(astRoot);
     //cout << "Semantic analysis successful..." << endl;
}

/*********************
 * NODE SCOPE CHECKER
 * *******************/

nodeSAChecker::nodeSAChecker()
{

     table.DefaultTigerTable();
}

void nodeSAChecker::check(node* Node){
     Node->accept(this);
}

void nodeSAChecker::throwError(const int &lineNumber,const string &errorMessage)
{
     cout << "ERROR: " << lineNumber << ": Semantic:  " << errorMessage << endl;
     exit(3);
}

bool nodeSAChecker::isArrayType(Type* type)
{
     if(type->GetActualKind() == T_ARR)
          return true;
     else
          return false;
}

bool nodeSAChecker::isArray(node* Node)
{
     if(Node->type->GetActualKind() == T_ARR)
          return true;
     else
          return false;
}

bool nodeSAChecker::isInteger(node* Node)
{
     if(Node->type->GetActualType()->name == "int")
          return true;
     else
          return false;
}

bool nodeSAChecker::isUnit(node* Node)
{
     if(Node->type->GetActualType()->name == "unit")
          return true;
     else
          return false;
}

bool nodeSAChecker::isRecordType(Type* type)
{
     if(type->GetActualKind() == T_REC)
          return true;
     else
          return false;
}

bool nodeSAChecker::isRecord(node* Node)
{
     if(Node->type->GetActualKind() == T_REC)
          return true;
     else 
          return false;
}

bool nodeSAChecker::sameType(node* left, node* right)
{
     if(left->type->GetActualType() == right->type->GetActualType())
          return true;
     else
          return false;
}

bool nodeSAChecker::legalArguments(FuncSymbol* func, callExp* CallExp)
{
     vector< pair<string,Type*> > args = func->args;
     vector<node*>* calledArgs =  CallExp->exps; 

     //For every argument, see if we can find a matching type entry for them  
     for(int i = 0; i < args.size(); i++)
     {
          //cout << "Intended argument name: " << args[i].second->name << endl << " on line " << CallExp->lineNumber;
          //cout << "Passed argument type name: " << (*calledArgs)[i]->type->name << endl;
          if(!(args[i].second->GetActualType()->name == (*calledArgs)[i]->type->GetActualType()->name))
          {
               throwError(CallExp->lineNumber, "Function argument type mismatch for function '" + func->name + ". Positional argument " + to_string(i) + " expecting type '" + args[i].second->name 
               + "' but received type '" + (*calledArgs)[i]->type->name + "'.");
          }
     }

     return true;
}

bool nodeSAChecker::isAssignableTo(node* Node, Type* type)
{
     Type* unitType = table.LookupType("unit");
     switch(Node->type->kind)
     {
          case T_REF:
               break;

     };

     //Make a special case because blah blah it's due in two
     // days and I'm sick of this language

     //It's because of how I did reference types, idk this IS SUPER ANNOYING TO FIX
     // so we're just slapping a bandage on this nonsense
     if((Node->type->kind == T_ARR && type->kind == T_ARR) && (Node->type->name != type->name))
          return false;

     //If the types match OR the Node's type is unit and the incoming type is a record
     if((Node->type->GetActualType()== type->GetActualType()) || (Node->type->kind == T_REC && type == unitType))
     {
          return true;
     }
     else
          return false;
}

/*************
 * visits
 * **********/

void nodeSAChecker::visitProgram(program* Program)
{
     check(Program->Node);
}
void nodeSAChecker::visitBreak(NBreak* Break)
{
     Scope* scope = table.PopScope();
     //If there was no scope at all or the current scope source was a let or function...
     if(scope == NULL || (scope->source == S_LET || scope->source == S_FUNC))
     {
          throwError(Break->lineNumber, "'Break' called outside of WHILE or FOR expression.");
     }

     Break->type = table.LookupType("unit");
}
void nodeSAChecker::visitNil(NNil* Nil)
{
    Nil->type = table.LookupType("unit");
}
//Only called when scope has to be verified for an ID
void nodeSAChecker::visitID(NId* id)
{
     //Check if it exists anywhere in the program
     Symbol* sym = table.LookupSymbol(id->name);
     if(sym == NULL)
     {
          throwError(id->lineNumber, "No such symbol with name '" + id->name + "' found in current scope.");
     }

     id->type = sym->type;
}
//Only called when scope has to be verified for an ID
void nodeSAChecker::visitTyID(NTyId* tyid)
{
     Type* ty = table.LookupType(tyid->name);
     if(ty == NULL)
     {
          throwError(tyid->lineNumber, "No such type with name '" + tyid->name + "' found in current scope.");
     }

     tyid->type = ty;
}
void nodeSAChecker::visitIntLit(NIntLit* intLit)
{
     Type* ty = table.LookupType("int");
     if(ty == NULL)
     {
          throwError(intLit->lineNumber, "Int type missing from table?");
     }
     intLit->type = ty;
}
void nodeSAChecker::visitStrLit(NStrLit* strLit)
{
     Type* ty = table.LookupType("string");
     if(ty == NULL)
     {
          throwError(strLit->lineNumber, "String type missing from table?");
     }
     strLit->type = ty;
}
void nodeSAChecker::visitSubscript(subscript* Subscript)
{
     check(Subscript->lValue);
     if(!isArray(Subscript->lValue))
          throwError(Subscript->lValue->lineNumber, "Invalid type. Subscript cannot be performed on type other than array.");
     check(Subscript->exp);
     if(!isInteger(Subscript->exp))
          throwError(Subscript->exp->lineNumber, "Invalid type. Subscript expression evaluate to int.");
     Subscript->type = Subscript->lValue->type->GetActualType();
}
void nodeSAChecker::visitFieldExp(fieldExp* FieldExp)
{
     //Make sure this is even a valid record first
     check(FieldExp->lValue);
     if(!isRecord(FieldExp->lValue))
          throwError(FieldExp->lValue->lineNumber, "Cannot access member on non-record type.");
     
     //Then make sure this is even a member of that type
     RecType* typ = (RecType*) FieldExp->lValue->type;
     NId* id = (NId*) FieldExp->ID;
     //If the record type actually has a member with this name...
     if(typ->hasMember(id->name))
     {
          pair<string,Type*> result = typ->getFieldPair(id->name);
          FieldExp->type = result.second;
     }
     else
          throwError(FieldExp->lineNumber, "Record type '" + typ->name + "' has no member named " + id->name);
}

void nodeSAChecker::visitSeqExp(seqExp* SeqExp)
{
     for(int i = 0; i < SeqExp->exps->size(); i++)
     {
          check((*(SeqExp->exps))[i]);
     }

     //Set the type of sequence to the last expression or unit
     if(SeqExp->exps->size() > 0)
     {
          SeqExp->type = (*(SeqExp->exps))[SeqExp->exps->size()-1]->type;
     }
     else
          SeqExp->type = table.LookupType("unit");
}

void nodeSAChecker::visitNegation(negation* neg)
{
     check(neg->operand);
     if(!isInteger(neg->operand))
          throwError(neg->lineNumber, "Negation operand must be of type int.");
     else
          neg->type = table.LookupType("int");
}

void nodeSAChecker::visitCallExp(callExp* CallExp)
{
     //Verify the symbol for the function exists
     Symbol* sym = table.LookupSymbol(((NId*) CallExp->id)->name);
     if(sym == NULL)
          throwError(CallExp->lineNumber, "No function with name '" + ((NId*) CallExp->id)->name + "' found.");
     
     //Verify that it's even a function name
     if(sym->kind != SYM_FUNC)
          throwError(CallExp->lineNumber, "'" + ((NId*) CallExp->id)->name + "' is not a function identifier.");

     //Make sure # of arguments match
     FuncSymbol* fsym = (FuncSymbol*) sym;
     if(CallExp->exps->size() != fsym->args.size())
          throwError(CallExp->lineNumber, "Incorrect number of arguments to function call. Expecting " + fsym->args.size());
     
     //Check each of the sub expressions for the args first
     for(int i = 0; i < CallExp->exps->size(); i++)
     {
          check((*(CallExp->exps))[i]);
     }

     //Verify that the args match those expected by the function
     if(!legalArguments(fsym, CallExp))
          throwError(CallExp->lineNumber, "Argument mismatch. Verify type and position of arguments for function call.");
     
     //Set the type of this node
     CallExp->type = fsym->type;
}

//Copied from Dr. Schwesinger because I'm lazy
void nodeSAChecker::visitInfixExp(infixExp* InfixExp) {
     check(InfixExp->leftNode);
     check(InfixExp->rightNode);
     switch (InfixExp->op) {
          case OP_ADD:
          case OP_SUBTRACT:
          case OP_MULTIPLY:
          case OP_DIVIDE:
          case OP_OR:
          case OP_AND:
               if(!isInteger(InfixExp->leftNode))
                    throwError(InfixExp->leftNode->lineNumber, "Left operand not of type int.");
               if(!isInteger(InfixExp->rightNode))
                    throwError(InfixExp->rightNode->lineNumber, "Right operand not of type int.");
               break;
        case OP_EQ:
        case OP_NEQ:
        case OP_LT:
        case OP_LEQ:
        case OP_GT:
        case OP_GEQ:
               if(!isAssignableTo(InfixExp->leftNode, InfixExp->rightNode->type->GetActualType()))
                    throwError(InfixExp->lineNumber, "Infix Operands not of same type.");
               break;
        default:
               throwError(InfixExp->lineNumber, "Unexpected operator...wait, what???");
               break;
    }

     InfixExp->type = table.LookupType("int");
}

void nodeSAChecker::visitArrCreate(arrCreate* ArrCreate)
{
     //Verify type exists in scope first, then set its type
     check(ArrCreate->tyId);
     /*Type* ty = table.LookupType(((NTyId*) ArrCreate->tyId)->name);
     if(ty == NULL)
          throwError(ArrCreate->lineNumber, "No type with name '" + ((NTyId*) ArrCreate->tyId)->name + "' found.");
     */
     ArrCreate->type = ArrCreate->tyId->type;

     //Verify that the type is even an array type
     if(!isArrayType(ArrCreate->type))
          throwError(ArrCreate->lineNumber, "Type '" + ArrCreate->type->name + "' is not an array type.");
     


     //Check the size subscript
     check(ArrCreate->subscriptExp);
     if(!isInteger(ArrCreate->subscriptExp))
          throwError(ArrCreate->lineNumber, "Subscript cannot be of type '" + ArrCreate->subscriptExp->type->name + "'; must evaluate to integer type.");
     
     //Make sure the exp it fills with is okay
     check(ArrCreate->postExp);

     //Make sure the expression can even be assigned to the array type
     if(!sameType(ArrCreate->tyId, ArrCreate->postExp))
          throwError(ArrCreate->lineNumber, "Array type and initializing expression type differ.");

}

void nodeSAChecker::visitRecCreate(recCreate* RecCreate)
{
     //Verify type exists in scope first, then set its type
     Type* ty = table.LookupType(((NTyId*) RecCreate->tyId)->name);
     if(ty == NULL)
          throwError(RecCreate->lineNumber, "No type with name '" + ((NTyId*) RecCreate->tyId)->name + "' found.");
     RecCreate->type = ty;

     //Verify that it's even a record type
     if(!isRecordType(ty))
          throwError(RecCreate->lineNumber, "Type '" + ty->name + "' is not a record type.");
     
     //Verify that all of its fields are semantically correct
     RecType* rectype = (RecType*) ty;
     for(int i = 0; i < RecCreate->fields->size(); i++)
     {
          fieldCreate* fieldNode = (fieldCreate*)((*RecCreate->fields)[i]);

          //Make sure the record even has the field that the fieldCreate is using
          if(!rectype->hasMember(((NId*)fieldNode->id)->name))
               throwError(fieldNode->lineNumber, "Record of type '" + rectype->name + "' has no field with name '" + ((NId*)fieldNode->id)->name + "'.");

          //Get the field
          check(fieldNode); //Verify the types of each field node
          pair<string,Type*> field = rectype->getFieldPair(((NId*)fieldNode->id)->name);
          fieldNode->id->type = field.second; //Set the type here cause I can't in the fieldCreate node itself
          

          //Make sure types match for fields to value
          if(!isAssignableTo(fieldNode->id, field.second))
               throwError(fieldNode->lineNumber, "Field assigned type of '" + fieldNode->type->name + "'; expected '" + ty->name + "'");
          
          fieldNode->type = fieldNode->id->type;
    
     }
}

void nodeSAChecker::visitFieldCreate(fieldCreate* FieldCreate)
{ 
     //Note: don't do anything about ID or own type because
     // recCreate handles that for this node because reasons
     check(FieldCreate->exp);
}

void nodeSAChecker::visitAssignment(assignment* Assign)
{
     check(Assign->lVal);
     check(Assign->exp);
     if(!isAssignableTo(Assign->lVal, Assign->exp->type->GetActualType()))
          throwError(Assign->lVal->lineNumber, "Left operand of type '" + Assign->lVal->type->name + "' does not match assigned type of '" + Assign->exp->type->name + "'.");

     Assign->type = table.LookupType("unit");
}

void nodeSAChecker::visitIfThenElse(ifThenElse* iTE)
{
     //Make sure condition is intp 
     check(iTE->ifExp);
     if(!isInteger(iTE->ifExp))
          throwError(iTE->ifExp->lineNumber, "Result of expression is of type '" + iTE->ifExp->type->name + "', not of type 'int'.");
     
     //Run check of else block
     check(iTE->thenExp);

     //If there is an else expression...
     if(iTE->elseExp != NULL)
     {
          //Have to make sure the else and then expressions match type
          check(iTE->elseExp);
          if(!sameType(iTE->thenExp,iTE->elseExp))
               throwError(iTE->lineNumber, "Else expression is of type '" + iTE->elseExp->type->name + "'; must match Then expression of type '" + iTE->thenExp->type->name + "'.");
     }
     else
     {
          //Else, we have to make sure the then is unit type
          if(!isUnit(iTE->thenExp))
               throwError(iTE->lineNumber, "Then expression is of type '" + iTE->thenExp->type->name + "'; must be of type 'unit' if there is no else block.");
     }

     //Set overall expression type to Then's type
     iTE->type = iTE->thenExp->type;
}

void nodeSAChecker::visitWhileExp(whileExp* While)
{
     check(While->condition);
     if(!isInteger(While->condition))
          throwError(While->condition->lineNumber, "While condition is of type '" + While->condition->type->name + "'; must be of type 'int'");

     //Add a new scope for within the loop
     table.PushScope(new Scope(table.top, S_WHILE));
     check(While->action);

     //Throw error if body isn't unit
     if(!isUnit(While->action))
     {
          throwError(While->action->lineNumber, "Body of while condition evaluates to type '" + While->action->type->name + "'; must be of type 'unit.'");
     }
}

void nodeSAChecker::visitForExp(forExp* forEx)
{
     /*
     //See if the ID used for the check is in scope
     check(forEx->id);

     //Make sure the identifier is an int type
     if(!isInteger(forEx->id))
          throwError(forEx->id->lineNumber, "Loop condition identifier is of type '" + forEx->id->type + "'; must be of type 'int'.");

     */

     //Make sure the assignment expression is an int type
     check(forEx->assign);
     
     if(!isInteger(forEx->assign))
          throwError(forEx->assign->lineNumber, "Loop initial expression is of type '" + forEx->assign->type->name + "'; ID must be assigned type 'int'.");

     //Set the type of the identifier now that it's been verified
     forEx->id->type = forEx->assign->type;

     //Make sure the condition expression is an int type
     check(forEx->condition);
     if(!isInteger(forEx->condition))
          throwError(forEx->condition->lineNumber, "Loop condition is of type '" + forEx->id->type->name + "'; Condition must be type 'int'.");

     //Create a new scope with the loop variable in it and push it
     Scope* loopScope = new Scope(table.top, S_FOR);
     loopScope->AddSymbol(new VarSymbol(((NId*)forEx->id)->name, SYM_VAR, table.LookupType("int"), true));
     table.PushScope(loopScope);

     //Check the actions for validation
     check(forEx->action); 
     forEx->type = table.LookupType("unit");
}

void nodeSAChecker::visitLetExp(letExp* LetExp)
{
     //Make a new scope to work with
     Scope* letScope = new Scope(table.top, S_LET);
     table.PushScope(letScope);

     //First, scan for any types in here add them to the scope
     for(int i = 0; i < LetExp->decs->size(); i++)
     {
          decc* dec = (decc*) (*(LetExp->decs))[i];
          
          if(dec->kind == D_TY)
          {
               //See if the ID is unique in the current scope
               string typeName = ((NTyId*)dec->id)->name;
               Type* unique = letScope->LookupType(typeName);
               //If it's not unique, that might be okay
               if(!(unique==NULL))
               {
                    //Have to make sure the dec before isn't a type dec with the same name
                    if(i >= 1) //if the dec isn't the first one
                    {
                         decc* previousDec = (decc*) (*(LetExp->decs))[i-1];     
                         //If the last dec is type a declaration & the type name matches this one
                         if(previousDec->kind == D_TY && ((NId*)(previousDec->id))->name == typeName)
                         {
                              throwError(dec->id->lineNumber, "Type with name '" + unique->name + "' already exists in scope.");
                         } 
                    }       
               }

               tyDec* TyDec = (tyDec*) dec; //Convert to type declaration 
               tyDef* TyDef = (tyDef*) TyDec->tyDef;

               //Construct the appropriate type based on kind
               // but some of their info will be blank; will fill in later
               Type* ty;
               refTy* RefTy; //C++ is annoying
               arrTy* ArrTy;
               recTy* RecTy;
               Type* tempRecType;
               map<string,Type*>* fields;
               switch(TyDef->kind)
               {
                    case DEF_REF:
                         RefTy = (refTy*) TyDef;
                         check(RefTy->tyId); //For safety's sake
                         ty = new RefType(((NTyId*)(dec->id))->name, T_REF, RefTy->tyId->type); //Use its definition's kind to make a temporary type
                         ty->name = ((NTyId*)(dec->id))->name;
                         //cout << "Name is : " << ty-name;
                         ty->kind = T_REF;
                         break;
                    case DEF_ARR:
                         ArrTy = (arrTy*) TyDef;
                         check(ArrTy->tyId); //For safety's sake
                         ty = new ArrType(((NTyId*)(dec->id))->name, T_ARR, ArrTy->tyId->type); //Use its definition's kind to make a temporary type
                         ty->name = ((NTyId*)(dec->id))->name;
                         ty->kind = T_ARR;
                         break;
                    case DEF_REC:
                         RecTy = (recTy*) TyDef;

                         fields = new map<string,Type*>();
                         tempRecType = new Type(((NTyId*)(dec->id))->name, T_REC);
                         letScope->AddType(tempRecType);
                         for(int i = 0; i < RecTy->fieldDecs->size(); i++)
                         {
                              fieldDec* field = (fieldDec*) (*(RecTy->fieldDecs))[i];
                              check(field);
                              pair<string,Type*> fieldPair = field->FieldToPair();
                              
                              fields->insert(fieldPair);
                         }
                         letScope->RemoveType(tempRecType); // //Kill the temporary type

                         ty = new RecType(((NTyId*)(dec->id))->name, T_REC, fields); //Use its definition's kind to make a temporary type
                         
                         ty->name = ((NTyId*)(dec->id))->name;
                         ty->kind = T_REC;
                         
                         break;
                    default:
                         cout << "Wait...what?" << endl;
                         break;
               };
               
               letScope->AddType(ty);
          }
     }
     //Second, scan for function definitions in order to support mutual recursion
     for(int i = 0; i < LetExp->decs->size(); i++)
     {
          decc* dec = (decc*) (*(LetExp->decs))[i];

          if(dec->kind == D_FUNC)
          {
              checkFunctionSignature((funDec*) dec);
          }
     }
     //Third, add all of these new functions to the scope
     // after confirming the signatures are semantically valid
     for(int i = 0; i < LetExp->decs->size(); i++)
     {
          decc* dec = (decc*) (*(LetExp->decs))[i];
          if(dec->kind == D_FUNC)
          {
               funDec* FunDec = (funDec*) dec; //Convert to function declaration  

               //Make sure there isn't already a function with the same name in the same scope
               Symbol* funcSym = letScope->LookupSymbol(((NId*)(FunDec->id))->name);
               if(!(funcSym == NULL))
                    throwError(FunDec->lineNumber, "Function with same name already declared in scope.");

               FuncSymbol* sym = new FuncSymbol(((NId*)FunDec->id)->name, SYM_FUNC, FunDec->type, FunDec->getParams()); //Use its definition's kind to make a temporary type
               //Get params works here because all the type should have been set before
               letScope->AddSymbol(sym);
          }
     }

     //Fourth, actually run semantic checks on everything
     for(int i = 0; i < LetExp->decs->size(); i++)
     {
          check((*(LetExp->decs))[i]);
     }

     //Fifth, check for recursive type cycles without record types
     //JK THATS TOO HARD

     //Sixth, analyze the body and set the type for the whole Let expression 
     for(int i = 0; i < LetExp->exps->size(); i++)
     {
          check((*(LetExp->exps))[i]);
     }
}

void nodeSAChecker::visitDec(decc* Dec)
{
    
}

void nodeSAChecker::visitTyDec(tyDec* TyDec)
{
     check(TyDec->tyDef);
     TyDec->type = table.LookupType((((NId*)(TyDec->id))->name)); 
}

void nodeSAChecker::visitTyDef(tyDef* TyDef)
{

}

void nodeSAChecker::visitRefTy(refTy* refTy)
{
     check(refTy->tyId); 
     //refTy->type = refTy->tyId->type;
}

void nodeSAChecker::visitArrTy(arrTy* ArrTy)
{
     check(ArrTy->tyId);
     ArrTy->type = ArrTy->tyId->type;
}

void nodeSAChecker::visitRecTy(recTy* RecTy)
{
     for(int i = 0; i < RecTy->fieldDecs->size(); i++)
     {
          check((*(RecTy->fieldDecs))[i]);
     }
}

void nodeSAChecker::visitFieldDec(fieldDec* FieldDec)
{
     check(FieldDec->tyId);
     FieldDec->type = FieldDec->tyId->type;
}

void nodeSAChecker::visitFunDec(funDec* FunDec)
{
     //At this point, there should already be a symbol for this in the table
     //Just need to make a scope that's appropriate for the function when entered
     // before checking the body.
     FuncSymbol* sym = (FuncSymbol*)table.LookupSymbol(((NId*)(FunDec->id))->name);

     //Generate a scope from its symbol table intro
     Scope* funcScope = sym->CreateScopeFromParams();
     funcScope -> last = table.top;
     table.PushScope(funcScope);

     //Verify all of its expressions are valid within the new scope
     check(FunDec->exp);

     //Make sure body type and return type match
     if(!isAssignableTo(FunDec->exp, FunDec->returnType->type))
          throwError(FunDec->lineNumber, "Type mismatch. Function body returns type '" + FunDec->exp->type->name + "' but returnType is '" + FunDec->returnType->type->name + "'.");

     //Pop the function scope when done
     table.PopScope();
}

void nodeSAChecker::visitVarDec(varDec* VarDec)
{
     //Verify the type
     check(VarDec->tyId);

     //Make sure we aren't overriding other variables in the same scope
     Scope* currentScope = table.top;
     VarSymbol* sym = (VarSymbol*)currentScope->LookupSymbol(((NId*)VarDec->id)->name);
     if((!(sym == NULL)) && (sym->type == VarDec->tyId->type)) //If the symbol is in the current scope already AND the types match
     {
          throwError(VarDec->lineNumber, "Symbol with name '" + sym->name + "' already exists in scope.");
     }

     //Set the identifier to the type
     VarDec->id->type = VarDec->tyId->type;

     //Verify the expression works
     check(VarDec->exp);

     //Make sure the expression and ID's type match
     //if(!sameType(VarDec->exp, VarDec->id))
     if(!isAssignableTo(VarDec->id, VarDec->exp->type))
          throwError(VarDec->lineNumber, "Type mismatch; identifer of type '" + VarDec->id->type->name + "' assigned type '" + VarDec->exp->type->name + "'.");

     //We can add it in now, and let it shadow other variables
     string name = ((NId*)(VarDec->id))->name;
     currentScope->AddSymbol(new VarSymbol(name, SYM_VAR, VarDec->tyId->type));
}

void nodeSAChecker::checkFunctionSignature(funDec* FunDec)
{
     //Semantic check all of the parameters
     for(int i = 0; i < FunDec->params->size(); i++)
     {
          check((*(FunDec->params))[i]);
     }

     //Check the return type too
     check(FunDec->returnType);
     Type* ty = table.LookupType(FunDec->returnType->type->name);
     FunDec->type = ty;

}