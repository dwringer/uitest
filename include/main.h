#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vgl.h>
#include <GL/wglew.h>  // Required when using 4.3 context in Windows

#include <audio.h>
#include <camera.h>
#include <console.h>
#include <objmodel.h>
#include <textrender.h>
#include <utils.h>
#include <LoadShaders.h>

#define BOOL_TRUE_FLAG 1
#define BOOL_FALSE_FLAG 0

#define WINDOW_TITLE argv[0]
#define START_FULLSCREEN BOOL_FALSE_FLAG
#define INIT_WIDTH 1280
#define INIT_HEIGHT 720
#define LATITUDE 39.629
#define LONGITUDE -79.894
#define SKYBOX_SIZE 8500.f
#define DAYS_PER_SECOND (1./128.)
#define MAX_DT (1./60.)
#define SPEED_MOD 1.
#define MAX_SIMULATION_SPEED (512*1024)
#define MIN_SIMULATION_SPEED (1./512)
#define CAMERA_CHANGE_MIN_SECONDS 3.
#define CAMERA_CHANGE_MAX_SECONDS 13.
#define BASE_CONSOLE_WIDTH 768
#define BASE_CONSOLE_HEIGHT 768
#define FPS_UPDATE_HZ 5
#define CAMERA_UPDATE_HZ 2
#define TARGET_UPDATE_HZ 3
#define FOV_MINIMUM 0.
#define FOV_MAXIMUM 170.
#define MOUSE_X_SENSITIVITY .5
#define MOUSE_Y_SENSITIVITY .23

#define SIDEREAL_DAYS_PER_SECOND (DAYS_PER_SECOND * (366./365.))


enum SHADER_PROGRAM_IDS { GLSLMainProgram, // No shading - flat texture
			  GLSLTextProgram, // 2D screen coordinates
			  GLSLHMapProgram  // Vertex displacement, shading
};


enum InteractionMode { IMODE_TEXT,    // Keys mapped to text input
		       IMODE_CONTROL  // Keys mapped to control input
};


class RenderingProgram {
public:
	RenderingProgram(void);
	RenderingProgram(int argc, char **argv);
	~RenderingProgram(void);
	void resize(int, int);
	void keys(int, int, int, int);
	void scroll(double, double);
	void cursorPos(double, double);
	void init(void);
	void toggleFullscreen(void);
	void pause(void);
	void unpause(void);
	void speedUp(void);
	void speedDown(void);
	void translateWorldNS(float);
	void translateWorldEW(float);
	void translateWorldFB(float);
	void translateWorldLR(float);
	void adjustCameraFov(float);
	void draw(ObjModel);
	void update(void);
	void pStep(float);
       	void display(void);
	void drawShapes(void);
	void drawSky(void);
	void changeCoordinates(float, float);
	void sendToUniverse(int, float, float);
	glm::vec2 getCoordinates(void);
	void printCoordinates(void);
	void console_print(std::string);
	void console_print(std::string, Console*);
	int mainLoop(void);

private:
	InteractionMode interactionMode;
	FT_Library ft;
	Font fontRenderer;
	Font regularFont;
	std::vector< std::string > songs;
	int songIndex;
	clock_t initFrame;
	clock_t lastFrame;
	clock_t lastDisplayFrame;
	float dt;
	float maxDt;
	float speedMod;
	float measuredFPS;
	float accumulatedTime;
	int lastFPSUpdateFrame;
	int lastCameraUpdateFrame;
	int lastTargetUpdateFrame;
	ShaderLoader Loader;
	std::vector< GLuint > programs;
	GLint uniformMVP;
	std::vector< ObjModel > meshes;
	std::vector< ObjModel > universeMeshes;
	float Latitude;
	float Longitude;
	glm::vec4 translationVelocityNWSE;
	glm::vec4 translationVelocityFLBR;
	glm::vec4 cursorVelocityULDR;
	glm::vec4 cameraVelocityUADC;
	glm::vec4 cameraVelocityFLBR;
	glm::vec2 fovAdjustment;
	WavePlayer sound;
	GLFWwindow* window;
	GLFWmonitor* monitor;
	const GLFWvidmode* vidmode;
	int Width;
	int Height;
	glm::vec4 savedWindowDimensions;
	int cameraIndex;
	clock_t lastCameraChange;
	clock_t nextCameraChange;
	std::vector< Camera > cameras;
	Console mainConsole;
	Console fpsConsole;
	Console centerConsole;
	Console cameraConsole;
	Console targetConsole;
	std::vector< Console* > consoles;
	bool paused;
	bool isFullscreen;
	bool showGlobe;
	bool cameraSwitching;
	void keysTextInput(int, int, int, int);
	void keysControlInput(int, int, int, int);
};
