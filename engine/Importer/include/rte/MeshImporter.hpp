#pragma once

#include "IMeshImporter.hpp"

#include <string>

namespace RTE::Importing
{
class MeshImporter : public IMeshImporter
{
public:
    Rendering::Mesh ImportMesh(std::string file) override;
};

} // namespace RTE::Importing