#ifndef __MODEL_H__
#define __MODEL_H__
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>

#include "utils.h"
#include "texture.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "tiny_obj_loader.h"

#define POSITION_OFFSET BUFFER_OFFSET(0)
#define NORMAL_OFFSET   BUFFER_OFFSET(3*sizeof(float))
#define COLOR_OFFSET    BUFFER_OFFSET(6*sizeof(float))
#define ST_OFFSET       BUFFER_OFFSET(9*sizeof(float))

typedef struct
{
	float x, y, z;
	float nx, ny, nz;
	float r, g, b;
	float s, t;
} Vertex;


class ObjModel {
 public:
	ObjModel(std::string, std::string, bool);
	ObjModel(std::string, std::string, GLuint*, bool);
	ObjModel(std::string, std::string, std::string, GLuint*, bool);
	~ObjModel(void);
	void init(std::string, std::string, bool);
	void RenderTo(Camera, glm::vec3);
	void RenderMoreTo(Camera);
	void Rotate(float, glm::vec3);
	void RotatePosition(float, glm::vec3);
	void RotatePosition(float, glm::vec3, glm::vec3);
	void Scale(float);
	void Scale(glm::vec3);
	void ApplyModelTransform(void);
	void ApplyModelTransform2(void);
	void ApplyModelTranslate(void);
	glm::mat4 ComputeModelMatrix(void);
	glm::mat4 orientation(void);
	std::vector< std::string > orientationReport(void);
	void Translate(glm::vec3);
	std::vector< Vertex > LoadVertices(std::string);
	glm::mat4 mvp_to_camera(Camera);
	void pushModelMatrix(void);
	glm::mat4 peekModelMatrix(void);
	void bufferModelMatrix(void);
	void popModelMatrix(void);
	void setModelMatrix(glm::mat4);
	void pushTranslationMatrix(void);
	void popTranslationMatrix(void);
	void bindVAO(void);
	std::vector< GLuint > textureID;
	GLuint VAO[1];
	GLuint VBO[1];
	GLuint* shaderProgram;
	bool disableProjection;
	int activeVAO;
	int vertexCount;
	bool isPrototype;
 private:
	bool heightMap;
	std::vector< Vertex > vMatrix;
	glm::mat4 baseMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 translationMatrix;
	std::vector< glm::mat4 > modelMatrixStack;
	std::vector< glm::mat4 > translationMatrixStack;
	void PrepareVertexAttributes(void);
	void BufferVertices(void);
	void ReBufferVertices(void);
	tinyobj::attrib_t attrib;
	std::vector< tinyobj::shape_t > shapes;
	std::vector< tinyobj::material_t > materials;
	TextureLoader TexLoader;
};

#endif
