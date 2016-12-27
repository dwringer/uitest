#include "objmodel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "LoadShaders.h"

#define VERTEX_IDX(N) attrib.vertices[3*idx.vertex_index+(N)]
#define NORMAL_IDX(N) attrib.normals[3*idx.normal_index+(N)]
#define MAT_COLOR(N, I) materials[(N)].diffuse[(I)]
#define ST_COORDS(I) attrib.texcoords[2*idx.texcoord_index+(I)]

enum colorBases {R, G, B};
enum Attrib_IDs {vPosition, vNormal, vColor, vTexCoord};  // GLSL Layout Locs


ObjModel::ObjModel(std::string filename, std::string texture, bool prototype) {
	heightMap = 0;
	shaderProgram = NULL;
	init(filename, texture, prototype);
}


ObjModel::ObjModel(std::string filename, std::string texture,
		   GLuint* shader_program, bool prototype) {
	heightMap = 0;
	shaderProgram = shader_program;
	init(filename, texture, prototype);
}


ObjModel::ObjModel(std::string filename, std::string texture,
		   std::string heightmap, GLuint* shader_program,
		   bool prototype) {
	heightMap = 1;
	shaderProgram = shader_program;
	init(filename, texture, prototype);
	if (prototype)
		textureID.push_back(TexLoader.LoadTexture(heightmap.c_str()));
}


void ObjModel::init(std::string filename, std::string texture, bool prototype) {
	/* Initialize VAO/VBO and object texture */
	isPrototype = prototype;
	vMatrix = LoadVertices(filename);
	vertexCount = vMatrix.size();
	baseMatrix = glm::mat4();
	if (prototype) {
		glGenVertexArrays(1, VAO);
		glGenBuffers(1, VBO);
		activeVAO = 0;
		textureID.push_back(TexLoader.LoadTexture(texture.c_str()));
		BufferVertices();
	}
	disableProjection = 0;
}


glm::mat4 ObjModel::orientation(void) {
	glm::mat4 _m = ComputeModelMatrix();
	return _m *
		glm::mat4(glm::vec4(0., 0., 0., 1.),
			  glm::vec4(0., 0., 1., 1.),
			  glm::vec4(-1, 0., 0., 1.),
			  glm::vec4(0., 1., 0., 1.));
}


#define __LINEVEC(OSS, LBL, VEC)  \
	OSS << LBL << ffloat(VEC[0], 3) << ", "  \
	<< ffloat(VEC[1], 3) << ", "  \
	<< ffloat(VEC[2], 3)
std::vector< std::string > ObjModel::orientationReport(void) {
	glm::mat4 _o = orientation();
	glm::vec3 _pos(_o[0]);
	glm::vec3 _front(_o[1]);
	glm::vec3 _right(_o[2]);
	glm::vec3 _up(_o[3]);
	std::ostringstream _posS, _frontS, _rightS, _upS;
	__LINEVEC(_posS, " POS: ", _pos);
	__LINEVEC(_frontS, "LOOK: ", _front);
	__LINEVEC(_rightS, "RGHT: ", _right);
	__LINEVEC(_upS, "  UP: ", _up);
	std::vector< std::string > _acc;
	_acc.push_back(_posS.str());
	_acc.push_back(_frontS.str());
	_acc.push_back(_rightS.str());
	_acc.push_back(_upS.str());
	return _acc;
}
#undef __LINEVEC


void ObjModel::RenderTo(Camera camera, glm::vec3 light_pos_cameraspace) {
	/* Set up texture(s), VAO, and shader uniforms, then draw */
	glUseProgram(*shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	if (heightMap) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID[1]);
	}
	bindVAO();
	glm::mat4 m = ComputeModelMatrix();
	glm::mat4 mv = camera.viewMatrix * m;
	glm::mat4 timv = camera.viewMatrix * glm::transpose(glm::inverse(m));
	glm::mat4 mvp = camera.projectionMatrix * mv;
	glm::vec3 lightPosCS = light_pos_cameraspace;
	GLint uniformMVP = glGetUniformLocation(*shaderProgram, "MVP");
	GLint uniformMV = glGetUniformLocation(*shaderProgram, "MV");
	GLint uniformTiMV = glGetUniformLocation(*shaderProgram, "tiMV");
	GLint uniformLightPosCS =
		glGetUniformLocation(*shaderProgram, "LightPos_cameraspace");
	glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(uniformMV, 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(uniformTiMV, 1, GL_FALSE, glm::value_ptr(timv));
	glUniform3fv(uniformLightPosCS, 1,
		     glm::value_ptr(lightPosCS));
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}


void ObjModel::RenderMoreTo(Camera camera) {
	/* Render, assuming a prototype with vertices was already loaded */
	glm::mat4 m = ComputeModelMatrix();
	glm::mat4 mv = camera.viewMatrix * m;
	glm::mat4 timv = camera.viewMatrix * glm::transpose(glm::inverse(m));
	glm::mat4 mvp = camera.projectionMatrix * mv;
	GLint uniformMVP = glGetUniformLocation(*shaderProgram, "MVP");
	GLint uniformMV = glGetUniformLocation(*shaderProgram, "MV");
	GLint uniformTiMV = glGetUniformLocation(*shaderProgram, "tiMV");
	glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(uniformMV, 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(uniformTiMV, 1, GL_FALSE, glm::value_ptr(timv));
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}


glm::mat4 ObjModel::ComputeModelMatrix(void) {
	/* Combine three distinct model matrix components */
	return translationMatrix * modelMatrix * baseMatrix;
}


void ObjModel::PrepareVertexAttributes(void) {
	/* Route vertex attribute pointers to VAO offsets */
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE,
			      sizeof(Vertex), POSITION_OFFSET);
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE,
			      sizeof(Vertex), NORMAL_OFFSET);
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE,
			      sizeof(Vertex), COLOR_OFFSET);
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE,
			      sizeof(Vertex), ST_OFFSET);
}	


void ObjModel::BufferVertices(void) {
	/* Bind VAO and VBO, set up vertex attribute pointers, then buffer */
	glBindVertexArray(VAO[activeVAO]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[activeVAO]);
	PrepareVertexAttributes();
	glBufferData(GL_ARRAY_BUFFER,
		     vMatrix.size() * sizeof(Vertex),
		     &(vMatrix)[0],
		     GL_DYNAMIC_DRAW);
}


void ObjModel::ReBufferVertices(void) {
	/* Refill this object's VBO with Vertexes from vMatrix */
	glBindBuffer(GL_ARRAY_BUFFER, VBO[activeVAO]);
	PrepareVertexAttributes();
	glBufferSubData(GL_ARRAY_BUFFER, 0, vMatrix.size() * sizeof(Vertex),
			&(vMatrix)[0]);
}


void ObjModel::ApplyModelTransform(void) {
	baseMatrix = modelMatrix * baseMatrix;
	modelMatrix = glm::mat4();
}


void ObjModel::ApplyModelTransform2(void) {
	/* Apply modelMatrix to vMatrix, then clear modelMatrix and rebuffer */
        for (int i = 0; i < vMatrix.size(); i++) {
		Vertex _v = vMatrix[i];
		glm::vec4 _pos = modelMatrix * glm::vec4(_v.x, _v.y, _v.z, 1.);
		vMatrix[i].x = _pos.x / _pos.w;
		vMatrix[i].y = _pos.y / _pos.w;
		vMatrix[i].z = _pos.z / _pos.w;
	}
	modelMatrix = glm::mat4();	
	ReBufferVertices();
}


void ObjModel::ApplyModelTranslate(void) {
	baseMatrix = translationMatrix * baseMatrix;
	translationMatrix = glm::mat4();
}


ObjModel::~ObjModel(void) { };


void ObjModel::Rotate(float radians, glm::vec3 rotation_axis) {
	if (disableProjection)
		rotation_axis = glm::vec3(glm::rotate(glm::mat4(),
						      glm::radians(90.f),
						      glm::vec3(1., 0., 0.)) *
					  glm::vec4(rotation_axis, 1.));
	modelMatrix = glm::rotate(modelMatrix,
				  radians,
				  rotation_axis);
}


void ObjModel::RotatePosition(float radians, glm::vec3 rotation_axis) {
	/* Rotate position about the origin */
	translationMatrix = glm::rotate(translationMatrix,
					radians,
					rotation_axis);
}


void ObjModel::RotatePosition(float radians, glm::vec3 center,
			      glm::vec3 rotation_axis) {
	/* Rotate position about a given position */
	Translate(-center);
	RotatePosition(radians, rotation_axis);
	Translate(center);
}


void ObjModel::Scale(float scalar) {
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scalar));
}


void ObjModel::Scale(glm::vec3 axis_scales) {
	modelMatrix = glm::scale(modelMatrix, axis_scales);
}


void ObjModel::Translate(glm::vec3 translation_vec) {
	translationMatrix = glm::translate(translationMatrix, translation_vec);
}


void ObjModel::pushTranslationMatrix(void) {
	translationMatrixStack.push_back(translationMatrix);
	translationMatrix = glm::mat4();
}


void ObjModel::popTranslationMatrix(void) {
	translationMatrix = translationMatrixStack.back();
	translationMatrixStack.pop_back();
}


void ObjModel::pushModelMatrix(void) {
	modelMatrixStack.push_back(modelMatrix);
	modelMatrix = glm::mat4();
}


void ObjModel::popModelMatrix(void) {
	modelMatrix = modelMatrixStack.back();
	modelMatrixStack.pop_back();
}


void ObjModel::bufferModelMatrix(void) {
	modelMatrixStack.push_back(modelMatrix);
}


glm::mat4 ObjModel::peekModelMatrix(void) {
	return modelMatrixStack.back();
}


void ObjModel::setModelMatrix(glm::mat4 new_matrix) {
	modelMatrix = new_matrix;
}


std::vector< Vertex > ObjModel::LoadVertices(std::string filename) {
	vMatrix.clear();
	std::string err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
	 		      filename.c_str(), "obj/")) exit(1);
	for (size_t s=0; s < shapes.size(); s++) {
		int indexOffset = 0;
		for (size_t f=0;
		     f < shapes[s].mesh.num_face_vertices.size();
		     f++) {
			int materialID = shapes[s].mesh.material_ids[f];
			int fv = shapes[s].mesh.num_face_vertices[f];
			for (size_t v=0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[
						           indexOffset + v];
				float vx = VERTEX_IDX(0);
				float vy = VERTEX_IDX(1);
				float vz = VERTEX_IDX(2);
				float vnx = NORMAL_IDX(0);
				float vny = NORMAL_IDX(1);
				float vnz = NORMAL_IDX(2);
				float vr = MAT_COLOR(materialID, R);
				float vg = MAT_COLOR(materialID, G);
				float vb = MAT_COLOR(materialID, B);
				float tx = ST_COORDS(0);
				float ty = ST_COORDS(1);
				Vertex newVert = {vx, vy, vz,
						  vnx, vny, vnz,
						  vr, vg, vb,
						  tx, ty};
				vMatrix.push_back(newVert);
			}
			indexOffset += fv;
		}
	}
	return vMatrix;
}


void ObjModel::bindVAO(void) {
	glBindVertexArray(VAO[activeVAO]);
}


glm::mat4 ObjModel::mvp_to_camera(Camera camera) {
	/* Derive model matrix and project to camera */
	if (disableProjection) {
		return glm::ortho(-0.5f, 0.5f,
				  -(float)(1./camera.aspectRatio)/2,
				  (float)(1./camera.aspectRatio)/2) *
			translationMatrix *
			glm::rotate(modelMatrix,
				    glm::radians(-90.f),
				    glm::vec3(1.,0.,0)) *
			baseMatrix;
	} else {
		return glm::mat4(camera.projectionMatrix *
				 camera.viewMatrix *
				 translationMatrix *
				 modelMatrix *
				 baseMatrix);
	}
}
