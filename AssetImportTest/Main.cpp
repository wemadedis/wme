#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>

int main()
{
	const std::string& pFile = "C:\\Users\\Freemedude\\Desktop\\test\\test.blend";
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);


	// If the import failed, report it
	if (!scene)
	{
		std::cout << "!SCENE NOOOOOOOOOOO" << std::endl;
		return false;
	}
	// We're done. Everything will be cleaned up by the importer destructor
	
	std::cout << "Success!" << std::endl;
	return true;
}