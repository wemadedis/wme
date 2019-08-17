#include "ShaderGenerator.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

namespace RTE::Rendering::Shaders
{

typedef Shader::StructBuilder StructBuilder;
typedef Shader::ShaderBuilder ShaderBuilder;

Variable* DefineVariable(Type* type, string name)
{
    return new Variable(type, name);
}

Struct* DefineStruct(string name, vector<pair<Type*,string>> variables)
{
    vector<Declaration*> vars;
    for(auto var : variables)
    {
        vars.push_back(DefineVariable(var.first, var.second));
    }
    return new Struct(name, vars);
}

vector<Struct*> GetStandardStructs()
{
    vector<Struct*> structs;

    Struct* directionalLight = Shader::CreateStruct("DirectionalLight")
                                    .WithMember("vec4", "Color")
                                    .WithMember("vec4", "Direction")
                                    .Build();
    structs.push_back(directionalLight);

    Struct* pointLight =  Shader::CreateStruct("PointLight")
                                    .WithMember("vec4", "Color")
                                    .WithMember("vec4", "PositionRadius")
                                    .Build();
    structs.push_back(pointLight);

    Struct* transformData = Shader::CreateStruct("Transform")
                                    .WithMember("mat4", "ModelMatrix")
                                    .WithMember("mat4", "NormalMatrix")
                                    .WithMember("mat4", "MVPMatrix")
                                    .Build();
    structs.push_back(transformData);

    Struct* SurfaceData = Shader::CreateStruct("Surface")
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
    structs.push_back(SurfaceData);

    Struct* CameraData = Shader::CreateStruct("Camera")
                                    .WithMember("float", "FoV")
                                    .WithMember("float", "NearPlane")
                                    .WithMember("float", "FarPlane")
                                    .WithMember("vec4", "Position")
                                    .WithMember("vec4", "ClearColor")
                                    .WithMember("mat4", "ViewMatrix")
                                    .WithMember("mat4", "ProjectionMatrix")
                                    .Build();
    structs.push_back(CameraData);

    Struct* WorldData = Shader::CreateStruct("World")
                                    .WithMember("PointLight", "PointLights", 10)
                                    .WithMember("uint", "PointLightCount")
                                    .WithMember("DirectionalLight", "DirectionalLights", 10)
                                    .WithMember("uint", "DirectionalLightCount")
                                    .Build();
    structs.push_back(WorldData);

    return structs;
}

vector<Declaration*> GetStandardVertexInputOutput()
{
    vector<Declaration*> inoutVariables;
    inoutVariables.push_back(new InVariable(new Vec3(), "in_position", 0));
    inoutVariables.push_back(new InVariable(new Vec4(), "in_color", 1));
    inoutVariables.push_back(new InVariable(new Vec3(), "in_normal", 2));
    inoutVariables.push_back(new InVariable(new Vec2(), "in_UV", 3));

    inoutVariables.push_back(new OutVariable(new Vec3(), "out_normal", 0));
    inoutVariables.push_back(new OutVariable(new Vec3(), "out_eye", 1));
    inoutVariables.push_back(new OutVariable(new Vec2(), "out_UV", 2));
    inoutVariables.push_back(new OutVariable(new Vec3(), "out_position_viewspace", 3));

    return inoutVariables;
}

vector<Declaration*> GetStandardFragmentInputOutput()
{
    return {};
}

vector<Declaration*> GetStandardUniforms()
{
    vector<Declaration*> uniforms;

    uniforms.push_back(new Uniform(new Type("Transform"), "Transform", 0, 0));
    uniforms.push_back(new Uniform(new Type("Surface"), "Surface", 0, 1));
    uniforms.push_back(new Uniform(new Type("Camera"), "Camera", 0, 2));
    uniforms.push_back(new Uniform(new Type("World"), "World", 0, 3));

    return uniforms;
}


Program* GetStandardVertexShader()
{
    auto structs = GetStandardStructs();
    auto inout = GetStandardVertexInputOutput();
    auto uniforms = GetStandardUniforms();

    vector<Declaration*> decs;
    
    for(auto* str : structs)
    {
        decs.push_back(str);
    }

    for(auto* io : inout)
    {
        decs.push_back(io);
    }

    for(auto* uni : uniforms)
    {
        decs.push_back(uni);
    }


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

    decs.push_back(phong_vertex);
    decs.push_back(main);

    return new Program(decs);
}

void GenerateDummyCode()
{
    Program *p = GetStandardVertexShader();
    cout << p->ToString() << endl;
}

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

Struct* StructBuilder::Build()
{
    return new Struct(_name, _members);
}

StructBuilder Shader::CreateStruct(string name)
{
    return StructBuilder(name);
}


//------------------------ShaderBuilder-----------------------------------


ShaderBuilder::ShaderBuilder(DescriptorSet* descriptorSet, Vertex vertex)
{
    ProcessVertexInput(vertex);
    ProcessDescriptorSet(descriptorSet);
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
        
    }
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
    _structs.push_back(str);
    return *this;
}

ShaderBuilder& ShaderBuilder::SetDescriptorType(string descriptorName, string type)
{
    auto descriptorInfo = _descriptorSet->GetBindingMap()[descriptorName];
    uint32_t binding = descriptorInfo.LayoutIndex;
    uint32_t set = descriptorInfo.Binding.binding;
    auto stages = descriptorInfo.Binding.stageFlags;
    /*if(find_if(standardTypes.begin(), standardTypes.end(), [type](string stdType){ return stdType.compare(type) == 0; }) == standardTypes.end())
    {
        if(find_if(_structs.begin(), _structs.end(), [type](Struct* str){ return str->GetName().compare(type) == 0; }) == _structs.end())
        {
            throw RTEException("Trying to set descriptor to an unknown type!");
        }
    }
*/
    Uniform* uni = new Uniform(new Type(type), descriptorName, set, binding);
    
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
    _vertexResources.push_back(new OutVariable(new Type(type), name, _currentVarLocation++));
    _fragmentResources.push_back(new InVariable(new Type(type), name, _currentVarLocation++));
    return *this;
}



}