#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "SOIL2/SOIL2.h"

#include "Mesh.h"

GLint TextureFromFile(const char *path, std::string directory);

class Model
{
public:
    // Constructor, path is a filepath to a 3D model
    Model(GLchar *path)
    {
        loadModel(path);
    }
    
    // Draw the model by drawing each mesh
    void Draw(Shader shader)
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
        {
            this->meshes[i].Draw(shader);
        }
    }
    
private:
    /* Model Data */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded; // stores all the textures loaded so far, ensures no duplication of textures
    
    // Load a model with support ASSIMP extensions from file and stores the resulting meshes in this->meshes
    void loadModel(std::string path)
    {
        // Read file using assimp
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        // Check for errors
        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        
        // Retrieve the directory path of the filepath
        this->directory = path.substr(0, path.find_last_of('/'));
        
        // Process ASSIMP root node recursively
        this->processNode(scene->mRootNode, scene);
    }
    
    // Process a node recursively. Process each mesh on the node, then repeat for all children
    void processNode(aiNode *node, const aiScene *scene)
    {
        // Process each mesh on the node
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            // Obtain the mesh from the scene using the index stored in the node
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            // Add the processed mesh to meshes vector.
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        
        // Now process any child nodes
        for (GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }
    }
    
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        /* Mesh Data */
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;
        
        // Process each vertex
        for (GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;
            
            // Position
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            // Normal
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            
            // If the texture has texcoords
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                
                vertex.TexCoords = vec;
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f); // Default to 0 if no texCoords
            }
            // Now add the vertex!
            vertices.push_back(vertex);
        }
        
        // For each face, add all indices to the indices vector
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for (GLuint j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        
        // Process materials
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            // Diffuse Maps
            // Again(see mesh.h) assume texture naming convention, texture_diffuseN... etc
            std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            
            // Specular Maps
            std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        // Return the mesh object using the extracted data
        return Mesh(vertices, indices, textures);
    }
    
    // Check all materials of a given type and load if not already loaded.
    // return as a vector of Texture structs
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            
            // Check if texture was loaded before, if so, skip loading a new texture.
            GLboolean skip = false;
            for (const auto &texture : textures_loaded)
            {
                if (texture.Path == str) {
                    textures.push_back(texture);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one.
                    break;
                }
            }
            
            // If it hasn't been loaded, then loaded
            if (!skip) {
                Texture texture;
                texture.Id = TextureFromFile(str.C_Str(), this->directory);
                texture.Type = typeName;
                texture.Path = str;
                textures.push_back(texture);
                
                this->textures_loaded.push_back(texture); // store it for entire model to ensure no duplication occurs
            }
        }
        return textures;
    }
};

// Return a textureID for the specified path/dir
GLint TextureFromFile(const char *path, std::string directory)
{
    // Generate texture ID and load texture data
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    int width, height;
    
    unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    
    return textureID;
    
}
