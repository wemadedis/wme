#include "ShaderGenerator.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

namespace RTE::Rendering::Shaders
{

typedef Shader::StructBuilder StructBuilder;
typedef Shader::ShaderBuilder ShaderBuilder;

//------------------------StructBuilder-----------------------------------

StructBuilder::StructBuilder(string structName)
{
    _name = structName;
}

StructBuilder& StructBuilder::WithMember(string type, string name, uint32_t arraySize)
{
    Type* _type = arraySize == 1 ? new Type(type) : new ArrayType(new Type(type), arraySize);
    _members.push_back(new Variable(_type, name));
    return *this;
}

Struct* StructBuilder::Build(bool isUniform)
{
    return new Struct(_name, _members, isUniform);
}

StructBuilder Shader::CreateStruct(string name)
{
    return StructBuilder(name);
}


//------------------------ShaderBuilder-----------------------------------


ShaderBuilder::ShaderBuilder(DescriptorSet* descriptorSet, Vertex vertex)
{
    CreateDefaultStructs();
    ProcessVertexInput(vertex);
    ProcessDescriptorSet(descriptorSet);
    CreateStandardInputOutput();
}

void ShaderBuilder::CreateDefaultStructs()
{
    Struct* str = Shader::CreateStruct("DirectionalLight")
                                    .WithMember("vec4", "Color")
                                    .WithMember("vec4", "Direction")
                                    .Build();
    _structs.insert({str->GetName(), str});
    str = Shader::CreateStruct("PointLight")
                                    .WithMember("vec4", "Color")
                                    .WithMember("vec4", "PositionRadius")
                                    .Build();
    _structs.insert({str->GetName(), str});
    str = Shader::CreateStruct("TransformData")
                                    .WithMember("mat4", "ModelMatrix")
                                    .WithMember("mat4", "NormalMatrix")
                                    .WithMember("mat4", "MVPMatrix")
                                    .Build();
    _structs.insert({str->GetName(), str});
    str = Shader::CreateStruct("SurfaceData")
                                    .WithMember("float", "Ambient")
                                    .WithMember("float", "Diffuse")
                                    .WithMember("float", "Specular")
                                    .WithMember("float", "Shininess")
                                    .WithMember("float", "Reflectivity")
                                    .WithMember("float", "Transparency")
                                    .WithMember("vec4", "Color")
                                    .WithMember("uint", "Texture")
                                    .WithMember("bool", "HasTexture")
                                    .Build();
    _structs.insert({str->GetName(), str});
    str = Shader::CreateStruct("CameraData")
                                    .WithMember("float", "FoV")
                                    .WithMember("float", "NearPlane")
                                    .WithMember("float", "FarPlane")
                                    .WithMember("vec4", "Position")
                                    .WithMember("vec4", "ClearColor")
                                    .WithMember("mat4", "ViewMatrix")
                                    .WithMember("mat4", "ProjectionMatrix")
                                    .Build();
    _structs.insert({str->GetName(), str});
    str = Shader::CreateStruct("LightData")
                                    .WithMember("PointLight", "PointLights", 10)
                                    .WithMember("uint", "PointLightCount")
                                    .WithMember("DirectionalLight", "DirectionalLights", 10)
                                    .WithMember("uint", "DirectionalLightCount")
                                    .Build();
    _structs.insert({str->GetName(), str});
    str = Shader::CreateStruct("InstanceData")
                                    .WithMember("TransformData", "Transform")
                                    .WithMember("SurfaceData", "Surface")
                                    .Build(true);
    _structs.insert({str->GetName(), str});
    str = Shader::CreateStruct("WorldData")
                                    .WithMember("CameraData", "Camera")
                                    .WithMember("LightData", "Lights")
                                    .Build(true);
    _structs.insert({str->GetName(), str});
}

void ShaderBuilder::ProcessVertexInput(Vertex vertex)
{
    _vertexResources.push_back(new InVariable(new Vec3(), "in_position", _currentVarLocation++));
    _vertexResources.push_back(new InVariable(new Vec4(), "in_color", _currentVarLocation++));
    _vertexResources.push_back(new InVariable(new Vec3(), "in_normal", _currentVarLocation++));
    _vertexResources.push_back(new InVariable(new Vec2(), "in_UV", _currentVarLocation++));
}

void ShaderBuilder::ProcessDescriptorSet(DescriptorSet* descriptorSet)
{
    _descriptorSet = descriptorSet;
    for(auto binding : descriptorSet->GetBindingMap())
    {
        string name = binding.first;
        uint32_t bindingIndex = binding.second.Binding.binding;
        uint32_t set = binding.second.LayoutIndex;
        _uniforms.insert({name, new Uniform(nullptr, name, set, bindingIndex)});
    }
    //Standard descriptors
    SetDescriptorType("Instance", "InstanceData");
    SetDescriptorType("World", "WorldData");
    SetDescriptorType("Texture", "sampler2D");
}


void ShaderBuilder::CreateStandardInputOutput()
{
    WithVariable("vec3", "out_normal");
    WithVariable("vec3", "out_eye");
    WithVariable("vec2", "out_UV");
    WithVariable("vec3", "out_position_viewspace");
}

ShaderBuilder& ShaderBuilder::WithPhong()
{
    UsePhong = true;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithNormalMap(string textureDescriptorName)
{
    _normalMap = textureDescriptorName;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithTexture(string textureDescriptorName)
{
    _texture = textureDescriptorName;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithStruct(Struct* str)
{
    _structs.insert({str->GetName(), str});
    return *this;
}

ShaderBuilder& ShaderBuilder::SetDescriptorType(string descriptorName, string type)
{
    auto descriptorInfo = _descriptorSet->GetBindingMap()[descriptorName];
    auto stages = descriptorInfo.Binding.stageFlags;

    Uniform* uni = _uniforms[descriptorName];
    
    if(find_if(standardTypes.begin(), standardTypes.end(), [type](string str){ return str.compare(type) == 0; }) != standardTypes.end())
    {
        uni->SetType(new Type(type));
    }
    else if(_structs.find(type) != _structs.end())
    {
        uni->SetType(_structs[type]);
    }
    else
    {
        throw RTEException("Trying to set a descriptor to an unkown type!");
    }
    
    if(stages & VK_SHADER_STAGE_VERTEX_BIT)
    {
        _vertexResources.push_back(uni);
    }
    if(stages & VK_SHADER_STAGE_FRAGMENT_BIT)
    {
        _fragmentResources.push_back(uni);
    }

    return *this;
}

ShaderBuilder& ShaderBuilder::WithVariable(string type, string name)
{
    uint32_t location = _currentVarLocation++;
    _vertexResources.push_back(new OutVariable(new Type(type), name, location));
    _fragmentResources.push_back(new InVariable(new Type(type), name, location));
    return *this;
}

Shader ShaderBuilder::Build()
{
    vector<Declaration*> declarations;
    
    for(auto strpair : _structs)
    {
        if(!strpair.second->IsUniform()) declarations.push_back(strpair.second);
    }
    for(auto res : _vertexResources) declarations.push_back(res);
    
    FreeCode* phong_vertex = new FreeCode(new string(
        "void CalculatePhongComponents()\n"
        "{\n"
        "   mat4 modelView = Camera.ViewMatrix * Transform.ModelMatrix;\n"
        "   vec4 position = vec4(in_position,1.0f)\n"
        "   out_position_viewspace = vec3(modelView*position);\n"
        "   vec4 normal = Transform.NormalMatrix * vec4(in_normal);\n"
        "   out_normal = normalize(vec3(normal));\n"
        "   out_eye = normalize(out_position_viewspace);\n"
        "   if(dot(out_normal, out_eye) > 0) out_normal = -out_normal;\n"
        "}\n"
    ));

    FreeCode* main = new FreeCode(new string(
        "void main()\n"
        "{\n"
        "   gl_Position = Transform.MVPMatrix * vec4(in_position,1.0f);\n"
        "   CalculatePhongComponents();\n"
        "   out_UV = in_UV;\n"
        "}\n"
    ));

    declarations.push_back(phong_vertex);
    declarations.push_back(main);
    Program vert = Program(declarations);

    cout << vert.ToString() << endl;
    return Shader();
}

ShaderBuilder Shader::Create(DescriptorSet* descriptorSet, Vertex* vertex)
{
    return ShaderBuilder(descriptorSet, *vertex);
}

}