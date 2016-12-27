#include "camera.h"

Camera::Camera(void) { };

Camera::Camera(glm::vec3 position, float heading, float pitch, float fov) {
	/* Instantiate camera with orientation derived from heading */
	cameraAngle = (glm::radians(-1. * heading) -
		       glm::radians(90.f)) * -1.;
	cameraPitch = pitch;
	glm::vec3 cameraFacing = glm::vec3(glm::cos(cameraAngle),
					   glm::tan(glm::radians(pitch)),
					   glm::sin(cameraAngle));
	glm::vec3 up = glm::vec3(0., 1., 0.);
	glm::vec3 right = glm::cross(cameraFacing, up);
	up = glm::cross(right, cameraFacing);
	init(position, cameraFacing, up, fov);
}


std::vector< std::string > Camera::orientationReport(void) {
	/* String vector describing current orientation */
	std::vector< std::string > _strVec;
	std::ostringstream _posStr;
	_posStr << " POS: " << ffloat(orientation[O_POSITION][0], 3) << ", "
		<< ffloat(orientation[O_POSITION][1], 3) << ", "
		<< ffloat(orientation[O_POSITION][2], 3);
	std::ostringstream _lookAtStr;
	_lookAtStr << "LOOK: " << ffloat(orientation[O_FRONT][0], 3) << ", "
		   << ffloat(orientation[O_FRONT][1], 3) << ", "
		   << ffloat(orientation[O_FRONT][2], 3);
	std::ostringstream _rightStr;
	_rightStr << "RGHT: " << ffloat(orientation[O_RIGHT][0], 3) << ", "
		  << ffloat(orientation[O_RIGHT][1], 3) << ", "
		  << ffloat(orientation[O_RIGHT][2], 3);
	std::ostringstream _upStr;
	_upStr << "  UP: " << ffloat(orientation[O_UP][0], 3) << ", "
	       << ffloat(orientation[O_UP][1], 3) << ", "
	       << ffloat(orientation[O_UP][2], 3);
	_strVec.push_back(_posStr.str());
	_strVec.push_back(_lookAtStr.str());
	_strVec.push_back(_rightStr.str());
	_strVec.push_back(_upStr.str());
	return _strVec;
}


Camera::~Camera(void) {
}


void Camera::init(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up,
		  float fov) {
	/* Initialize camera orientation and matrices */
	orientation[O_POSITION] = glm::vec4(position, 1.);
	orientation[O_FRONT] = glm::vec4(glm::normalize(lookAt), 1.);
	orientation[O_UP] = glm::vec4(glm::normalize(up), 1.);
	orientation[O_RIGHT] = glm::vec4(glm::normalize(glm::cross(lookAt,
								   up)), 1.);
	aspectRatio = 1.;
	hfov = glm::radians(fov);
	vfov = 2 * glm::atan(glm::tan((hfov/2.)) * (1./aspectRatio));
	zNear = .01;
	zFar = 5000000.;
	projectionMatrix = glm::perspective(vfov, aspectRatio, zNear, zFar);
	viewMatrix = glm::lookAt(position, position + lookAt, up);
}


void Camera::setFov(GLfloat horizontal_fov) {
	/* Adjust camera horizontal field-of-view */
	hfov = horizontal_fov;
	aspect(aspectRatio);
};


float Camera::getDir(void) {
	/* Return compass bearing relative to surface, in radians */
	glm::vec3 _north(0., 0., 1.);
	glm::vec3 _lookat(orientation[O_FRONT]);
	_lookat[1] = 0.;
	float _thetaXZ = glm::acos(glm::dot(_north, glm::normalize(_lookat)));
	return (_lookat[0] < 0) ? _thetaXZ : (glm::radians(360.) - _thetaXZ);
}


GLfloat Camera::getFov(void) {
	/* Return camera horizontal field-of-view */
	return hfov;
};


void Camera::aspect(GLfloat aspect_ratio) {
	/* Set aspect ratio and recalculate projection matrix */
	aspectRatio = aspect_ratio;
	vfov = 2 * glm::atan(glm::tan((hfov/2.)) * (1./aspectRatio));
	projectionMatrix = glm::perspective(vfov, aspectRatio, zNear, zFar);
}


void Camera::tilt(float tilt_amount) {
	/* Adjust pitch degrees up or down */
	float _newPitch = cameraPitch + tilt_amount;
	if (_newPitch > MAX_PITCH) {
		cameraPitch = MAX_PITCH;
	} else if (_newPitch < -MAX_PITCH) {
		cameraPitch = -MAX_PITCH;
	} else {
		cameraPitch = _newPitch;
	}
	_Orient();
}


void Camera::pan(float turn_amount) {
	/* Adjust cameraAngle radians by specified degrees */
	cameraAngle += glm::radians(turn_amount);
	_Orient();
}


void Camera::raise(float lift_amount) {
	/* Raise or lower the camera */
	glm::vec3 _pos = orientation[O_POSITION];
	_pos[1] += lift_amount;
	orientation[O_POSITION] = glm::vec4(_pos, 1.);
	_Orient();
}


void Camera::_Orient(void) {
	/* Recalculate orientation from angles and set view matrix */
	glm::vec3 cameraFacing = glm::vec3(glm::cos(cameraAngle),
					   glm::tan(glm::radians(cameraPitch)),
					   glm::sin(cameraAngle));
	glm::vec3 pos = glm::vec3(orientation[O_POSITION]);
	glm::vec3 up = glm::vec3(0., 1., 0.);
	glm::vec3 right = glm::cross(cameraFacing, up);
	up = glm::cross(right, cameraFacing);
	viewMatrix = glm::lookAt(pos, pos + cameraFacing, up);
	orientation[O_FRONT] = glm::vec4(glm::normalize(cameraFacing), 1.);
	orientation[O_UP] = glm::vec4(glm::normalize(up), 1.);
	orientation[O_RIGHT] = glm::vec4(glm::normalize(right), 1.);
}
