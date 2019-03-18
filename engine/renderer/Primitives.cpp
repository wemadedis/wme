#include "rte/Primitives.h"
#include "rte/RenderStructs.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>


using namespace std;
using namespace glm;

namespace RTE::Renderer::Primitives
{

Mesh* MakeCylinder(float radius, float height, int faces){
    vec3 pos = vec3(0.0f,0.0f,radius);
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<uint16_t> indices;
    float angle = 0.0f;
    float step = glm::radians(360.0f/faces);
    //Top and bottom center vertices
    vertices.push_back(vec3(0,height/2.0f,0));
    normals.push_back(vec3(0,1,0));
    vertices.push_back(vec3(0,-height/2.0f,0));
    normals.push_back(vec3(0,-1,0));

    //Create vertices for the top and bottom of the cylinder
    for(int i = 0; i <= faces; i++){
        vec3 newPos = rotate(pos, angle, vec3(0,1,0));
        vertices.push_back(newPos+vec3(0,height/2.0f,0));
        normals.push_back(vec3(0,1,0));
        vertices.push_back(newPos-vec3(0,height/2.0f,0));
        normals.push_back(vec3(0,-1,0));
        angle += step;
    }
    for(uint32_t i = 2; i < vertices.size()-2; i += 2) {
        //Cylinder top triangles
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 2);

        //Cylinder bottom triangles (inverse order to be rendered from bottom view)
        indices.push_back(1);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }
    size_t x = vertices.size();
    //Create all vertices
    for(int i = 0; i <= faces; i++){
    vec3 newPos = rotate(pos, angle, vec3(0,1,0));
    vertices.push_back(newPos+vec3(0,height/2.0f,0));
    vec3 n = normalize(newPos);
    normals.push_back(n);
    //cout << n.x << " " << n.y << " " << n.z  << " Lenght: " << length(n) << endl;
    vertices.push_back(newPos-vec3(0,height/2.0f,0));
    normals.push_back(n);
    angle += step;
    }
    //Calculate indices and normals
    for(size_t i = x; i < vertices.size()-2; i += 2){
        //Cylinder side triangles
        indices.push_back(i);
        indices.push_back(i+1);
        indices.push_back(i+2);

        indices.push_back(i+1);
        indices.push_back(i+3);
        indices.push_back(i+2);

    }
    Mesh* cylinder = new Mesh();
    for(uint32_t i = 0; i < vertices.size(); i++){
        Vertex v = {vertices[i], vec3(0.5f), normals[i]};
        cylinder->vertices.push_back(v);
    }
    cylinder->indices = indices;
    return cylinder;
}

};