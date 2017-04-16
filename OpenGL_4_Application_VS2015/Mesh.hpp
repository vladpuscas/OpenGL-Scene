//
//  Mesh.hpp
//  Lab4
//
//  Created by CGIS on 27/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include "GLEW/glew.h"
#include <string>
#include <vector>
#include "Shader.hpp"

namespace gps {

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    //ambientTexture, diffuseTexture, specularTexture
    std::string type;
    std::string path;
};

struct Material
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

	void Draw(gps::Shader shader);

private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

	// Initializes all the buffer objects/arrays
	void setupMesh();

};

}
#endif /* Mesh_hpp */
