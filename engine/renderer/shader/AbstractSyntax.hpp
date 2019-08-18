#pragma once

#include <string>
#include <vector>

namespace RTE::Rendering::Shaders::AbstractSyntax
{
using namespace std;

class ToStringClass
{
public:
    virtual string ToString() = 0;
};

class Type : public ToStringClass 
{
protected:
    string _typeName;
public:
    Type(string name);
    string ToString();
};

class Expression : public ToStringClass 
{
public:
    virtual string ToString() = 0;
};

class Statement : public ToStringClass 
{
public:
    virtual string ToString() = 0;
};

class Declaration : public ToStringClass 
{
protected:
    string _name;
public:
    string GetName();
    virtual string ToString() = 0;
};


class Access : public Expression 
{
public:
    virtual string ToString() = 0;
};


class ExpressionStatement : public Statement 
{
    Expression *_expression;
public:
    ExpressionStatement(Expression *expression);

    string ToString();
};

class Program : public ToStringClass
{
    vector<Declaration*> _declarations;
public:
    Program(vector<Declaration*> declarations);

    string ToString();
    
};

class ArrayType : public Type
{
    Type* _type;
public:
    uint32_t Size;
    //For dynamic arrays
    ArrayType(Type* type);
    //For constant sized arrays
    ArrayType(Type* type, uint32_t size);

    string ToString();
};

class Void : public Type 
{
public:
    Void();
};

class Bool : public Type 
{
public:
    Bool();
};

class Int : public Type 
{
public:
    Int();
};

class UInt : public Type 
{
public:
    UInt();
};

class Float : public Type 
{
public:
    Float();
};

class Char : public Type
{
public:
    Char();
};

class Vec2 : public Type 
{
public:
    Vec2();
};

class Vec3 : public Type 
{
public:
    Vec3();
};

class Vec4 : public Type 
{
public:
    Vec4();
};

class Mat3 : public Type 
{
public:
    Mat3();
};

class Mat4 : public Type 
{
public:
    Mat4();
};

class ConstantInt : public Expression 
{ 
    int _value; 
public:
    ConstantInt(int value);

    string ToString();
};

class ConstantFloat : public Expression 
{ 
    float _value;
public:
    ConstantFloat(float value);

    string ToString();
};

class ConstantChar : public Expression 
{ 
    char _value; 
public:
    ConstantChar(char value);

    string ToString();
};

class Return : public Statement
{
    Expression* _returnExpression;
public:
    //void
    Return();
    //any other type
    Return(Expression* expression);

    string ToString();
};

class Block : public Statement 
{
    vector<Declaration*> _declarations;
    vector<Statement*> _statements;
public:
    Block(vector<Declaration*> declarations, vector<Statement*> statements);

    string ToString();
};


class While : public Statement
{
    Expression* _predicate;
    Statement* _body;
public:
    While(Expression* predicate, Block* body);

    string ToString();
};

class For : public Statement
{
    Expression* _init;
    Expression* _predicate;
    Expression* _increment;
    Statement* _body;
public:
    For(Expression* init, Expression* predicate, Expression* inc, Block* body);

    string ToString();
};

class Variable : public Declaration
{
protected:
    Type *_type;
public:
    Variable(Type *type, string name);

    string ToString();
};

class InVariable : public Variable
{
    uint32_t _location;
public:
    InVariable(Type *type, string name, uint32_t location);

    string ToString();
};

class OutVariable : public Variable
{
    uint32_t _location;
public:
    OutVariable(Type *type, string name, uint32_t location);

    string ToString();
};


class Uniform : public Variable
{
    uint32_t _set, _binding;
public:
    Uniform(Type* type, string name, uint32_t set, uint32_t binding);
    string ToString();
    void SetType(Type* type);
};

class AccessVariable : public Access
{
    string _variableName;
public:
    AccessVariable(string variableName);
    string ToString();
};

class AccessMember : public Access
{
    Expression* _objectExpression;
    string _memberName;
public:
    AccessMember(Expression* objectExpression, string memberName);
    string ToString();
};

class AssignToVariable : public Expression
{
    Access *_accessVariable;
    Expression *_expression;
public:
    AssignToVariable(Access *accessVariable, Expression *expression);

    string ToString();
};

class BinaryOperator : public Expression
{
    char _operator;
    Expression* _left;
    Expression* _right;
public:
    BinaryOperator(char op, Expression* left, Expression* right);

    string ToString();
};

class Function : public Declaration
{
    Type *_returnType;
    vector<Variable> _parameters;
    Statement *_statement;
public:
    Function(Type *returnType, string name, vector<Variable> parameters, Statement *statement);

    string ToString();
};

class FunctionCall : public Expression
{
    string _name;
    vector<Expression*> _parameters;
public:
    FunctionCall(string name, vector<Expression*> parameters);

    string ToString();
};

class Struct : public Declaration, public Type
{
    vector<Declaration*> _declarations;
    bool _isUniform = false;
public:
    Struct(string name, vector<Declaration*> declarations, bool isUniform = false);
    string ToString();

    bool IsUniform();
};

class FreeCode : public Declaration
{
    string* _code = nullptr;
public:
    FreeCode(string* code);

    string ToString();
};



} // namespace RTE::Rendering::Shader
