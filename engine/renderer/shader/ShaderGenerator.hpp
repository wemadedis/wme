#pragma once
#include "AbstractSyntax.hpp"
#include "DescriptorSet.h"
#include "rte/RenderStructs.h"

namespace RTE::Rendering::Shaders
{
using namespace AbstractSyntax;
Variable* DefineVariable(Type* type, string name);
Struct* DefineStruct(string name, vector<pair<Type*,string>> variables);

vector<Struct*> GetStandardStructs();
vector<Declaration*> GetStandardVertexInputOutput();
vector<Declaration*> GetStandardFragmentInputOutput();
vector<Declaration*> GetStandardUniforms();

Program* GetStandardVertexShader();

void SaveShaderToFile(Program* shader);
void CompileShader(Program* shader);

void GenerateDummyCode();

class Shader
{
    const vector<string> standardTypes = {"sampler", "sampler2D", "samplerBuffer", "usamplerBuffer"};
public:
    class StructBuilder
    {
        string _name;
        vector<Declaration*> _members;
        StructBuilder(string structName);
    public:
        StructBuilder& WithMember(string type, string name, uint32_t arraySize = 1);
        Struct* Build();

        friend class Shader;
    };

    class ShaderBuilder
    {
        bool UsePhong = false;
        string _texture = "";
        string _normalMap = "";
        uint32_t _currentVarLocation;
        vector<Struct*> _structs;

        vector<Declaration*> _vertexResources;

        vector<Declaration*> _fragmentResources;

        DescriptorSet* _descriptorSet;
        //TODO: Wanna define custom vertex structure?
        ShaderBuilder(DescriptorSet* descriptorSet, Vertex vertex);
        void ProcessVertexInput(Vertex vertex);
        void ProcessDescriptorSet(DescriptorSet* descriptorSet);

    public:
        ShaderBuilder& WithPhong();
        ShaderBuilder& WithNormalMap(string textureDescriptorName);
        ShaderBuilder& WithTexture(string textureDescriptorName);
        ShaderBuilder& WithStruct(Struct* str);
        ShaderBuilder& SetDescriptorType(string descriptorName, string type);
        ShaderBuilder& WithVariable(string type, string name);
        Shader Build();

        friend class Shader;
    };

    static StructBuilder CreateStruct(string name);
    static ShaderBuilder Create(DescriptorSet* descriptorSet, Vertex* vertex);
    
};

}
    