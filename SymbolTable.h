/*
     Created by:    Braden Luancing
     Major:         Computer Science
     Creation Date: 11/28/19
     Due Date:      12/8/19
     Course:        CSC425
     Professor:     Dr. Schwesinger
     Assignment:    #3
     Filename:      SymbolTable.h
     Purpose:       Contains types and functions for managing a symbol table
                    for the Tiger language. Includes scopes and scope management,
                    as well as types and values.

*/

/** @defgroup SYMBOL Symbol Table
 *  A symbol table, complete with scope, types, symbols, and values.
 *  @{
 */

#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <map>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

//Forward declsrations for some thnigs
class Scope;
class Symbol;
class Type;
class Value;

/**
 * @brief Table of symbols and types arranged in a scope
 * stack. Allows scope management functions as well as identifier
 * lookup.
 * 
 */
class SymbolTable{
     public:
          /**
           * @brief Construct a new, empty symbol table.
           * 
           */
          SymbolTable();
          
          /**
           * @brief Construct a new SymbolTable with the passed in scope
           * at the top of the scope stack.
           * 
           */
          SymbolTable(Scope*);

          /**
           * @brief Destroy the Symbol Table object.
           * 
           */
          ~SymbolTable();

          /**
           * @brief Removes a scope from the stack and returns a reference to it.
           * 
           * @return Scope* The scope popped off the top of the stack.
           */
          Scope* PopScope();

          /**
           * @brief Pushes a new scope onto the top of the stack.
           * 
           */
          void PushScope(Scope*);
          
          /**
           * @brief Initializes this symbol table with default Tiger
           * symbols and types.
           */
          void DefaultTigerTable();

          /**
           * @brief Searches the entire scope stack for a function or variable symbol by 
           * name and returns a reference to it; if not found, returns NULL.
           * 
           * @return Symbol* The symbol, if found; NULL if not found in the table.
           */
          Symbol* LookupSymbol(const string &);

          /**
           * @brief Searches the entire scope stack for a type by 
           * name and returns a reference to it; if not found, returns NULL.
           * 
           * @return Type* The type, if found; NULL if not found in the table.
           */
          Type* LookupType(const string &);

          /**
           * @brief Returns true if the symbol table has no scopes on it.
           * 
           * @return true If the symbol table has no scopes on it.
           * @return false If the symbol table has at least one scope on it.
           */
          bool IsEmpty();

          /**
           * @brief Prints all types and symbols for all scopes on the table.
           * 
           */
          void PrintAll();

          /**
           * @brief The top of the scope stack.
           * 
           */
          Scope* top;

          /**
           * @brief The number of scopes in the symbol table currently.
           * 
           */
          int size;


};

/**
 * @brief The origin of a scope, such as a scope from a Let expression.
 * 
 */
enum ScopeType
{
     /**
      * @brief Scope originated from Let expression.
      * 
      */
     S_LET,
     /**
      * @brief Scope originated from While loop.
      * 
      */
     S_WHILE,
     /**
      * @brief Scope originated from For loop.
      * 
      */
     S_FOR,
     /**
      * @brief Scope originated from Function activation.
      * 
      */
     S_FUNC
};

/**
 * @brief Describes a scope in the program. Contains maps of names to symbols
 * and types. Allows for symbol and type lookup, and removal.
 * 
 */
class Scope{
     public:
          /**
           * @brief Construct a new Scope object with no previous scope and a S_LET
           * origin.
           * 
           */
          Scope();

          /**
           * @brief Construct a new Scope object with a reference to a passed in Scope
           * and a given origin.
           * 
           * @param last The scope previous to this one on the stack.
           * @param source The origin of this scope.
           */
          Scope(Scope* last, ScopeType source);

          /**
           * @brief Construct a new Scope object with given symbols, types, and source.
           * 
           * @param decs A map of symbols to start with.
           * @param types A map of types to start with.
           * @param source The origin of this scope.
           */
          Scope(map<string,Symbol*> decs, map<string,Type*> types, ScopeType source);

          /**
           * @brief Construct a new Scope object with a previous scope, given symbols, types,
           * and source.
           * 
           * @param last The scope previous to this one in a stack.
           * @param decs A map of symbols to start with.
           * @param types A map of types to start with.
           * @param source The origin of this scope.
           */
          Scope(Scope* last, map<string,Symbol*> decs, map<string,Type*> types, ScopeType source);

          /**
           * @brief Search for a symbol by name; if found, return it, else, return NULL.
           * 
           * @param id Name of symbol to search for.
           * @return Symbol* The symbol, if found; else, NULL.
           */
          Symbol* LookupSymbol(const string &id);

          /**
           * @brief Search for a type by name; if found, return it, else, return NULL.
           * 
           * @param id Name of type to search for.
           * @return Type* The type, if found; else, NULL.
           */
          Type* LookupType(const string &id);

          /**
           * @brief Adds a symbol to the current scope. Returns false if
           * there was already a symbol in there of that name that couldn't be
           * overwritten; otherwise, returns true.
           * 
           * @param symbol The symbol to add to the scope.
           * @return true If successfully added.
           * @return false If could not add symbol.
           */
          bool AddSymbol(Symbol* symbol);

          /**
           * @brief Adds a type to the current scope. Returns false if
           * there was already a type in there of that name that couldn't be
           * overwritten; otherwise, returns true.
           * 
           * @param type The type to add to the scope.
           * @return true If successfully added.
           * @return false If could not add type.
           */
          bool AddType(Type* type);

          /**
           * @brief Removes a type from the current scope. Returns true if
           * successful; false if no such could type could be found and removed.
           * 
           * @param type The type to remove.
           * @return true If the type was found and removed.
           * @return false If no such type was found.
           */
          bool RemoveType(Type* type);

          /**
           * @brief Prints all symbols and types in the current scope.
           * 
           */
          void Print();

          /**
           * @brief Where the scope originated from.
           * 
           */
          ScopeType source;

          /**
           * @brief All variable and function symbols by name.
           * 
           */
          map<string, Symbol*> decs;

          /**
           * @brief All types by name.
           * 
           */
          map<string, Type*> types;

          /**
           * @brief The last scope that this stacks on top of.
           * 
           */
          Scope* last;
};

/************
 * SYMBOLS
 * **********/

/**
 * @brief Differentiates among kinds of symbols, either variable or function.
 * 
 */
enum SymbolKind{
     /**
      * @brief A variable symbol.
      * 
      */
     SYM_VAR,

     /**
      * @brief A function symbol.
      * 
      */
     SYM_FUNC
};

/**
 * @brief A symbol to represent a function or variable. Includes type, which is variable
 * type for variable symbols or return type for function symbols.
 * 
 */
class Symbol{
     public:
          /**
           * @brief Destroy the Symbol object
           * 
           */
          ~Symbol();

          /**
           * @brief Construct a new Symbol object with a name and kind.
           * 
           * @param name The name/identifier for the symbol.
           * @param kind Whether it is a function or variable symbol.
           */
          Symbol(const string &name, SymbolKind kind);

          /**
           * @brief Prints the name of the symbol.
           * 
           */
          virtual void Print();

          /**
           * @brief The name/identifier for the symbol.
           * 
           */
          string name;

          /**
           * @brief The type of a variable symbol, or return type of a function
           * symbol.
           * 
           */
          Type* type;

          /**
           * @brief The kind of symbol, either function or variable.
           * 
           */
          SymbolKind kind;

};

/**
 * @brief A symbol specific to variable identifiers.
 * 
 */
class VarSymbol : public Symbol {
     public:
          /**
           * @brief Construct a new Var Symbol object with a name, kind, type,
           * and whether or not it is read only.
           * 
           * @param name The name/identifier for the symbol.
           * @param kind The kind of the symbol, either function or variable.
           * @param type The type of a variable symbol, or return type of a function
           * @param readOnly Whether or not the symbol value can be changed at runtime.
           * False by default.
           */
          VarSymbol(const string &name, SymbolKind kind, Type* type, bool readOnly = false);

          /**
           * @brief Construct a new Var Symbol object with a name, kind, type,
           * value, and whether or not it is read only.
           * 
           * @param name The name/identifier for the symbol.
           * @param kind The kind of the symbol, either function or variable.
           * @param type The type of a variable symbol, or return type of a function
           * @param val The current value of the variable.
           * @param readOnly Whether or not the symbol value can be changed at runtime.
           * False by default.
           */
          VarSymbol(const string &name, SymbolKind kind, Type* type, Value* val, bool readOnly = false);
          
          /**
           * @brief Prints the name and type of the symbol.
           * 
           */
          void Print() override;

          /**
           * @brief Whether or not the variable can be modified at runtime.
           * 
           */
          bool readOnly;

          /**
           * @brief The current value of the variable.
           * 
           */
          Value* val;

};

/**
 * @brief A symbol specific to function identifiers, complete with arguments,
 * name, and return type.
 * 
 */
class FuncSymbol : public Symbol{
     public: 
          /**
           * @brief Construct a new Func Symbol object
           * 
           * @param name The name/identifier for the function.
           * @param kind The kind of the symbol, either function or variable.
           * @param type Return type of the function.
           * @param args Name->Type pairs for each of its formal parameters. 
           */
          FuncSymbol(const string &name, SymbolKind kind, Type* type, vector< pair<string,Type*> > args);
          
          /**
           * @brief Prints out the function name, return types, and all formal parameters.
           * 
           */
          void Print() override;

          /**
           * @brief Creates a new Scope object preloaded with symbols for the parameters from
           * this function.
           * 
           * @return Scope* The Scope created from the function's parameters.
           */
          Scope* CreateScopeFromParams();

          /**
           * @brief Name->Type pairs for each formal parameter.
           * 
           */
          vector< pair<string,Type*> > args;
};

/*****************
 * TYPES
 * **************/


/**
 * @brief The kind of type, whether primitive (string, int, unit),
 * a reference type, array type, or record type.
 * 
 */
enum TypeKind{

     /**
      * @brief A primitive type (string, int, or unit)
      * 
      */
     T_PRIM, //Primitive
     
     /**
      * @brief A reference to another type (e.g. type i = int)
      * 
      */
     T_REF, //Ref to other type
     
     /**
      * @brief An array of another type.
      * 
      */
     T_ARR, //Array
     
     /**
      * @brief A record with fields of other types.
      * 
      */
     T_REC //Record
};

/**
 * @brief A type in the Tiger language, denoted by name and kind.
 * 
 */
class Type{
     public:
          /**
           * @brief Construct a new Type object from a given name and kind.
           * 
           * @param name The name/identifier for the type.
           * @param kind The kind of type this is.
           */
          Type(const string &name, TypeKind kind);
          
          /**
           * @brief Destroy the Type object
           * 
           */
          ~Type();

          /**
           * @brief Prints out the type name.
           * 
           */
          virtual void Print();
          
          /**
           * @brief Returns true if this type is equal to another one. Used instead
           * of == because I'm too stupid to overload that.
           * 
           * @param rhs The other type to compare too.
           * @return true If the types are equal.
           * @return false If the types are not equal.
           */
          bool Equals(Type* rhs);


          //Returns the actual type that the type references;
          // i.e. if it is a reference or array type, it will return int or string
          // if it is a record type, it returns itself
          
          /**
           * @brief Returns the "actual type" of this type, meaning going down
           * the Reference or Array type's chain until it reaches a primitive or record type.
           * 
           * @return Type* The "actual" type of this type.
           */
          Type* GetActualType();

          /**
           * @brief Returns the "actual" TypeKind of this type; for reference types,
           * returns the TypeKind of its reference.
           * 
           * @return TypeKind 
           */
          TypeKind GetActualKind();
          
          /**
           * @brief The name of this type.
           * 
           */
          string name;

          /**
           * @brief The kind of this type, either primitive, reference,
           * array or record.
           * 
           */
          TypeKind kind;
};

/**
 * @brief A type that references another type.
 * 
 */
class RefType : public Type{
     public:
          /**
           * @brief Construct a new reference type with given name and kind.
           * 
           * @param name The name/identifier for the type.
           * @param kind The kind of type this is.
           */
          RefType(const string &name, TypeKind kind);

          /**
           * @brief Construct a new reference type with given name, kind
           * and type to reference.
           * 
           * @param name The name/identifier for the type.
           * @param kind The kind of type this is.
           * @param ref The type that this type references.
           */
          RefType(const string &name, TypeKind kind, Type* ref);
          
          /**
           * @brief Prints the type name and the type it references.
           * 
           */
          void Print() override;

          /**
           * @brief The type that this type references.
           * 
           */
          Type* ref;

};

/**
 * @brief A type that is an array of other types.
 * 
 */
class ArrType : public Type{
     public:
          /**
           * @brief Construct a new array type with a given name and kind.
           * 
           * @param name The name/identifier for the type.
           * @param kind The kind of type this is.
           */
          ArrType(const string &name, TypeKind kind);

          /**
           * @brief Construct a new array type with a given name, kind,
           * and type to reference.
           * 
           * @param name The name/identifier for the type.
           * @param kind The kind of type this is.
           * @param ref The type that this is an array of.
           */
          ArrType(const string &name, TypeKind kind, Type* ref);

          /**
           * @brief Prints the name of the type and type it is an array of.
           * 
           */
          void Print() override;

          /**
           * @brief The type that this is an array of.
           * 
           */
          Type* ref;
};

/**
 * @brief A record type with multiple fields, similar to a struct.
 * 
 */
class RecType : public Type{
     public:
          /**
           * @brief Construct a new RecordType with a given name and kind.
           * 
           * @param name The name/identifier for the type.
           * @param kind The kind of type this is.
           */
          RecType(const string &name, TypeKind kind);

          /**
           * @brief Construct a new RecordType with a given name, kind,
           * and collection of fields.
           * 
           * @param name The name/identifier for the type.
           * @param kind The kind of type this is.
           * @param fields Collection of Name->Type pairs for each field.
           */
          RecType(const string &name, TypeKind kind, map<string,Type*>* fields);
          
          /**
           * @brief Prints the name of the record only, because I'm lazy.
           * 
           */
          void Print() override;

          /**
           * @brief Collection of Name->Type pairs for each field.
           * 
           */
          map< string,Type*>* fields;

          /**
           * @brief True if the record type has a field with a given name.
           * 
           * @param name The name of the field to search for.
           * @return true If the record type has a field with the given name
           * @return false If the record type does not have a field with the given name
           */
          bool hasMember(const string &name);

          /**
           * @brief Returns a Name->Type pair for a field with a given name.
           * 
           * @param name The name of the field to pull out.
           * @return pair<string,Type*> The Name->Type pair for the found field.
           */
          pair<string,Type*> getFieldPair(const string &name);
};

/***********
 * VALUE
 * ********/

/**
 * @brief The kind of value that this is, either an integer, string,
 * record, array, or break (technically has no meaning).
 * 
 */
enum ValueType{
     /**
      * @brief An integer value.
      * 
      */
     V_INT,

     /**
      * @brief A string value.
      * 
      */
     V_STR,

     /**
      * @brief An array value.
      * 
      */
     V_ARR,

     /**
      * @brief A record value.
      * 
      */
     V_REC,

     /**
      * @brief A break value, special to break nodes and possibly unused.
      * 
      */
     V_BREAK
};

/**
 * @brief A value for a symbol; could be an integer, array, record, or string. And technically
 * a break.
 * 
 */
class Value {
     public:
          /**
           * @brief Construct a new Value object
           * 
           */
          Value();

          /**
           * @brief The kind of value.
           * 
           */
          ValueType kind;

          /**
           * @brief Prints nothing for the base type.
           * 
           */
          virtual void Print();

          /**
           * @brief Copies the current value depending on its ValueType and returns
           * the resulting value.
           * 
           * @return Value* 
           */
          virtual Value* Copy();

          /**
           * @brief Changes the internal value of this value to match the passed value.
           * 
           * @param other Value to match.
           */
          void SetValue(Value* other);

          /**
           * @brief Returns true if values are equal.
           * 
           * @param other Value to compare against.
           * @return true If the values are equal.
           * @return false If the values aren't equal.
           */
          virtual bool operator==(Value* other);

};

/**
 * @brief An integer value type.
 * 
 */
class IntValue : public Value
{
     public:
          /**
           * @brief Construct a new IntValue object initialized to a given value.
           * 
           * @param val Value to begin with.
           */
          IntValue(int val);

          /**
           * @brief Returns the value of this current IntValue.
           * 
           * @return int The value contained in this IntValue.
           */
          int GetValue();

          /**
           * @brief Sets the integer value to a new one.
           * 
           * @param inVal New value for this object.
           */
          void SetValue(const int &inVal);

          /**
           * @brief Set the integer value of this object to match another IntValue.
           * 
           * @param otherValue The other value to match.
           */
          void SetValue(IntValue* otherValue);

          /**
           * @brief Prints the current value on this object.
           * 
           */
          void Print() override;

          /**
           * @brief Returns a copy of this value.
           * 
           * @return Value* A new copy of this value.
           */
          Value* Copy() override;

          /**
           * @brief Returns true if values are equal.
           * 
           * @param other Value to compare against.
           * @return true If the values are equal.
           * @return false If the values aren't equal.
           */
          bool operator==(Value* other) override;

     private:
          /**
           * @brief The true integer value of this object.
           * 
           */
          int val;

};
/**
 * @brief A string value type.
 * 
 */
class StringValue : public Value
{
     public:
          /**
           * @brief Construct a new StringValue object initialized to a given value.
           * 
           * @param val Value to begin with.
           */
          StringValue(const string &val);

          /**
           * @brief Returns the value of this current StringValue.
           * 
           * @return string The value contained in this StringValue.
           */
          string GetValue();

          /**
           * @brief Sets the string value to a new one.
           * 
           * @param inVal New value for this object.
           */
          void SetValue(const string &inVal);

          /**
           * @brief Set the string value of this object to match another StringValue.
           * 
           * @param otherValue The other value to match.
           */
          void SetValue(StringValue* otherValue);

          /**
           * @brief Prints the current value on this object.
           * 
           */
          void Print() override;

          /**
           * @brief Returns a copy of this value.
           * 
           * @return Value* A new copy of this value.
           */
          Value* Copy() override;

          /**
           * @brief Returns true if values are equal.
           * 
           * @param other Value to compare against.
           * @return true If the values are equal.
           * @return false If the values aren't equal.
           */
          bool operator==(Value* other) override;

     private:
          /**
           * @brief The true string value of this object.
           * 
           */
          string val; 
};

/**
 * @brief An array value type, filled with reference to other values.
 * 
 */
class ArrayValue : public Value
{
     public:
          /**
           * @brief Construct a new ArrayValue with a given size and value to initialize
           * the entire array with.
           * 
           * @param size Size of array.
           * @param val Value to initialize the whole array with.
           */
          ArrayValue(const int &size, Value* val);

          /**
           * @brief Construct a new Array Value object with a given set of values; does
           * not deep copy the array.
           * 
           * @param val 
           */
          ArrayValue(vector<Value*> val);

          /**
           * @brief Get the value at a designated index.
           * 
           * @param index The index of the value.
           * @return Value* Value at given index.
           */
          Value* GetValue(const int &index);

          /**
           * @brief Get the vector of values contained in this array; does not
           * copy the inner values.
           * 
           * @return vector<Value*> Vector of values contained in this array.
           */
          vector<Value*> GetArray();

          /**
           * @brief Sets a value at a given index.
           * 
           * @param index Index of value to change.
           * @param val New value to go into index.
           */
          void SetValue(const int &index, Value* val);

          /**
           * @brief Sets the array of values to a new one; does not delete all reference
           * to values contained in the array.
           * 
           * @param val The vector of values to replace the old one.
           */
          void SetValue(vector<Value*> val);

          /**
           * @brief Prints all values in the array.
           * 
           */
          void Print() override;

          /**
           * @brief Deep copies the values in the array and returns a new ArrayValue.
           * 
           * @return Value* A new value with copied values.
           */
          Value* Copy() override;

          /**
           * @brief Returns true if values are equal.
           * 
           * @param other Value to compare against.
           * @return true If the values are equal.
           * @return false If the values aren't equal.
           */
          inline bool operator==(Value* other);

     private:
          /**
           * @brief Collection of all values in this array.
           * 
           */
          vector<Value*> val; 
};

/**
 * @brief A record value, with Name->Value pairs for each record field.
 * 
 */
class RecordValue : public Value
{
     public:
          /**
           * @brief Construct a blank RecordValue object; mostly used for NIL.
           * 
           */
          RecordValue();

          /**
           * @brief Construct a RecordValue with a collection field values.
           * 
           * @param val Name->Value pairs for each field.
           */
          RecordValue(map<string,Value*> val);

          /**
           * @brief Returns the value for a record's field given a name.
           * 
           * @param member Name of the field.
           * @return Value* The value for the named field.
           */
          Value* GetValue(const string &member);

          /**
           * @brief Get the map of Name->Value pairs for this record.
           * 
           * @return map<string,Value*> Map of Name->Value pairs for all fields of this record.
           */
          map<string,Value*> GetRecord();

          /**
           * @brief Set value of a named field.
           * 
           * @param key Name of the field.
           * @param inValue New value for the field.
           */
          void SetValue(const string &key, Value* inValue);

          /**
           * @brief Changes the map of Name->Value pairs to a new one; does not
           * delete all old values
           * 
           * @param val New map of fields
           */
          void SetValue(map<string, Value*> val);

          /**
           * @brief Prints all field names and values for the record.
           * 
           */
          void Print() override;

          /**
           * @brief Deep copies this record and its fields and returns the new RecordValue.
           * 
           * @return Value* The new copied RecordValue.
           */
          Value* Copy() override;

          /**
           * @brief Returns true if values are equal.
           * 
           * @param other Value to compare against.
           * @return true If the values are equal.
           * @return false If the values aren't equal.
           */
          inline bool operator==(Value* other);

     private:
          /**
           * @brief Map of name->value pairs for all fields.
           * 
           */
          map<string, Value*> val; 
};
/** @} */
#endif