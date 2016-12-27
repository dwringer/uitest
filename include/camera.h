#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <iostream>
#include <vector>
#include <sstream>
#include "vgl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <utils.h>

#define MAX_PITCH 89.99999

enum OrientationIndices { O_POSITION,
			  O_FRONT,
			  O_RIGHT,
			  O_UP
};


class Camera {
 public:
	Camera(void);
	Camera(glm::vec3, float, float, float);
	~Camera(void);
	glm::mat4 orientation;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	std::vector< std::string > orientationReport(void);
	void tilt(float);
	void pan(float);
	void raise(float);
	void aspect(GLfloat);
	void setFov(GLfloat);
	GLfloat getFov(void);
	float getDir(void);
	float aspectRatio;
	float cameraAngle;
	float cameraPitch;
 private:
	void init(glm::vec3, glm::vec3, glm::vec3, float);
	void _Orient(void);
	float hfov;
	float vfov;
	float zNear;
	float zFar;
};


#endif
