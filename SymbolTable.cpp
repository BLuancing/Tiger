#include <map>
#include <string>
#include <iostream>
#include "SymbolTable.h"

using namespace std;

SymbolTable::SymbolTable()
{    
     top = NULL;
     size = 0;
}

SymbolTable::SymbolTable(Scope* scope)
{
     top = scope;
     size = 1;
}
SymbolTable::~SymbolTable()
{
     while(top != NULL)
     {
          Scope* target = top;
          top = target->last;
          delete target;
     }
}
Scope* SymbolTable::PopScope()
{
     //cout << "Table popped." << endl;
     if(IsEmpty())
     {
          return NULL;
     }
     else
     {
          Scope* target = top;
          top = target->last;
          size--;
          return target;
     }
}
void SymbolTable::PushScope(Scope* scope)
{
     scope->last = top;
     top = scope;
     size++;
}
void SymbolTable::DefaultTigerTable()
{
     Scope* defScope = new Scope(top, S_LET);

     //Add default types
     Type* i = new Type("int",T_PRIM);
     defScope->AddType(i);
     Type* s = new Type("string",T_PRIM);
     defScope->AddType(s);
     Type* u = new Type("unit",T_PRIM);
     defScope->AddType(u);

     //Add print function
     vector< pair<string,Type*> > printMap;
     printMap.push_back(pair<string,Type*>("s", s)); 
     defScope->AddSymbol(new FuncSymbol("print", SYM_FUNC, u, printMap));

     //Add integer print function
     vector< pair<string,Type*> > iPrintMap;
     iPrintMap.push_back(pair<string,Type*>("i", i)); 
     defScope->AddSymbol(new FuncSymbol("printi", SYM_FUNC, u, iPrintMap));

     //Add not function
     vector< pair<string,Type*> > notMap;
     notMap.push_back(pair<string,Type*>("i", i)); 
     defScope->AddSymbol(new FuncSymbol("not", SYM_FUNC, i, notMap));

     //Add this to the top of the stack
     this->PushScope(defScope);
}
Symbol* SymbolTable::LookupSymbol(const string &id)
{
     Scope* currentScope = top;
     while(currentScope != NULL)
     {
          Symbol* symb = currentScope->LookupSymbol(id);
          if(symb != NULL)
               return symb;
          currentScope = currentScope->last;
     }

     return NULL;
}
Type* SymbolTable::LookupType(const string &id)
{
     Scope* currentScope = top;
     while(currentScope != NULL)
     {
          Type* typ = currentScope->LookupType(id);
          if(typ != NULL)
               return typ;
          currentScope = currentScope->last;
     }

     return NULL;
}
bool SymbolTable::IsEmpty()
{
     if(size == 0)
          return true;
     else
          return false;
}
void SymbolTable::PrintAll()
{
     Scope* currentScope = top;
     while(currentScope != NULL)
     {
          currentScope->Print();
          cout << endl << "------------" << endl;
          currentScope = currentScope->last;
     }
}

/***************
 *  SCOPE
 * ************/

Scope::Scope()
{
     last = NULL;
     source = S_LET;
}
Scope::Scope(map<string,Symbol*> decs, map<string,Type*> types, ScopeType source)
:decs(decs),types(types),source(source)
{
     last = NULL;
}
Scope::Scope(Scope* last, ScopeType source):last(last),source(source){}
Scope::Scope(Scope* last, map<string,Symbol*> decs, map<string,Type*> types, ScopeType source)
:last(last),decs(decs),types(types),source(source)
 {

 }
Symbol* Scope::LookupSymbol(const string &id)
{
     map<string,Symbol*>::iterator itr;
     for (itr = decs.begin(); itr != decs.end(); ++itr) { 
          if(itr->first == id)
               return itr->second;
    } 

    return NULL;
}
Type* Scope::LookupType(const string &id)
{
     map<string,Type*>::iterator itr;
     for (itr = types.begin(); itr != types.end(); ++itr) { 
          if(itr->first == id)
               return itr->second;
    } 

    return NULL;
}
bool Scope::AddSymbol(Symbol* symbol)
{
     pair<map<string,Symbol*>::iterator,bool> result = decs.insert(pair<string,Symbol*>(symbol->name, symbol));
     //If there's already a symbol for it in the table...
     if(result.second == false)
     {
          //Because Tiger lets you hide stuff in the same scope so long as the types
          // are different, gotta check for that
          Symbol* existingSymbol = (*(result.first)).second;
          if(existingSymbol->type == symbol->type)
          {
               return false;
          }
               
          //If they don't match...
          else
          {
               //Wipe the other symbol since it's basically irrelevant now,
               // then insert this new one
               decs.erase(symbol->name);
               decs.insert(pair<string,Symbol*>(symbol->name, symbol));
               return true;
          }
     }

     else
          return true;
}
bool Scope::AddType(Type* type)
{
     pair<map<string,Type*>::iterator,bool> result = types.insert(pair<string,Type*>(type->name, type));
     //If there's already a symbol for it in the table...
     if(result.second == false)
     {
          return false;
     }
     else
          return true;
}
bool Scope::RemoveType(Type* type)
{
     int erased = types.erase(type->name);
     if(erased == 1)
          return false;
     else
          return true;
}

void Scope::Print()
{
     //Print symbols
     map<string,Symbol*>::iterator symItr;
     cout << "-SYMBOLS-" << endl << endl;
     for (symItr = decs.begin(); symItr != decs.end(); ++symItr) 
     { 
          symItr->second->Print();
          cout << endl; 
     }
     cout << endl;

    //Print Types
     map<string,Type*>::iterator tyItr;
     cout << "-TYPES-" << endl << endl;
     for (tyItr = types.begin(); tyItr != types.end(); ++tyItr) 
     { 
          tyItr->second->Print(); 
          cout << endl;
     } 
     cout << endl;
}

/**********
 * SYMBOLS
 * *******/
Symbol::~Symbol(){}
Symbol::Symbol(const string &name, SymbolKind kind):name(name), kind(kind){}
void Symbol::Print()
{
     cout << name << " (<" << type->name << ">)";
}

/************
 * VAR SYMBOL
 * **********/

VarSymbol::VarSymbol(const string &name, SymbolKind kind, Type* type, bool readOnly):Symbol(name,kind)
{
     this->type = type;
     this->readOnly = readOnly;
}
VarSymbol::VarSymbol(const string &name, SymbolKind kind, Type* type, Value* val, bool readOnly)
:Symbol(name,kind), val(val)
{
     this->type = type;
     this->readOnly = readOnly;
}
void VarSymbol::Print()
{
     cout << name << " (<" << type->name << ">)";
}

FuncSymbol::FuncSymbol(const string &name, SymbolKind kind, Type* type, vector< pair<string,Type*> > args):
Symbol(name,kind), args(args)
{
     this->type = type;
}
void FuncSymbol::Print()
{
     cout << type->name << " " << name << " (";
     for(int i = 0; i < args.size(); i++)
     {
          cout << args[i].first << " : <" << args[i].second->name << "> , ";
     }

     cout << ")";
}

Scope* FuncSymbol::CreateScopeFromParams()
{
     Scope* scope = new Scope();
     scope->source = S_FUNC;
     for(int i = 0; i < args.size(); i++)
     {
          pair<string,Type*> param = args[i];
          scope->AddSymbol(new VarSymbol(param.first, SYM_VAR, param.second));
     }

     return scope;
}

/********
 * TYPES
 * ******/

Type::Type(const string &name, TypeKind kind):name(name), kind(kind){}
Type::~Type(){}
bool Type::Equals(Type* rhs)
{
     Type* lhs = this;

     //First, see if their names are the same
     //if(lhs.name == rhs.name)
     if(lhs->name == rhs->name)
          return true;
     else 
     {
          Type* actualType1;
          const Type* actualType2;
          //Next, see if they are reference or array types; need to pull
          // out the base type they are referencing to see if those match
          //if(lhs.kind == T_REF )
          if(lhs->kind == T_REF )
          {
               //actualType1 = ((RefType*)&lhs)->ref->GetActualType();
               actualType1 = ((RefType*)lhs)->ref->GetActualType();
               
          }
          //else if(lhs.kind == T_ARR)
          else if(lhs->kind == T_ARR)
          {
              // actualType1 = ((ArrType*)&lhs)->ref->GetActualType();
              actualType1 = ((ArrType*)lhs)->ref->GetActualType();
          }
          else
          {
               //actualType1 = &lhs;
               actualType1 = lhs;
          }
               

          //if(rhs.kind == T_REF )
          if(rhs->kind == T_REF )
          {
               //actualType2 = ((RefType*)&rhs)->ref->GetActualType();
               actualType2 = ((RefType*)rhs)->ref->GetActualType();
          }
          //else if(rhs.kind == T_ARR)
          else if(rhs->kind == T_ARR)
          {
               //actualType2 = ((ArrType*)&rhs)->ref->GetActualType();
               actualType2 = ((ArrType*)rhs)->ref->GetActualType();
          }
          else
          {
               actualType2 = rhs;
          }
               //actualType2 = &rhs;


          cout << "Set left actual type to " << actualType1->name << endl;
          cout << "Set right actual type to " << actualType2->name << endl;
          //This should set either side to either int or string
          if(actualType1->name == actualType2->name)
               return true;
          else
               return false;

     }
          return false;
}

Type* Type::GetActualType()
{
     if(name == "int" || name == "string")
     {
          return this;
     }
     else
     {
          switch(kind)
          {
               case T_REF:
                    return ((RefType*)this)->ref->GetActualType();
               case T_ARR:         
                    return ((ArrType*)this)->ref->GetActualType();
               case T_REC:
               case T_PRIM:
                    return this;
          };
     }
}

TypeKind Type::GetActualKind()
{
     if(name == "int" || name == "string")
     {
          return this->kind;
     }
     else
     {
          switch(kind)
          {
               case T_REF:
                    return ((RefType*)this)->ref->kind;
               case T_ARR:         
                    return T_ARR;
               case T_REC:
                    return T_REC;
               case T_PRIM:
                    return T_PRIM;
          };
     }
}
void Type::Print()
{
     cout << name;
}
RefType::RefType(const string &name, TypeKind kind):Type(name,kind){}
RefType::RefType(const string &name, TypeKind kind, Type* ref):Type(name,kind), ref(ref)
{
     //Store the kind of the kind of reference
     // since it's important to note what kind of type the
     // original reference was (array or ref)
     //
     // This arose from issues where type a = array of int, type b = a, and b[1] is called
     TypeKind refKind = ref->kind;
     ref = ref->GetActualType();
     ref->kind = refKind;
     
}
void RefType::Print()
{
     cout << name << " (ref to " << ref->name << ")";
}
ArrType::ArrType(const string &name, TypeKind kind):Type(name,kind){}
ArrType::ArrType(const string &name, TypeKind kind, Type* ref):Type(name,kind), ref(ref)
{
     //Store the kind of the kind of reference
     // since it's important to note what kind of type the
     // original reference was (array or ref)
     //
     // This arose from issues where type a = array of int, type b = a, and b[1] is called
     TypeKind refKind = ref->kind;
     ref = ref->GetActualType();
     ref->kind = refKind;
}
void ArrType::Print()
{
     cout << name << " (array of " << ref->name << ")";
}
RecType::RecType(const string &name, TypeKind kind):Type(name,kind){}
RecType::RecType(const string &name, TypeKind kind, map<string,Type*>* fields):Type(name,kind),fields(fields)
{
}
void RecType::Print()
{
     cout << name;
}
bool RecType::hasMember(const string &name)
{
     map<string,Type*>::iterator itr = fields->find(name);
     if(itr == fields->end())
          return false;
     else 
          return true;
}
pair<string,Type*> RecType::getFieldPair(const string &name)
{
     map<string,Type*>::iterator itr = fields->find(name);
     if(itr == fields->end())
          cout << "WARNING: getFieldPair() found no field with such a name.\n";
     return *itr;

}

/*********
 * VALUES
 * *******/

Value::Value(){}
void Value::Print(){cout << "(no value)";};
Value* Value::Copy(){return NULL;}
bool Value::operator==(Value* other)
{
     if(this->kind == other->kind)
          return true;
     else
          return false;
}

/**
 * @brief Copies the passed value and sets its own value to match.
 * 
 * @param other The value to copy.
 */
void Value::SetValue(Value* other)
{
     if(kind == V_INT) //Int value
     {
          IntValue* newInt = ((IntValue*)((IntValue*)other)->Copy());
          ((IntValue*)this)->SetValue(newInt->GetValue());
          delete newInt;
     }
     else if(kind == V_STR) //String value
     {
          StringValue* newString = ((StringValue*)((StringValue*)other)->Copy());
          ((StringValue*)this)->SetValue(newString->GetValue());
          delete newString;
     }
     else if(kind == V_ARR) //Array value
     {
          ArrayValue* newArray = ((ArrayValue*)((ArrayValue*)other)->Copy());
          ((ArrayValue*)this)->SetValue(newArray->GetArray());
          delete newArray;
     }
     else //Record value
     {
          RecordValue* newRecord = ((RecordValue*)((RecordValue*)other)->Copy());
          ((RecordValue*)this)->SetValue(newRecord->GetRecord());
          delete newRecord;
     }
}

IntValue::IntValue(int val){ this->val = val; kind = V_INT;}
int IntValue::GetValue(){return val;}
void IntValue::SetValue(const int &inVal){val = inVal;}
void IntValue::SetValue(IntValue* otherValue){val = otherValue->GetValue();}
void IntValue::Print(){cout << GetValue();}
Value* IntValue::Copy(){return new IntValue(GetValue());}
bool IntValue::operator==(Value* other)
{
     IntValue* intVal = (IntValue*) other;
     if(intVal->val == this->val)
          return true;
     else
          return false;
}

StringValue::StringValue(const string &val):val(val){kind = V_STR;}
string StringValue::GetValue(){return val;}
void StringValue::SetValue(const string &inVal){val = inVal;}
void StringValue::SetValue(StringValue* otherValue){val = otherValue->GetValue();}
void StringValue::Print(){cout << GetValue();}
Value* StringValue::Copy(){return new StringValue(GetValue());}
bool StringValue::operator ==(Value* other)
{
     StringValue* str = (StringValue*) other;
     if(str->val == this->val)
          return true;
     else
          return false;
}

ArrayValue::ArrayValue(const int &size, Value* val)
{
     kind = V_ARR; 
     for(int i = 0; i < size; i++)
     {
          //cout << "PUSHED BACK" << endl;
          this->val.push_back(val->Copy());
     }
}
ArrayValue::ArrayValue(vector<Value*> val):val(val){}

Value* ArrayValue::GetValue(const int &index)
{
     //If the value exceeds the bounds of the array...
     if(index < 0 || index >= this->val.size() )
     {
          //Throw a runtime error
          //cout << "ERROR: Runtime: Array access out of bounds." << endl;
          //exit(4);
          return NULL;
     }
     return val[index];}
vector<Value*> ArrayValue::GetArray(){return val;}
void ArrayValue::SetValue(const int &index, Value* val)
{
     //If the value exceeds the bounds of the array...
     if(index < 0 || index >= this->val.size() )
     {
          //Throw a runtime error
          cout << "ERROR: Runtime: Array access out of bounds." << endl;
          exit(4);
     }
          
     delete this->val[index];
     this->val[index] = val->Copy();
}
void ArrayValue::SetValue(vector<Value*> val){this->val = val;}
void ArrayValue::Print()
{
     for(int i = 0; i < val.size()-1; i++)
     {
          val[i]->Print();
          cout << ", ";
     }
     
     val[val.size()-1]->Print();
}
Value* ArrayValue::Copy()
{
     vector<Value*> newValues;
     for(int i = 0; i < val.size(); i++)
     {
          newValues.push_back(val[i]->Copy());
     }

     return new ArrayValue(newValues);
}
bool ArrayValue::operator==(Value* other)
{
     ArrayValue* arr = (ArrayValue*) other;
     if(val.size() == arr->val.size())
     {
          for(int i = 0; i < val.size(); i++)
          {
               if(!(val[i] == arr->val[i]))
               {
                    return false;
               }
          }
          return true;
     }
     else
          return false;
}
RecordValue::RecordValue(){kind = V_REC;}
RecordValue::RecordValue(map<string,Value*> val):val(val){kind = V_REC;}
Value* RecordValue::GetValue(const string &member)
{
     map<string,Value*>::iterator itr = val.find(member);
     if(itr == val.end())
     {
          cout << "Wait...RecordValue couldn't find member?" << endl;
          exit(4);
     }
     return (itr->second)->Copy();
     }
void RecordValue::SetValue(const string &key, Value* inValue)
{
     //First, yoink out the value we want
     map<string,Value*>::iterator itr = val.find(key);
     if(itr == val.end())
     {
          cout << "Wait...RecordValue couldn't find member?" << endl;
          exit(4);
     }

     //Kill the old value, and replace it with the new one
     delete itr->second;
     itr->second = (inValue->Copy());
}
void RecordValue::SetValue(map<string,Value*> val){this->val = val;}
map<string, Value*> RecordValue::GetRecord(){return val;}
void RecordValue::Print()
{
     for(map<string,Value*>::iterator itr = val.begin(); itr != val.end(); itr++)
     {
          cout << itr->first << " : ";
          itr->second->Print();
          cout << ", ";
     }
}
Value* RecordValue::Copy()
{
     map<string,Value*> newMap;

     for(map<string,Value*>::iterator itr = val.begin(); itr != val.end(); itr++)
     {
          newMap.insert(pair<string,Value*>(itr->first, itr->second->Copy()));
     }

     return new RecordValue(newMap);
}

bool RecordValue::operator==(Value* other)
{
     RecordValue* rec = (RecordValue*) other;

     if(rec->val.size() == this->val.size())
     {
          for(map<string,Value*>::iterator itr = val.begin(); itr != val.end(); itr++)
          {
               map<string,Value*>::iterator otherRecItr = rec->val.find(itr->first);
               if(otherRecItr == rec->val.end())
                    return false;
               if(!(otherRecItr->second == itr->second))
                    return false;
          }
          return true;
     }
     else
          return false;
          
}