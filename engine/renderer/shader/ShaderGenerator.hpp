#pragma once
#include "AbstractSyntax.hpp"
#include "DescriptorSet.h"
#include "rte/RenderStructs.h"

namespace RTE::Rendering::Shaders
{
using namespace AbstractSyntax;

void SaveShaderToFile(Program* shader);
void CompileShader(Program* shader);


class Shader
{
public:
    class StructBuilder
    {
        string _name;
        vector<Declaration*> _members;
        StructBuilder(string structName);
    public:
        StructBuilder& WithMember(string type, string name, uint32_t arraySize = 1);
        Struct* Build(bool isUniform = false);

        friend class Shader;
    };

    class ShaderBuilder
    {
        const vector<string> standardTypes = {"sampler", "sampler2D", "samplerBuffer", "usamplerBuffer"};
        bool UsePhong = false;
        string _texture = "";
        string _normalMap = "";
        uint32_t _currentVarLocation = 0;

        unordered_map<string, Struct*> _structs;
        unordered_map<string, Uniform*> _uniforms;

        vector<Declaration*> _vertexResources;

        vector<Declaration*> _fragmentResources;

        DescriptorSet* _descriptorSet;
        //TODO: Wanna define custom vertex structure?
        ShaderBuilder(DescriptorSet* descriptorSet, Vertex vertex);
        void CreateDefaultStructs();
        void ProcessVertexInput(Vertex vertex);
        void ProcessDescriptorSet(DescriptorSet* descriptorSet);
        void CreateStandardInputOutput();

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
    