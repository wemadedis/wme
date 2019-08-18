#include "AbstractSyntax.hpp"

namespace RTE::Rendering::Shaders::AbstractSyntax
{
using namespace std;

static const string tab = "    ";

Type::Type(string name)
{
    _typeName = name;
}

string Type::ToString()
{
    return _typeName;
}

string Declaration::GetName()
{
    return _name;
}

ExpressionStatement::ExpressionStatement(Expression *expression)
{
    _expression = expression;
}

string ExpressionStatement::ToString()
{
    return _expression->ToString();
}

Program::Program(vector<Declaration*> declarations)
{
    _declarations = declarations;
}

string Program::ToString()
{
    string str = "";
    for(uint32_t declarationIndex = 0; declarationIndex < _declarations.size(); declarationIndex++)
    {
        str += _declarations[declarationIndex]->ToString() + "\n";
    }
    return str;
}

ArrayType::ArrayType(Type* type) : Type(type->ToString())
{
    _type = type;
    Size = 0;
}

ArrayType::ArrayType(Type* type, uint32_t size) : Type(type->ToString())
{
    //Dynamic sized arrays are determined at construction time, so can only make them with the previous constructor
    if(size == 0) throw "Array cannot be of size 0";
    _type = type;
    Size = size;
}

string ArrayType::ToString()
{
    return _type->ToString();
}


Void::Void() : Type("void") {}

Bool::Bool() : Type("bool") {}

Int::Int() : Type("int") {}

UInt::UInt() : Type("uint") {};

Float::Float() : Type("float") {}

Char::Char() : Type("char") {}

Vec2::Vec2() : Type("vec2") {}

Vec3::Vec3() : Type("vec3") {}

Vec4::Vec4() : Type("vec4") {}

Mat3::Mat3() : Type("mat3") {}

Mat4::Mat4() : Type("mat4") {}

ConstantInt::ConstantInt(int value)
{
    _value = value;
}

string ConstantInt::ToString()
{
    return to_string(_value);
}

ConstantFloat::ConstantFloat(float value)
{
    _value = value;
}

string ConstantFloat::ToString()
{
    return to_string(_value);
}


ConstantChar::ConstantChar(char value)
{
    _value = value;
}

string ConstantChar::ToString()
{
    return to_string(_value);
}

Return::Return()
{
    _returnExpression = nullptr;
}

Return::Return(Expression* expression)
{
    _returnExpression = expression;
}

string Return::ToString()
{
    string str = "return";
    if(_returnExpression != nullptr)
    {
        str += + " " + _returnExpression->ToString();
    }
    return str;
}

Block::Block(vector<Declaration*> declarations, vector<Statement*> statements)
{
    _declarations = declarations;
    _statements = statements;
}

string Block::ToString()
{
    string str = "{\n";
    for (auto declaration : _declarations)
    {
        str += tab +  declaration->ToString() + ";\n";
    }
    for (auto statement : _statements)
    {
        str += tab + statement->ToString() + ";\n";
    }
    str += "}\n";
    return str;
}


While::While(Expression* predicate, Block* body)
{
    _predicate = predicate;
    _body = body;
}

string While::ToString()
{
    string str = "while(" + _predicate->ToString() + ")\n";
    str += _body->ToString();
    return str;
}

For::For(Expression* init, Expression* predicate, Expression* inc, Block* body)
{
    _init = init;
    _predicate = predicate;
    _increment = inc;
    _body = body;
}

string For::ToString()
{
    string str = "for(" + _init->ToString() + "; " + _predicate->ToString() + "; " + _increment->ToString() + ")\n";
    str += _body->ToString();
    return str;
}

Variable::Variable(Type *type, string name)
{
    _type = type;
    _name = name;
}

string Variable::ToString()
{
    string str = _type->ToString() + " " + _name;
    auto array = dynamic_cast<ArrayType*>(_type);
    if(array != nullptr)
    {
        if(array->Size == 0) str += "[]";
        else str += "[" + to_string(array->Size) + "]";
    }
    return str;
}

InVariable::InVariable(Type *type, string name, uint32_t location) : Variable(type, name) 
{
    _location = location;
}

string InVariable::ToString()
{
    return "layout(location = " + to_string(_location) +") in " + Variable::ToString() + ";";
}

OutVariable::OutVariable(Type *type, string name, uint32_t location) : Variable(type, name) 
{
    _location = location;
}

string OutVariable::ToString()
{
    return "layout(location = " + to_string(_location) +") out " + Variable::ToString() + ";";
}

Uniform::Uniform(Type* type, string name, uint32_t set, uint32_t binding) : Variable(type, name)
{
    _set = set;
    _binding = binding;
}

string Uniform::ToString()
{
    string layout = "layout(set = " + to_string(_set) + ", binding = " + to_string(_binding) + ") ";
    string typeName = "uniform " + Variable::ToString();
    return layout + typeName + ";";
}

void Uniform::SetType(Type* type)
{
    _type = type;
}

AccessVariable::AccessVariable(string variableName)
{
    _variableName = variableName;
}

string AccessVariable::ToString()
{
    return _variableName;
}    

AccessMember::AccessMember(Expression* objectExpression, string memberName)
{
    _objectExpression = objectExpression;
    _memberName = memberName;
}

string AccessMember::ToString()
{
    return _objectExpression->ToString() + "." + _memberName;
}

AssignToVariable::AssignToVariable(Access *accessVariable, Expression *expression)
{
    _accessVariable = accessVariable;
    _expression = expression;
}

string AssignToVariable::ToString()
{
    return _accessVariable->ToString() + " = " + _expression->ToString(); //Newline unnecessary here?
}

BinaryOperator::BinaryOperator(char op, Expression* left, Expression* right)
{
    _operator = op;
    _left = left;
    _right = right;
}

string BinaryOperator::ToString()
{
    return _left->ToString() + " " + _operator + " " + _right->ToString();
}

Function::Function(Type *returnType, string name, vector<Variable> parameters, Statement *statement)
{
    _returnType = returnType;
    _name = name;
    _parameters = parameters;
    _statement = statement;
}

string Function::ToString()
{
    string str = _returnType->ToString() + " " + _name + "(";
    for(uint32_t parameterIndex = 0; parameterIndex < _parameters.size(); parameterIndex++)
    {
        str += _parameters[parameterIndex].ToString();
        if(parameterIndex < _parameters.size()-1)
        {
            str += ", ";
        }
    }
    str += ")\n" + _statement->ToString();
    return str;
}   


FunctionCall::FunctionCall(string name, vector<Expression*> parameters)
{
    _name = name;
    _parameters = parameters;
}

string FunctionCall::ToString()
{
    string str = _name + "(";
    for(uint32_t parameterIndex = 0; parameterIndex < _parameters.size(); parameterIndex++)
    {
        str += _parameters[parameterIndex]->ToString();
        if(parameterIndex < _parameters.size()-1)
        {
            str += ", ";
        }
    }
    str += ")";
    return str;
}


Struct::Struct(string name, vector<Declaration*> declarations, bool isUniform) : Type(name)
{
    _name = name;
    _declarations = declarations;
    _isUniform = isUniform;
}

string Struct::ToString()
{

    string str;
    if(!_isUniform) str += "struct ";
    str += _name + "\n{\n";
    for(uint32_t declarationIndex = 0; declarationIndex < _declarations.size(); declarationIndex++)
    {
        str += tab + _declarations[declarationIndex]->ToString() + ";\n";
    }
    if(!_isUniform) str += "};\n";
    else str += "}";
    return str;
}

bool Struct::IsUniform()
{
    return _isUniform;
}

FreeCode::FreeCode(string* code)
{
    _code = code;
}

string FreeCode::ToString()
{
    return *_code;
}

} // namespace RTE::Rendering::Shader
