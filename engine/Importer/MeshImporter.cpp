#include "rte/MeshImporter.hpp"

#include <fstream>
#include <sstream>

#include "rte/Utility.hpp"

inline bool FirstTwoInStringEquals(std::string str, char a, char b)
{
    return str[0] == a && str[1] == b;
}

namespace RTE::Importing
{
Rendering::Mesh MeshImporter::ImportMesh(std::string file)
{
    Rendering::Mesh mesh;

    std::string line;
    std::ifstream stream;

    stream.open(file);
    if (!stream.is_open())
    {
        Error("Failed to open file");
        return mesh;
    }

    while (getline(stream, line))
    {
        // Parse
        if (FirstTwoInStringEquals(line, 'v', ' '))
        {
            std::stringstream lineStream(line);

            Rendering::Vertex v;
            float found = 0;
            while(!lineStream.eof())
            {
                // lineStream >> found;
                // mesh.Vertices.push_back(found);
            }
        }
        if (FirstTwoInStringEquals(line, 'v', 'n'))
        {
            // Normals
        }
        if (FirstTwoInStringEquals(line, 'f', ' '))
        {
            // Face
        }
    }

    stream.close();

    return mesh;
}
} // namespace RTE::Importing
