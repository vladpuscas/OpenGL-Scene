//
//  Model3D.hpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Model3D_hpp
#define Model3D_hpp

#include <iostream>
#include <string>
#include <vector>

#include "Mesh.hpp"

#include "tiny_obj_loader.h"
#include "stb_image.h"

namespace gps {

    class Model3D
    {

    public:
		Model3D();

		Model3D(std::string fileName);

		Model3D(std::string fileName, std::string basePath);

		void Draw(gps::Shader shaderProgram);

    private:
		// Component meshes - group of objects
        std::vector<gps::Mesh> meshes;
		// Associated textures
        std::vector<gps::Texture> loadedTextures;

		// Does the parsing of the .obj file and fills in the data structure
		void ReadOBJ(std::string fileName, std::string basePath);

		// Retrieves a texture associated with the object - by its name and type
		gps::Texture LoadTexture(std::string path, std::string type);

		// Reads the pixel data from an image file and loads it into the video memory
		GLuint ReadTextureFromFile(const char* file_name);
    };
}

#endif /* Model3D_hpp */
