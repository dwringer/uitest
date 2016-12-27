#include <main.h>

/// Directly import PRNG code - must include ONLY ONCE:
#include <ParkMiller.c>
// This PRNG implementation is thoroughly untested,
//  but simple to use: seed_random() with an int
//  then call random() to get a value in [0.f..1.f)


/// Prepare a global pointer for the program:
RenderingProgram* MainProgram;  


/// GLFW callback functions (to be routed into class functions):
void ResizeFunction(GLFWwindow*, int, int);
void KeysFunction(GLFWwindow*, int, int, int, int);
void ScrollFunction(GLFWwindow*, double, double);
void CursorPosFunction(GLFWwindow*, double, double);


RenderingProgram::RenderingProgram(void) { };


RenderingProgram::RenderingProgram(int argc, char **argv) {
	/* Initialize the program */
	/// Start time reference:
	initFrame = clock();
	lastFrame = initFrame;
	lastDisplayFrame = initFrame;
	measuredFPS = CLOCKS_PER_SEC;
	lastFPSUpdateFrame = initFrame;
	lastCameraUpdateFrame = initFrame;
	lastTargetUpdateFrame = initFrame;
	paused = BOOL_FALSE_FLAG;

	/// Physics engine prep:
	accumulatedTime = 0.;
	speedMod = SPEED_MOD;
	maxDt = MAX_DT;
	
	/// Set up music:
	songs.push_back("sound/rec2.wav");
	songs.push_back("sound/rec1.wav");
	songs.push_back("sound/Q3.wav");
	songs.push_back("sound/Q2.wav");
	songs.push_back("sound/Q1.wav");
	songIndex = 0;

	/// Seed random generator:
	random_seed(clock());

	/// Create world cameras:
	Camera wCamera = Camera(glm::vec3(0., .8, -1.), 60.f, 20.f, 98.f);
	Camera hCamera = Camera(glm::vec3(0., 1.2, 0.), -70.f, 0.f, 110.f);
	Camera mCamera = Camera(glm::vec3(0., 2., 0.), 0., -MAX_PITCH, 115.f);
	Camera cCamera = Camera(glm::vec3(0., 1.2, 0.), -36.f, -5.f, 95.f);
	cameras.push_back(wCamera);
	cameras.push_back(hCamera);
	cameras.push_back(mCamera);
	cameras.push_back(cCamera);
	cameraIndex = cameras.size() - 1;
	nextCameraChange = CAMERA_CHANGE_MIN_SECONDS +
		random() * (CAMERA_CHANGE_MAX_SECONDS -
			    CAMERA_CHANGE_MIN_SECONDS);
	lastCameraChange = initFrame;
	cameraSwitching = BOOL_TRUE_FLAG;
	
	/// Initialize GLFW3:
	glfwInit();
	// Set OpenGL to 4.3 core profile:
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Create the window:
	Width = INIT_WIDTH;
	Height = INIT_HEIGHT;
	monitor = glfwGetPrimaryMonitor();
	vidmode = glfwGetVideoMode(monitor);
	isFullscreen = START_FULLSCREEN;
	if (isFullscreen) {
		Width = vidmode->width;
		Height = vidmode->height;
		savedWindowDimensions = glm::vec4((Width - INIT_WIDTH)/2,
						  (Height - INIT_HEIGHT)/2,
						  INIT_WIDTH, INIT_HEIGHT);
	}
	for (int i = 0; i < cameras.size(); i++)
		cameras[i].aspect((float)Width/Height);
	window = glfwCreateWindow(Width, Height, WINDOW_TITLE,
				  ((isFullscreen) ? monitor : NULL),
				  NULL);
	glfwMakeContextCurrent(window);
	// Hide mouse cursor:
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// Register callback functions:
	glfwSetFramebufferSizeCallback(window, ResizeFunction);
	glfwSetKeyCallback(window, KeysFunction);
	glfwSetScrollCallback(window, ScrollFunction);
	glfwSetCursorPosCallback(window, CursorPosFunction);
	
	/// Initialize GLEW:
	glewExperimental = TRUE;  // Required for 4.3 context on Windows
	glewInit();

	/// Initialize OpenGL shaders and default buffers:
	init();

	/// Prepare UI:
	// Initialize freetype:
	if (FT_Init_FreeType(&ft)) exit(1);
	// Set up font for text rendering:
	fontRenderer = Font("fonts/ArtesianBoldOblique.ttf",
			    (int)32 * FONT_UPSAMPLE, ft);
	regularFont = Font("fonts/ArtesianRegular.ttf",
			   (int)22 * FONT_UPSAMPLE, ft);
	// Initialize print consoles:
	interactionMode = IMODE_CONTROL;
	mainConsole = Console(&regularFont, &fontRenderer, 30, 13000, 5000,
			      170, 23, 15, CONSOLE_NONE,
			      BASE_CONSOLE_WIDTH, BASE_CONSOLE_HEIGHT,
			      BOOL_FALSE_FLAG);
	fpsConsole = Console(&regularFont, &fontRenderer, 30, 15000, 3500,
			     50, 107, 15, 1,
			     BASE_CONSOLE_WIDTH, BASE_CONSOLE_HEIGHT,
			     BOOL_TRUE_FLAG);
	centerConsole = Console(&fontRenderer, &regularFont, 30, 15000, 3500,
				Height / 2. + 15., Width / 2. - 50., 15, 2,
				BASE_CONSOLE_WIDTH, BASE_CONSOLE_HEIGHT,
				BOOL_FALSE_FLAG);
	cameraConsole = Console(&regularFont, &fontRenderer, 30, 15000, 3500,
				Height, 400, -20, 5,
				BASE_CONSOLE_WIDTH, BASE_CONSOLE_HEIGHT,
				BOOL_TRUE_FLAG);
	targetConsole = Console(&regularFont, &fontRenderer, 30, 15000, 3500,
				Height, 23, -20, 5,
				BASE_CONSOLE_WIDTH, BASE_CONSOLE_HEIGHT,
				BOOL_FALSE_FLAG);
	Console* _consolePointers[] = {&mainConsole, &fpsConsole,
				       &centerConsole, &cameraConsole,
				       &targetConsole};
	int _llen = sizeof(_consolePointers) / sizeof(_consolePointers[0]);
	for (int i = 0; i < _llen; i++)
		consoles.push_back(_consolePointers[i]);
	for (int i = 0; i < consoles.size(); i++)
		consoles[i]->resizeWindow(Width, Height);
	console_print("Console initialized", &mainConsole);
	console_print("### FPS", &fpsConsole);
	console_print("Program", &centerConsole);
	console_print("  Initialized", &centerConsole);
	console_print("Camera console initialized", &cameraConsole);
	console_print("Target console initialized", &targetConsole);
	// Initialize input values:
	translationVelocityNWSE = glm::vec4(0., 0., 0., 0.);
	translationVelocityFLBR = glm::vec4(0., 0., 0., 0.);
	fovAdjustment = glm::vec2(0., 0.);
	cursorVelocityULDR = glm::vec4(0., 0., 0., 0.);
	cameraVelocityUADC = glm::vec4(0., 0., 0., 0.);
	glfwSetCursorPos(window, Width / 2., Height / 2.);
	
	/// Instantiate geometry:
	drawShapes();

	/// Set world Latitude and Longitude:
	changeCoordinates(Latitude, Longitude);
	showGlobe = BOOL_TRUE_FLAG;
	
	/// Close freetype:
	FT_Done_FreeType(ft);
}


RenderingProgram::~RenderingProgram(void) {
	/* I'm sure I should think of  something  to put here */
}


void KeysFunction(GLFWwindow* window,
		  int key, int scancode, int action, int mods) {
	/* Patch GLFW keystroke callback to class function */
	MainProgram->keys(key, scancode, action, mods);
}


void ResizeFunction(GLFWwindow* window, int width, int height) {
	/* Patch GLFW resize callback to class function */
	MainProgram->resize(width, height);
}


void ScrollFunction(GLFWwindow* window, double xoffset, double yoffset) {
	/* Patch GLFW scrollwheel callback to class function */
	MainProgram->scroll(xoffset, yoffset);
}


void CursorPosFunction(GLFWwindow* window, double xpos, double ypos) {
	/* NOT IMPLEMENTED! - GLFW callback function for mouse cursor */
	MainProgram->cursorPos(xpos, ypos);
}


void RenderingProgram::resize(int width, int height) {
	/* GLFW resize callback handler */
	if ((width == 0) && (height == 0)) {
		pause();
		return;
	}
	Width = width;
	Height = height;
	glViewport(0, 0, Width, Height);
	for (int i = 0; i < cameras.size(); i++)
		cameras[i].aspect((float)width/height);	
	for (int i = 0; i < consoles.size(); i++)
		consoles[i]->resizeWindow(width, height);
	glfwSetCursorPos(window, Width / 2., Height / 2.);
}


void RenderingProgram::scroll(double xoffset, double yoffset) {
	/* GLFW scrollwheel callback handler */
	ostringstream _fovs;
	adjustCameraFov(-glm::radians(3. * yoffset));
	_fovs << "FOV " << (int)(glm::degrees(cameras[cameraIndex].getFov()))
	      << "º";
	console_print(_fovs.str());
}


void RenderingProgram::cursorPos(double xpos, double ypos) {
	/* NOT IMPLEMENTED! - GLFW mouse cursor callback handler */
}


void RenderingProgram::toggleFullscreen(void) {
	/* Toggle fullscreen and manage recovery of window state */
	if (isFullscreen) {
		int _xOffset, _yOffset;
		_xOffset = savedWindowDimensions[0];
		_yOffset = savedWindowDimensions[1];
		Width = savedWindowDimensions[2];
		Height = savedWindowDimensions[3];
		glfwSetWindowMonitor(window, NULL,
				     _xOffset, _yOffset, Width, Height,
				     GLFW_DONT_CARE);
		console_print("Fullscreen disabled");
	} else {
		int _xpos, _ypos;
		glfwGetWindowPos(window, &_xpos, &_ypos);
		savedWindowDimensions = glm::vec4(_xpos, _ypos, Width, Height);
		Width = vidmode->width;
		Height = vidmode->height;
		glfwSetWindowMonitor(window, monitor, 0, 0, Width, Height,
				     vidmode->refreshRate);
		console_print("Fullscreen enabled");
	};
	mainConsole.resizeWindow(Width, Height);
	fpsConsole.resizeWindow(Width, Height);
	isFullscreen = (isFullscreen) ? BOOL_FALSE_FLAG : BOOL_TRUE_FLAG;
}


void RenderingProgram::speedUp(void) {
	/* Double simulation time per physics frame */
	ostringstream speedString;
	if (speedMod < MAX_SIMULATION_SPEED) {
		speedMod *= 2.0;
		maxDt *= 2.0;
		speedString << "Speed increased: ";
	} else {
		speedString << "Speed limited: ";
	}
	speedString << speedMod << "x";
	console_print(speedString.str());
}


void RenderingProgram::speedDown(void) {
	/* Halve simulation time per physics frame */
	ostringstream speedString;
	if (speedMod > MIN_SIMULATION_SPEED) {
		speedMod /= 2.0;
		maxDt /= 2.0;
		speedString << "Speed decreased: ";
	} else {
		speedString << "Speed limited: ";
	}
	speedString << speedMod << "x";
	console_print(speedString.str());
}


void RenderingProgram::translateWorldNS(float delta_theta) {
	/* Move north/south */
	Latitude += delta_theta;
	if (Latitude > 360.) {
		Latitude -= 360.;
	} else if (Latitude < -360.) {
		Latitude += 360.;
	}
	changeCoordinates(Latitude, Longitude);
}


void RenderingProgram::translateWorldEW(float delta_theta) {
	/* Move east/west */
	Longitude -= delta_theta;
	if (Longitude > 180.) {
		Longitude = -180. + (Longitude - 180.);
	} else if (Longitude < -180.) {
		Longitude = 180. + (Longitude + 180.);
	}
	changeCoordinates(Latitude, Longitude);
}


void RenderingProgram::translateWorldFB(float delta_theta) {
	/* Move forward/backward */
	float _camDir = cameras[cameraIndex].getDir();
	float _ewSign = 1.;
	if ((abs(Latitude) > 90.) && (abs(Latitude) < 270.)) {
		_ewSign = -1.;
	}
	translateWorldNS(glm::cos(_camDir) * delta_theta);
	translateWorldEW(_ewSign * -glm::sin(_camDir) * delta_theta);
}


void RenderingProgram::translateWorldLR(float delta_theta) {
	/* Move left/right */
	float _camDir = cameras[cameraIndex].getDir();
	float _ewSign = 1.;
	if ((abs(Latitude) > 90.) && (abs(Latitude) < 270.))
		_ewSign = -1.;
	translateWorldNS(glm::sin(_camDir) * delta_theta);
	translateWorldEW(_ewSign * glm::cos(_camDir) * delta_theta);
}


void RenderingProgram::adjustCameraFov(float delta_theta) {
	/* Adjust FOV of active camera by given angle */
	float _fov = cameras[cameraIndex].getFov();
	float _check = _fov + delta_theta;
	if (_check < glm::radians(FOV_MINIMUM)) {
		console_print("FOV at minimum");
	} else if (_check > glm::radians(FOV_MAXIMUM)) {
		console_print("FOV at maximum");
	} else {
		_fov += delta_theta;
		cameras[cameraIndex].setFov(_fov);
	}
}


glm::vec2 RenderingProgram::getCoordinates(void) {
	/* Return properly-constrained latitude and longitude */
	float _lat = Latitude;
	float _long = Longitude;
	while (_lat > 90) {
		_lat -= 180.;
		_lat *= -1;
		_long += 180.;
	}
	while (_lat < -90) {
		_lat += 180.;
		_lat *= -1.;
		_long += 180.;
	}
	while (_long > 180) _long -= 360.;
	while (_long < -180) _long += 360.;
	return glm::vec2(_lat, _long);
}


void RenderingProgram::printCoordinates(void) {
	/* Get constrained latitude/longitude and print formatted */
	ostringstream  _latLongStr;
	glm::vec2 _latLong = getCoordinates();
	float _lat = _latLong[0];
	float _long = _latLong[1];
	std::string _ns, _ew;
	if (_lat < 0) {
		_lat *= -1;
		_ns = "ºS";
	} else {
		_ns = "ºN";
	}
	if (_long < 0) {
		_long *= -1;
		_ew = "ºW";
	} else {
		_ew = "ºE";
	}
	_latLongStr << _lat << _ns << "," << _long << _ew;
	console_print(_latLongStr.str());
}


void RenderingProgram::pause(void) {
	/* Stop updating */
	console_print("Simulation paused");
	paused = BOOL_TRUE_FLAG;
}


void RenderingProgram::unpause(void) {
	/* Resume updating */
	paused = BOOL_FALSE_FLAG;
	console_print("Simulation unpaused");
}


#define __VECTOR_DRIVER(I, T, DT, V,  \
			F_0, SIGN_0, F_1, SIGN_1, F_2, SIGN_2, F_3, SIGN_3)  \
	T = (V)[I];  \
	T *= (DT) / MAX_DT;  \
	if ((T) > 0.) {  \
		switch(I) {  \
		case 0:  \
			(F_0)((SIGN_0)*(T));	\
			break;  \
		case 1:  \
			(F_1)((SIGN_1)*(T));	\
			break;  \
		case 2:  \
			(F_2)((SIGN_2)*(T));	\
			break;  \
		case 3:  \
			(F_3)((SIGN_3)*(T));	\
			break;  \
		}  \
	}


void RenderingProgram::update(void) {
	/* Update every cycle of the master loop - allocate physics frames */
	clock_t frame = clock();
	dt = (float)(frame - lastFrame) / CLOCKS_PER_SEC;
	if (dt > 0) measuredFPS = (1./dt + measuredFPS) / 2.;
	float _scaledDt = dt * speedMod;

	/// Update music:
	sound.update();
	if (!sound.isPlaying()) {
		sound.playWav(songs[songIndex].c_str());
		songIndex = (songIndex + 1) % songs.size();
	}

	/// Update FPS counter:
	ostringstream lineStream;
	if (((frame - lastFPSUpdateFrame) >
	     ((1./FPS_UPDATE_HZ) * CLOCKS_PER_SEC)) &&
	    fpsConsole.status()) {
		lineStream << (int)measuredFPS << " FPS";
		console_print(lineStream.str(), &fpsConsole);
		lastFPSUpdateFrame = frame;
	}

	/// Update camera readout:
	if (((frame - lastCameraUpdateFrame) >
	     ((1./CAMERA_UPDATE_HZ) * CLOCKS_PER_SEC)) &&
	    cameraConsole.status()) {
		std::vector< std::string > _orientation =
			cameras[cameraIndex].orientationReport();
		for (int i = 0; i < _orientation.size(); i++) {
			console_print(_orientation[i],
				      &cameraConsole);
		}
		std::ostringstream _dirStr;
		_dirStr << " HDG:" <<
			ffloat(glm::degrees(cameras[cameraIndex].getDir()),
			       1) << "º";
		glm::vec2 _coords = getCoordinates();
		_dirStr << " @" << ffloat(_coords[0], 2);
		_dirStr << "º," << ffloat(_coords[1], 2) << "º";
		console_print(_dirStr.str(), &cameraConsole);
		lastCameraUpdateFrame = frame;
	}

	/// Update target readout:
	if (((frame - lastTargetUpdateFrame) >
	     ((1./TARGET_UPDATE_HZ) * CLOCKS_PER_SEC)) &&
	    targetConsole.status()) {
		std::vector< std::string > _o =
			universeMeshes[universeMeshes.size() -
				       2].orientationReport();
		console_print("Target: Earth", &targetConsole);
		for (int i = 0; i < _o.size(); i++)
			console_print(_o[i], &targetConsole);
		lastTargetUpdateFrame = frame;
	}
	
	/// Update physics engine:
	accumulatedTime += _scaledDt;
	// Catch up on physics frames:
	while (accumulatedTime > maxDt) {
		pStep(maxDt);
		accumulatedTime -= maxDt;
	}
	// Burn time remaining (instead of interpolating, etc):
	pStep(accumulatedTime);
	accumulatedTime = 0.;

	/// Flip through the cameras every few seconds:
	clock_t _test = frame - lastCameraChange;
	if (cameraSwitching && (_test > nextCameraChange)) {
		ostringstream _camDir;
		cameraIndex = (cameraIndex + 1) % cameras.size();
		nextCameraChange = (CAMERA_CHANGE_MIN_SECONDS +
				    random() *
				    (CAMERA_CHANGE_MAX_SECONDS -
				     CAMERA_CHANGE_MIN_SECONDS)) *
			CLOCKS_PER_SEC;
		lastCameraChange = frame;
	}

	/// Manage world latitude/longitude translation in realtime:
	for (int i=0; i < 4; i++) {
		float _t;
		__VECTOR_DRIVER(i, _t, dt, translationVelocityNWSE,
				translateWorldNS, 1,
				translateWorldEW, 1,
				translateWorldNS, -1,
				translateWorldEW, -1);
		__VECTOR_DRIVER(i, _t, dt, translationVelocityFLBR,
				translateWorldFB, 1,
				translateWorldLR, 1,
				translateWorldFB, -1,
				translateWorldLR, -1);
		__VECTOR_DRIVER(i, _t, dt, cursorVelocityULDR,
				cameras[cameraIndex].tilt, 1,
				cameras[cameraIndex].pan, -1,
				cameras[cameraIndex].tilt, -1,
				cameras[cameraIndex].pan, 1);
		__VECTOR_DRIVER(i, _t, dt, cameraVelocityUADC,
				cameras[cameraIndex].raise, 1,
				cameras[cameraIndex].raise, 1,
				cameras[cameraIndex].raise, -1,
				cameras[cameraIndex].raise, -1);
	}
	/// Increment last frame clock time:
	lastFrame = frame;
}


void RenderingProgram::pStep(float dt) {
	/* Physics step [or, run a bunch of crazy transformations] */
	for(int i=0; i < meshes.size(); i++) {
		/* Ordinary models */
		//  ...
	}
	for(int i=0; i < universeMeshes.size(); i++) {
		if (i == universeMeshes.size() - 3) {
			/* SUN */
			universeMeshes[i].pushModelMatrix();
			universeMeshes[i].Rotate(DAYS_PER_SECOND *
					 glm::radians(360.f) * dt,
					 glm::vec3(0., -1., 0.));
			universeMeshes[i].ApplyModelTransform();
			universeMeshes[i].popModelMatrix();
		} else if (i == universeMeshes.size() - 1) {
			/* Skybox */
			universeMeshes[i].pushModelMatrix();
			universeMeshes[i].Rotate(SIDEREAL_DAYS_PER_SECOND *
					 glm::radians(360.f) * dt,
					 glm::vec3(0., -1.,0.));
			universeMeshes[i].ApplyModelTransform();
			universeMeshes[i].popModelMatrix();
		}
	}
}


void RenderingProgram::display(void) {
	/* Render a display frame */
	/// Prepare renderer for new 3D frame:
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/// Compute location of the sun:
	glm::vec3 sunPositionCS =
		cameras[cameraIndex].viewMatrix *
		universeMeshes[
			       universeMeshes.size() -
			       3].ComputeModelMatrix() *
		glm::vec4(0., 0., 0., 1.);
	
	/// Render the world objects:
	int _totalMeshes = meshes.size() + universeMeshes.size();
	for(int i = 0; i < _totalMeshes; i++) {
		if (!showGlobe && i == (_totalMeshes - 2))
			continue;
		ObjModel _m = (i >= meshes.size()) ?
			universeMeshes[i - meshes.size()] : meshes[i];
		if (_m.shaderProgram == NULL) {
			glUseProgram(programs[GLSLMainProgram]);
			uniformMVP = glGetUniformLocation(
				             programs[GLSLMainProgram], "MVP");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _m.textureID[0]);
			_m.bindVAO();
			glm::mat4 mvp = _m.mvp_to_camera(cameras[cameraIndex]);
			glUniformMatrix4fv(uniformMVP, 1, GL_FALSE,
					   glm::value_ptr(mvp));		
			glDrawArrays(GL_TRIANGLES, 0, _m.vertexCount);
		} else {
			if (_m.isPrototype) {
				_m.RenderTo(cameras[cameraIndex],
					    sunPositionCS);
			} else {
				_m.RenderMoreTo(cameras[cameraIndex]);
			}
		}
	}

	/// Render the console(s):
	glDisable(GL_DEPTH_TEST);
	clock_t _frame = clock();
	bool _textInputMode = interactionMode == IMODE_TEXT;
	for (int i = 0; i < consoles.size(); i++)
		consoles[i]->draw(programs[GLSLTextProgram],
				  (int)_frame, Width, Height,
				  _textInputMode);
	
        /// Manage FOV adjustment:
	for (int i=0; i < 2; i++) {
		float _t = fovAdjustment[i];
		switch (i) {
		case 0:
			adjustCameraFov(_t);
			break;
		case 1:
			adjustCameraFov(-_t);
			break;
		}
	}

	/// Update cursor pos
	float _dt = (float)(_frame - lastDisplayFrame) / CLOCKS_PER_SEC;
	double _mx, _my;
	glfwGetCursorPos(window, &_mx, &_my);
	glfwSetCursorPos(window, Width / 2., Height / 2.);
	double _dx = _mx - Width / 2.;
	double _dy = _my - Height / 2.;
	if (_dy != 0.) cameras[cameraIndex].tilt(-_dy *
						 MOUSE_Y_SENSITIVITY *
						 (_dt / MAX_DT));
	if (_dx != 0.) cameras[cameraIndex].pan(_dx *
						MOUSE_X_SENSITIVITY *
						(_dt / MAX_DT));

	/// Cleanup for next frame:
	lastDisplayFrame = _frame;
	glFlush();
}


void RenderingProgram::init(void) {
	/* Initialize shaders and texturing mode */
	ShaderInfo shaders[] = {{GL_VERTEX_SHADER, "shader/main.vert"},
				{GL_FRAGMENT_SHADER, "shader/main.frag"},
				{GL_NONE, NULL}};
	programs.push_back(Loader.LoadShaders(shaders));
	ShaderInfo textshaders[] = {{GL_VERTEX_SHADER, "shader/text.vert"},
				    {GL_FRAGMENT_SHADER, "shader/text.frag"},
				    {GL_NONE, NULL}};
	programs.push_back(Loader.LoadShaders(textshaders));
	ShaderInfo hmshaders[] = {{GL_VERTEX_SHADER, "shader/heightmap.vert"},
				  {GL_FRAGMENT_SHADER, "shader/heightmap.frag"},
				  {GL_NONE, NULL}};
	programs.push_back(Loader.LoadShaders(hmshaders));
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
}


void RenderingProgram::drawSky() {
	/* Add a skybox mesh and planet earth */
	Latitude = LATITUDE;
	Longitude = LONGITUDE;
	ObjModel Sun("obj/globe.obj", "img/sun.png",
		     &programs[GLSLMainProgram], BOOL_TRUE_FLAG);
	Sun.Scale(1.);
	Sun.ApplyModelTransform();
	Sun.Translate(glm::vec3(0., 0., -50.));
	Sun.ApplyModelTranslate();
	ObjModel Earth("obj/earth.obj", "img/truecolorGP.png",
		       "img/topographyGPSC.png", &programs[GLSLHMapProgram],
		       BOOL_TRUE_FLAG);
	ObjModel SkyObject("obj/globe.obj", "img/test.png",
			   &programs[GLSLMainProgram], BOOL_TRUE_FLAG);
	Earth.Scale(1.1);
	SkyObject.Scale(glm::vec3(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE));
	// Align prime meridian to 0 longitude
	Earth.Rotate(glm::radians(90.f), glm::vec3(0., 1., 0.));
	// Here NSEWUD = z+ z- x+ x- y+ y-
	// Rotate about y axis 90 to align gal. center to north)
	SkyObject.Rotate(glm::radians(90.f), glm::vec3(0., 1., 0.));
	// Rough galactic to celestial transformation; hacked together
	//  from http://ned.ipac.caltech.edu/level5/graph_relations.html
	//  and particularly from wgalchart.gif found at:
	// https://andrewrhodesphoto.wordpress.com/tag/guide-to-the-milky-way/
	// -> https://i1.wp.com/www.atlasoftheuniverse.com/wgalchart.gif
	SkyObject.Rotate(glm::radians(236.f), glm::vec3(0., -1., 0.));
	SkyObject.ApplyModelTransform();
	SkyObject.Rotate(glm::radians(64.f), glm::vec3(1., 0., 0.));
	Earth.ApplyModelTransform();
	SkyObject.ApplyModelTransform();
	// Save a copy of the model matrices before setting latitude/longitude:
	Sun.bufferModelMatrix();
	Earth.bufferModelMatrix();
	SkyObject.bufferModelMatrix();
	universeMeshes.push_back(Sun);
	universeMeshes.push_back(Earth);
	universeMeshes.push_back(SkyObject);
}


void RenderingProgram::changeCoordinates(float latitude, float longitude) {
	/* Rotate the universe to center the specified latitude/longitude */
	for (int i = 0; i < universeMeshes.size(); i++) {
		ObjModel* m = &universeMeshes[i];
		m->popModelMatrix();
		m->bufferModelMatrix();
		m->Rotate(glm::radians(longitude), glm::vec3(0., -1., 0.));
		m->Rotate(glm::radians(180 - (90.f + latitude)),
			  glm::vec3(glm::cos(glm::radians(longitude)), 0.,
				    -glm::sin(glm::radians(longitude))));
	}
}


void RenderingProgram::sendToUniverse(int mesh_index,
				      float latitude, float longitude) {
	/* Transfer meshes[mesh_index] to universeMeshes and set lat/long */
	ObjModel* m = &meshes[mesh_index];
	m->Rotate(glm::radians(180.), glm::vec3(0., -1., 0.));
	m->Rotate(glm::radians(180 - (90.f + latitude)),
		    glm::vec3(glm::cos(glm::radians(longitude)), 0.,
			      -glm::sin(glm::radians(longitude))));
	m->ApplyModelTransform();
	m->bufferModelMatrix();
	universeMeshes.insert(universeMeshes.begin(), *m);
	meshes.erase(meshes.begin() + mesh_index);
}

#define __LEN(ARR)  (sizeof(ARR) / sizeof((ARR)[0]))
void RenderingProgram::drawShapes(void) {
	/* Add object meshes */
	static const double latlongPairs[] = {
		34.28, 69.11,
		41.18, 19.49,
		36.42, 03.08,
		-14.16, -170.43,
		42.31, 01.32,
		-08.50, 13.15,
		17.20, -61.48,
		-36.30, -60.00,
		40.10, 44.31,
		12.32, -70.02,
		-35.15, 149.08,
		48.12, 16.22,
		40.29, 49.56,
		25.05, -77.20,
		26.10, 50.30,
		23.43, 90.26,
		13.05, -59.30,
		53.52, 27.30,
		50.51, 04.21,
		17.18, -88.30,
		06.23, 02.42,
		27.31, 89.45,
		-16.20, -68.10,
		43.52, 18.26,
		-24.45, 25.57,
		-15.47, -47.55,
		18.27, -64.37,
		04.52, 115.00,
		42.45, 23.20,
		12.15, -01.30,
		-03.16, 29.18,
		11.33, 104.55,
		03.50, 11.35,
		45.27, -75.42,
		15.02, -23.34,
		19.20, -81.24,
		04.23, 18.35,
		12.10, 14.59,
		-33.24, -70.40,
		39.55, 116.20,
		04.34, -74.00,
		-11.40, 43.16,
		-04.09, 15.12,
		09.55, -84.02,
		06.49, -05.17,
		45.50, 15.58,
		23.08, -82.22,
		35.10, 33.25,
		50.05, 14.22,
		-04.20, 15.15,
		55.41, 12.34,
		11.08, 42.20,
		15.20, -61.24,
		18.30, -69.59,
		-08.29, 125.34,
		-00.15, -78.35,
		30.01, 31.14,
		13.40, -89.10,
		03.45, 08.50,
		15.19, 38.55,
		59.22, 24.48,
		09.02, 38.42,
		-51.40, -59.51,
		62.05, -06.56,
		-18.06, 178.30,
		60.15, 25.03,
		48.50, 02.20,
		05.05, -52.18,
		-17.32, -149.34,
		00.25, 09.26,
		13.28, -16.40,
		41.43, 44.50,
		52.30, 13.25,
		05.35, -00.06,
		37.58, 23.46,
		64.10, -51.35,
		16.00, -61.44,
		14.40, -90.22,
		49.26, -02.33,
		09.29, -13.49,
		11.45, -15.45,
		06.50, -58.12,
		18.40, -72.20,
		-53.00, 74.00,
		14.05, -87.14,
		47.29, 19.05,
		64.10, -21.57,
		28.37, 77.13,
		-06.09, 106.49,
		35.44, 51.30,
		33.20, 44.30,
		53.21, -06.15,
		31.71, -35.10,
		41.54, 12.29,
		18.00, -76.50,
		31.57, 35.52,
		51.10, 71.30,
		-01.17, 36.48,
		01.30, 173.00,
		29.30, 48.00,
		42.54, 74.46,
		17.58, 102.36,
		56.53, 24.08,
		33.53, 35.31,
		-29.18, 27.30,
		06.18, -10.47,
		32.49, 13.07,
		47.08, 09.31,
		54.38, 25.19,
		49.37, 06.09,
		22.12, 113.33,
		-18.55, 47.31,
		42.01, 21.26,
		-14.00, 33.48,
		03.09, 101.41,
		04.00, 73.28,
		12.34, -07.55,
		35.54, 14.31,
		14.36, -61.02,
		-20.10, 57.30,
		-12.48, 45.14,
		19.20, -99.10,
		06.55, 158.09,
		47.02, 28.50,
		-25.58, 32.32,
		16.45, 96.20,
		-22.35, 17.04,
		27.45, 85.20,
		52.23, 04.54,
		12.05, -69.00,
		-22.17, 166.30,
		-41.19, 174.46,
		12.06, -86.20,
		13.27, 02.06,
		09.05, 07.32,
		-45.20, 168.43,
		39.09, 125.30,
		15.12, 145.45,
		59.55, 10.45,
		23.37, 58.36,
		33.40, 73.10,
		07.20, 134.28,
		09.00, -79.25,
		-09.24, 147.08,
		-25.10, -57.30,
		-12.00, -77.00,
		14.40, 121.03,
		52.13, 21.00,
		38.42, -09.10,
		18.28, -66.07,
		25.15, 51.35,
		37.31, 126.58,
		44.27, 26.10,
		55.45, 37.35,
		-01.59, 30.04,
		17.17, -62.43,
		14.02, -60.58,
		46.46, -56.12,
		13.10, -61.10,
		-13.50, -171.50,
		43.55, 12.30,
		00.10, 06.39,
		24.41, 46.42,
		14.34, -17.29,
		08.30, -13.17,
		48.10, 17.07,
		46.04, 14.33,
		-09.27, 159.57,
		02.02, 45.25,
		-25.44, 28.12,
		40.25, -03.45,
		15.31, 32.35,
		05.50, -55.10,
		-26.18, 31.06,
		59.20, 18.03,
		46.57, 07.28,
		33.30, 36.18,
		38.33, 68.48,
		13.45, 100.35,
		06.09, 01.20,
		-21.10, -174.00,
		36.50, 10.11,
		39.57, 32.54,
		38.00, 57.50,
		-08.31, 179.13,
		00.20, 32.30,
		50.30, 30.28,
		24.28, 54.22,
		51.36, -00.05,
		-06.08, 35.45,
		39.91, -77.02,
		18.21, -64.56,
		-34.50, -56.11,
		41.20, 69.10,
		-17.45, 168.18,
		10.30, -66.55,
		21.05, 105.55,
		44.50, 20.37,
		-15.28, 28.16,
		-17.43, 31.02		
	};
	int _llen = __LEN(latlongPairs);
	for (int i = 0; i < _llen; i += 2) {
		ObjModel Pin("obj/pin.obj", "img/testA.jpg",
			      &programs[GLSLHMapProgram],
			      (i == (_llen - 2)) ?
			      BOOL_TRUE_FLAG : BOOL_FALSE_FLAG);
		Pin.Scale(glm::vec3(.021f, -.021f, .021f));
		Pin.ApplyModelTransform();
		Pin.Translate(glm::vec3(0., 1.1, 0.));
		Pin.ApplyModelTranslate();
		meshes.push_back(Pin);
	}
	/* Generate skybox */
	drawSky();
	for (int i = 0; i < _llen; i += 2)
		sendToUniverse(0, latlongPairs[i], latlongPairs[i+1]);
}
#undef __LEN


void RenderingProgram::console_print(std::string text) {
	/* Print a line of text to the main console */
	mainConsole.write(text.c_str(), (int)clock());
}


void RenderingProgram::console_print(std::string text, Console* cn) {
	/* Print a line of text to the targeted console */
	cn->write(text.c_str(), (int)clock());
}


int RenderingProgram::mainLoop(void) {
	/* Master loop */
	while (!glfwWindowShouldClose(window)) {
		if (!paused) {
			update();
		}
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	glfwSetFramebufferSizeCallback(window, NULL);
	glfwSetKeyCallback(window, NULL);
	glfwSetScrollCallback(window, NULL);
	glfwSetCursorPosCallback(window, NULL);
	return 0;
}


int main(int argc, char **argv) {
	/* Toplevel execution body */
	RenderingProgram myProgram(argc, argv);
	MainProgram = &myProgram;
	myProgram.mainLoop();
	return 0;
}
