#include "Interpreter.h"

using namespace std;

Interpreter::Interpreter(node* astRoot):astRoot(astRoot){}

void Interpreter::Interpret()
{
     nodeInterpreter interpreter;
     interpreter.evaluate(astRoot);
}

/*********************
 * NODE INTERPRETER
 * *******************/

nodeInterpreter::nodeInterpreter()
{
     table.DefaultTigerTable();
}

void nodeInterpreter::evaluate(node* Node)
{
     Node->accept(this);
}

bool nodeInterpreter::isNonZero(Value* val)
{
     IntValue* intVal = (IntValue*) (val);
     if(intVal->GetValue() != 0)
          return true;
     else
          return false;
}


void nodeInterpreter::Print(IntValue* value)
{
     cout << value->GetValue();
}

void nodeInterpreter::Print(StringValue* value)
{
     cout << value->GetValue();
}

IntValue* nodeInterpreter::Not(IntValue* value)
{
     if(value->GetValue() == 0)
          return new IntValue(1);
     else
          return new IntValue(0);
}

/*********
 * VISITS
 * *******/

void nodeInterpreter::visitProgram(program* Program)
{
     evaluate(Program->Node);
}
void nodeInterpreter::visitBreak(NBreak* Break)
{
     breakCalled = true;
}
void nodeInterpreter::visitNil(NNil* Nil)
{
     Nil->value = new RecordValue();
}
void nodeInterpreter::visitID(NId* id)
{
     id->value = ((VarSymbol*)(table.LookupSymbol(id->name)))->val;
}
void nodeInterpreter::visitTyID(NTyId* tyid)
{
     
}
void nodeInterpreter::visitIntLit(NIntLit* intLit)
{
     intLit->value = new IntValue(intLit->val);
}
void nodeInterpreter::visitStrLit(NStrLit* strLit)
{
     strLit->value = new StringValue(strLit->val);
}
void nodeInterpreter::visitSubscript(subscript* Subscript)
{
     //Get the ArrayValue from left hand side
     evaluate(Subscript->lValue);
     ArrayValue* arr = (ArrayValue*)(Subscript->lValue->value);

     //Get the int result of the subscript
     evaluate(Subscript->exp);
     IntValue* index = (IntValue*)(Subscript->exp->value);

     //cout << "Getting index: " << index->GetValue() << endl;

     //Get the value in the array at the desired location
     int intIndex = index->GetValue();
     //cout << "Subscripting at index " << intIndex  << " on line " << Subscript->lineNumber << endl;
     Subscript->value = arr->GetValue(intIndex);
     if(Subscript->value == NULL)
     {
          cout << "ERROR " << Subscript->exp->lineNumber << ": Runtime: Array access out of bounds." << endl;
          exit(4);
     }
}
void nodeInterpreter::visitFieldExp(fieldExp* FieldExp)
{
     //Get the value for the Record type
     evaluate(FieldExp->lValue);
     RecordValue* rec = (RecordValue*)(FieldExp->lValue->value);

     //Get the current value of its member
     string name =((NId*)(FieldExp->ID))->name;
     FieldExp->value = rec->GetValue(name);
}
void nodeInterpreter::visitSeqExp(seqExp* SeqExp)
{
     //If it has at least one exp, set the value to the last one
     if(SeqExp->exps->size()!=0)
     {
          for(int i = 0; i < SeqExp->exps->size(); i++)
          {
               evaluate((*(SeqExp->exps))[i]);

               //Check if a break happened after each expression
               if(breakCalled)
                    break;
          }
          SeqExp->value = ((*(SeqExp->exps))[SeqExp->exps->size()-1])->value;
     } 

     //Else, just leave the value of the expression blank    
}
void nodeInterpreter::visitNegation(negation* neg)
{
     evaluate(neg->operand);
     IntValue* intVal = (IntValue*)(neg->operand->value);
     neg->value = new IntValue(-1*(intVal->GetValue()));
}
void nodeInterpreter::visitCallExp(callExp* CallExp)
{
     //Evaluate the contents of the call expression first
     vector<node*>* passedParameters = CallExp->exps;
     for(int i = 0; i < passedParameters->size(); i++)
     {
          evaluate((*passedParameters)[i]);
     }

     string name = ((NId*)(CallExp->id))->name;
     if(name == "print")
     {
          Print((StringValue*)(*passedParameters)[0]->value);
     } 
     else if(name == "printi")
     {
          Print((IntValue*)(*passedParameters)[0]->value);
     } 
     else if(name == "not")
     {
          CallExp->value = Not((IntValue*)(*passedParameters)[0]->value);
     } 
     else //It's a normal function call
     {
          //create new scope for function
          Scope* funcScope = new Scope(table.top, S_FUNC);
          table.PushScope(funcScope);

          //Build symbols for each parameter
          vector< pair <string,Type*>> funcParams = functionParams[name];
          for(int i = 0; i < passedParameters->size(); i++)
          { 
               VarSymbol* var = new VarSymbol(funcParams[i].first, SYM_VAR, funcParams[i].second, ((*passedParameters)[i])->value);
               funcScope->AddSymbol(var);
          }

          //Get the pair from the function map
          map<string,node*>::iterator itr = functionExpressions.find(name);
          evaluate(itr->second);
          if(itr->second->value != NULL)
               CallExp->value = itr->second->value->Copy();
          delete itr->second->value;

          //Pop scope and kill it when done
          delete table.PopScope();
     }   
}
void nodeInterpreter::visitInfixExp(infixExp* InfixExp) {
     //Evaluate the LEFT expression first
     evaluate(InfixExp->leftNode);

     //Test for the short circuit expressions & and |
     if(InfixExp->op == OP_AND && ((IntValue*)(InfixExp->leftNode->value))->GetValue() == 0)
    {
         InfixExp->value = new IntValue(0);
    }
    else if(InfixExp->op == OP_OR && ((IntValue*)(InfixExp->leftNode->value))->GetValue() != 0)
    {
         InfixExp->value = new IntValue(1);
    }


     evaluate(InfixExp->rightNode);

     //Welcome to IF/ELSE city, home of the bad code
     
     //Honestly I could have used a switch here but
     // a core design feature of my interpreter
     // is bad design so
    if(InfixExp->op == OP_ADD)
    {
          IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
          IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
          InfixExp->value = new IntValue(leftVal->GetValue()+rightVal->GetValue());
    } else if(InfixExp->op == OP_SUBTRACT)
    {

    
               IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
               IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
               InfixExp->value = new IntValue(leftVal->GetValue()-rightVal->GetValue());
    } else if(InfixExp->op == OP_MULTIPLY)
    {

    
               IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
               IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
               InfixExp->value = new IntValue(leftVal->GetValue()*rightVal->GetValue());
    } else if(InfixExp->op == OP_DIVIDE)
    {

    
               IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
               IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
               InfixExp->value = new IntValue(leftVal->GetValue()/rightVal->GetValue());
    } else if(InfixExp->op == OP_EQ)
    {
               if(InfixExp->leftNode->type->GetActualType()->name == "string")
               {
                    StringValue* leftVal = (StringValue*)(InfixExp->leftNode->value);
                    StringValue* rightVal = (StringValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() == rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else if(InfixExp->leftNode->type->GetActualType()->name == "int")
               {
                    IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
                    IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() == rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else //Else, they must be array or record types
               {
                    if(InfixExp->leftNode->value->kind == V_ARR && InfixExp->rightNode->value->kind == V_ARR)
                    {
                         if(InfixExp->leftNode->value == InfixExp->rightNode->value)
                              InfixExp->value = new IntValue(1);
                         else
                              InfixExp->value = new IntValue(0);
                    }
                    else if(InfixExp->leftNode->value->kind == V_REC && InfixExp->rightNode->value->kind == V_REC)
                    {
                         if(InfixExp->leftNode->value == InfixExp->rightNode->value)
                              InfixExp->value = new IntValue(1);
                         else
                              InfixExp->value = new IntValue(0);
                    }
                    else
                         cout << "Braden is Bad at Programming: Value type mismatch on line " << InfixExp->lineNumber << endl;
               }

    } else if(InfixExp->op == OP_NEQ)
    {
               if(InfixExp->leftNode->type->GetActualType()->name == "string")
               {
                    StringValue* leftVal = (StringValue*)(InfixExp->leftNode->value);
                    StringValue* rightVal = (StringValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() != rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else if(InfixExp->leftNode->type->GetActualType()->name == "int")
               {
                    IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
                    IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() != rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else
               {
                    if(InfixExp->leftNode->value->kind == V_ARR && InfixExp->rightNode->value->kind == V_ARR)
                    {
                         if(InfixExp->leftNode->value == InfixExp->rightNode->value)
                              InfixExp->value = new IntValue(0);
                         else
                              InfixExp->value = new IntValue(1);
                    }
                    else if(InfixExp->leftNode->value->kind == V_REC && InfixExp->rightNode->value->kind == V_REC)
                    {
                         if(InfixExp->leftNode->value == InfixExp->rightNode->value)
                              InfixExp->value = new IntValue(0);
                         else
                              InfixExp->value = new IntValue(1);
                    }
                    else
                         cout << "Braden is Bad at Programming: Value type mismatch on line " << InfixExp->lineNumber << endl;
               }

    }else if(InfixExp->op == OP_LT)
    {
               if(InfixExp->leftNode->type->GetActualType()->name == "string")
               {
                    StringValue* leftVal = (StringValue*)(InfixExp->leftNode->value);
                    StringValue* rightVal = (StringValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() < rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else if(InfixExp->leftNode->type->GetActualType()->name == "int")
               {
                    IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
                    IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() < rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else
               {
                    cout << "Ah shit I screwed up the typing somewhere." << endl;
               }

    }else if(InfixExp->op == OP_LEQ)
    {
               if(InfixExp->leftNode->type->GetActualType()->name == "string")
               {
                    StringValue* leftVal = (StringValue*)(InfixExp->leftNode->value);
                    StringValue* rightVal = (StringValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() <= rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else if(InfixExp->leftNode->type->GetActualType()->name == "int")
               {
                    IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
                    IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() <= rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else
               {
                    cout << "Ah shit I screwed up the typing somewhere." << endl;
               }

    }else if(InfixExp->op == OP_GT)
    {
               if(InfixExp->leftNode->type->GetActualType()->name == "string")
               {
                    StringValue* leftVal = (StringValue*)(InfixExp->leftNode->value);
                    StringValue* rightVal = (StringValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() > rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else if(InfixExp->leftNode->type->GetActualType()->name == "int")
               {
                    IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
                    IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() > rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else
               {
                    cout << "Ah shit I screwed up the typing somewhere." << endl;
               }

    } else if(InfixExp->op == OP_GEQ)
    {
               if(InfixExp->leftNode->type->GetActualType()->name == "string")
               {
                    StringValue* leftVal = (StringValue*)(InfixExp->leftNode->value);
                    StringValue* rightVal = (StringValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() >= rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else if(InfixExp->leftNode->type->GetActualType()->name == "int")
               {
                    IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
                    IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() >= rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else
               {
                    cout << "Ah shit I screwed up the typing somewhere." << endl;
               }

    }else if(InfixExp->op == OP_NEQ)
    {
               if(InfixExp->leftNode->type->GetActualType()->name == "string")
               {
                    StringValue* leftVal = (StringValue*)(InfixExp->leftNode->value);
                    StringValue* rightVal = (StringValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() != rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else if(InfixExp->leftNode->type->GetActualType()->name == "int")
               {
                    IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
                    IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
                    if(leftVal->GetValue() != rightVal->GetValue())
                         InfixExp->value = new IntValue(1);
                    else
                         InfixExp->value = new IntValue(0);
               }
               else
               {
                    cout << "Ah shit I screwed up the typing somewhere." << endl;
               }

    }else if(InfixExp->op == OP_AND)
    {
               IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
               IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
               if(leftVal->GetValue() != 0 && rightVal->GetValue() != 0)
               {
                    InfixExp->value =  new IntValue(1);
               }
               else
                    InfixExp->value = new IntValue(0);
    }else if(InfixExp->op == OP_OR)
    {
               IntValue* leftVal = (IntValue*)(InfixExp->leftNode->value);
               IntValue* rightVal = (IntValue*)(InfixExp->rightNode->value); 
               if(leftVal->GetValue() != 0 || rightVal->GetValue() != 0)
               {
                    InfixExp->value =  new IntValue(1);
               }
               else
                    InfixExp->value = new IntValue(0);
    }
}

void nodeInterpreter::visitArrCreate(arrCreate* ArrCreate)
{
     //Get the subscript size
     evaluate(ArrCreate->subscriptExp);
     int size = ((IntValue*)(ArrCreate->subscriptExp->value))->GetValue();

     //Get the value for the post expression
     evaluate(ArrCreate->postExp);

     //Create the new array value
     ArrCreate->value = new ArrayValue(size, ArrCreate->postExp->value->Copy());
}

/**
 * @brief 
 * 
 * @param RecCreate 
 */
void nodeInterpreter::visitRecCreate(recCreate* RecCreate)
{
     //For every field, create a value pair of id -> value
     map<string,Value*> valPairs;
     for(int i = 0; i < RecCreate->fields->size(); i++)
     {
          fieldCreate* FieldCreate = (fieldCreate*)(*(RecCreate->fields))[i];
          string name =((NId*)(FieldCreate->id))->name;
          evaluate(FieldCreate);

          valPairs.insert(pair<string,Value*>(name, FieldCreate->value));

     }

     //Create a new record value with it
     RecordValue* recVal = new RecordValue(valPairs);
     RecCreate->value = recVal;
}

void nodeInterpreter::visitFieldCreate(fieldCreate* FieldCreate)
{
     evaluate(FieldCreate->exp);
     FieldCreate->value = FieldCreate->exp->value;
}

void nodeInterpreter::visitAssignment(assignment* Assign)
{
     //Calculate all of the nodes for the LValue to get the reference
     // to whatever variable value is on the LHS
     evaluate(Assign->lVal);
     //delete(Assign->lVal->value);

     //Set it equal to a copy of the rhs+
     evaluate(Assign->exp);

     
     Assign->lVal->value->SetValue(Assign->exp->value->Copy());
}
void nodeInterpreter::visitIfThenElse(ifThenElse* iTE)
{
     //If the condition evaluates to non-zero...
     evaluate(iTE->ifExp);
     if(isNonZero(iTE->ifExp->value))
     {
          //Evaluate it and set the exp equal to the then's result
          evaluate(iTE->thenExp);
          iTE->value = iTE->thenExp->value;

     }
     //Else, if there's even else expression...
     else if(iTE->elseExp != NULL)
     {
          //Evaluate it and set the exp to its result
          evaluate(iTE->elseExp);
          iTE->value = iTE->elseExp->value;
     }
     //Else, just leave its value at null
}

void nodeInterpreter::visitWhileExp(whileExp* While)
{
     while(true)
     {
          //Wipe the old one if it was there
          if(While->condition->value != NULL)
          {
               delete(While->condition->value);
          }

          //Evaluate the condition
          evaluate(While->condition);

          //If it's anything other than zero...
          if(isNonZero(While->condition->value))
          {
               //Evaluate the inner actions
               evaluate(While->action);

               //After it runs the action, see if break was triggered and escape that way
               if(breakCalled)
               {
                    breakCalled = false;
                    break;
               }
          }
          else 
          {
               break;
          }
     }
}

void nodeInterpreter::visitForExp(forExp* forEx)
{
     //Create a new scope for the new variable
     Scope* forScope = new Scope(table.top, S_FOR);
     table.PushScope(forScope);

     //Create a symbol for the variable and add it to the scope
     string name = ((NId*)(forEx->id))->name;
     evaluate(forEx->assign);
     VarSymbol* var = new VarSymbol(name, SYM_VAR, forEx->assign->type, forEx->assign->value);
     forScope->AddSymbol(var);

     //Determine the value of the condition
     evaluate(forEx->condition);

     //Run the loop for the difference between the initial and condition
     IntValue* initial = (IntValue*) var->val;
     int limit = ((IntValue*) (forEx->condition->value))->GetValue();
     for(int i = initial->GetValue(); i < limit; i++)
     {
          //Update the variable's value in case the loop exps use it
          ((IntValue*)var->val)->SetValue(i);

          evaluate(forEx->action);
          if(breakCalled)
          {
               breakCalled = false;
               break;
          }
     }

     //Pop the scope once done
     delete (table.PopScope());
}
void nodeInterpreter::visitLetExp(letExp* LetExp)
{
     Scope* letScope = new Scope(table.top, S_LET);
     table.PushScope(letScope);
     //Evaluate all of the variable declarations
     //cout << endl << endl << "NUMBER OF DECLARATIONS: " << LetExp->decs->size() << endl << endl;
     for(int i = 0; i < LetExp->decs->size(); i++)
     {
          evaluate((*(LetExp->decs))[i]);
          
     }
     
     //Evaluate all of the body expressions in order
     for(int i = 0; i < LetExp->exps->size(); i++)
     {
          evaluate((*(LetExp->exps))[i]);
     }

     //Value of LetExp is value of the last body expression
     Value* lastVal = (*(LetExp->exps))[LetExp->exps->size()-1]->value;
     if(lastVal != NULL)
          LetExp->value = lastVal ->Copy();

     delete (table.PopScope());
}
void nodeInterpreter::visitDec(decc* Dec)
{}
void nodeInterpreter::visitTyDec(tyDec* TyDec)
{
     table.top->AddType(TyDec->type);
}

void nodeInterpreter::visitTyDef(tyDef* TyDef)
{

}

void nodeInterpreter::visitRefTy(refTy* RefTy)
{
}

void nodeInterpreter::visitArrTy(arrTy* ArrTy)
{
     
}

void nodeInterpreter::visitRecTy(recTy* RecTy)
{

}

void nodeInterpreter::visitFieldDec(fieldDec* FieldDec)
{

}

void nodeInterpreter::visitFunDec(funDec* FunDec)
{
     //Get the name and type that go with the func
     string name = ((NId*)(FunDec->id))->name;

     //Get the params
     vector<pair<string,Type*> > params = FunDec->getParams();
     
     //Make an entry in the function map for it
     functionExpressions.insert(pair<string,node*>(name, FunDec->exp));
     functionParams.insert(pair<string,vector< pair<string,Type*> > >(name, params));
}

void nodeInterpreter::visitVarDec(varDec* VarDec)
{
     //Get the name and type that go with the variable
     string name = ((NId*)(VarDec->id))->name;
     Type* varType = VarDec->type;

     //Evaluate whatever it's initialized to
     evaluate(VarDec->exp);

     //Create a new symbol and drop it on the table
     Symbol* varSym = new VarSymbol(name, SYM_VAR, varType, VarDec->exp->value);
     table.top->AddSymbol(varSym);

     
     
}