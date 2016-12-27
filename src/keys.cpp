#include <main.h>

#define __IMODE_CASE(MODE, F) case MODE: F(key, scancode, action, mods); break;
void RenderingProgram::keys(int key, int scancode, int action, int mods) {
	/* GLFW keystroke callback handler */
	switch(interactionMode) {
		__IMODE_CASE(IMODE_CONTROL, keysControlInput);
		__IMODE_CASE(IMODE_TEXT, keysTextInput);
	}
}
#undef __IMODE_CASE


#define IF_SHIFTED(REPR)  if (_shift) { _c = REPR; break; }
void RenderingProgram::keysTextInput(int key,
				     int scancode, int action, int mods) {
	bool _shift = mods & GLFW_MOD_SHIFT == GLFW_MOD_SHIFT;
	char _c = -1;
	if (action == GLFW_PRESS) {
	} else if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_UNKNOWN:
		case GLFW_KEY_SPACE:
		case GLFW_KEY_APOSTROPHE:
		case GLFW_KEY_COMMA:
		case GLFW_KEY_MINUS:
		case GLFW_KEY_PERIOD:
		case GLFW_KEY_SLASH:
			break;
		case GLFW_KEY_0:  IF_SHIFTED(')');
		case GLFW_KEY_1:  IF_SHIFTED('!');
		case GLFW_KEY_2:  IF_SHIFTED('@');
		case GLFW_KEY_3:  IF_SHIFTED('#');
		case GLFW_KEY_4:  IF_SHIFTED('$');
		case GLFW_KEY_5:  IF_SHIFTED('%');
		case GLFW_KEY_6:  IF_SHIFTED('^');
		case GLFW_KEY_7:  IF_SHIFTED('&');
		case GLFW_KEY_8:  IF_SHIFTED('*');
		case GLFW_KEY_9:  IF_SHIFTED('(');
			_c = (char)((int)('0') + (key - GLFW_KEY_0));
			break;
		case GLFW_KEY_SEMICOLON:
		case GLFW_KEY_EQUAL:
			break;
		case GLFW_KEY_A:
		case GLFW_KEY_B:
		case GLFW_KEY_C:
		case GLFW_KEY_D:
		case GLFW_KEY_E:
		case GLFW_KEY_F:
		case GLFW_KEY_G:
		case GLFW_KEY_H:
		case GLFW_KEY_I:
		case GLFW_KEY_J:
		case GLFW_KEY_K:
		case GLFW_KEY_L:
		case GLFW_KEY_M:
		case GLFW_KEY_N:
		case GLFW_KEY_O:
		case GLFW_KEY_P:
		case GLFW_KEY_Q:
		case GLFW_KEY_R:
		case GLFW_KEY_S:
		case GLFW_KEY_T:
		case GLFW_KEY_U:
		case GLFW_KEY_V:
		case GLFW_KEY_W:
		case GLFW_KEY_X:
		case GLFW_KEY_Y:
		case GLFW_KEY_Z:
			_c = (char)((int)(_shift ? 'A' : 'a') +
				    (key - GLFW_KEY_A));
			break;
		case GLFW_KEY_LEFT_BRACKET:
		case GLFW_KEY_BACKSLASH:
		case GLFW_KEY_RIGHT_BRACKET:
			break;
		case GLFW_KEY_GRAVE_ACCENT:
			if (action==GLFW_RELEASE)
				interactionMode = IMODE_CONTROL;
			break;
		case GLFW_KEY_WORLD_1:
		case GLFW_KEY_WORLD_2:
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_ENTER:
		case GLFW_KEY_TAB:
		case GLFW_KEY_BACKSPACE:
		case GLFW_KEY_INSERT:
		case GLFW_KEY_DELETE:
		case GLFW_KEY_RIGHT:
		case GLFW_KEY_LEFT:
		case GLFW_KEY_DOWN:
		case GLFW_KEY_UP:
		case GLFW_KEY_PAGE_UP:
		case GLFW_KEY_PAGE_DOWN:
		case GLFW_KEY_HOME:
		case GLFW_KEY_END:
		case GLFW_KEY_CAPS_LOCK:
		case GLFW_KEY_SCROLL_LOCK:
		case GLFW_KEY_NUM_LOCK:
		case GLFW_KEY_PRINT_SCREEN:
		case GLFW_KEY_PAUSE:
		case GLFW_KEY_F1:
		case GLFW_KEY_F2:
		case GLFW_KEY_F3:
		case GLFW_KEY_F4:
		case GLFW_KEY_F5:
		case GLFW_KEY_F6:
		case GLFW_KEY_F7:
		case GLFW_KEY_F8:
		case GLFW_KEY_F9:
		case GLFW_KEY_F10:
		case GLFW_KEY_F11:
		case GLFW_KEY_F12:
		case GLFW_KEY_F13:
		case GLFW_KEY_F14:
		case GLFW_KEY_F15:
		case GLFW_KEY_F16:
		case GLFW_KEY_F17:
		case GLFW_KEY_F18:
		case GLFW_KEY_F19:
		case GLFW_KEY_F20:
		case GLFW_KEY_F21:
		case GLFW_KEY_F22:
		case GLFW_KEY_F23:
		case GLFW_KEY_F24:
		case GLFW_KEY_F25:
		case GLFW_KEY_KP_0:
		case GLFW_KEY_KP_1:
		case GLFW_KEY_KP_2:
		case GLFW_KEY_KP_3:
		case GLFW_KEY_KP_4:
		case GLFW_KEY_KP_5:
		case GLFW_KEY_KP_6:
		case GLFW_KEY_KP_7:
		case GLFW_KEY_KP_8:
		case GLFW_KEY_KP_9:
		case GLFW_KEY_KP_DECIMAL:
		case GLFW_KEY_KP_DIVIDE:
		case GLFW_KEY_KP_MULTIPLY:
		case GLFW_KEY_KP_SUBTRACT:
		case GLFW_KEY_KP_ADD:
		case GLFW_KEY_KP_ENTER:
		case GLFW_KEY_KP_EQUAL:
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_LEFT_ALT:
		case GLFW_KEY_LEFT_SUPER:
		case GLFW_KEY_RIGHT_SHIFT:
		case GLFW_KEY_RIGHT_CONTROL:
		case GLFW_KEY_RIGHT_ALT:
		case GLFW_KEY_RIGHT_SUPER:
		case GLFW_KEY_MENU:
		 // case GLFW_KEY_LAST:
			break;
		}
		ostringstream _cstr;
		_cstr << _c;
		if (_c != -1) mainConsole.input(_c);
	}
}


void RenderingProgram::keysControlInput(int key,
					int scancode, int action, int mods) {
	bool _shift = mods & GLFW_MOD_SHIFT == GLFW_MOD_SHIFT;
	if (action == GLFW_PRESS) {
		/// Immediately activate on keypress
		switch(key) {
		case GLFW_KEY_HOME:
			// Home : Camera left
			cameraVelocityUADC[1] += .001;
			break;
		case GLFW_KEY_END:
			// End : Camera right
			cameraVelocityUADC[3] += .001;
			break;
		case GLFW_KEY_PAGE_UP:
			// PgUp : Camera up
			cameraVelocityUADC[0] += .001;
			break;
		case GLFW_KEY_PAGE_DOWN:
			// PgDn : Camera down
			cameraVelocityUADC[2] += .001;
			break;
		case GLFW_KEY_UP:
			translationVelocityNWSE[0] += 1.;
			break;
		case GLFW_KEY_LEFT:
			translationVelocityNWSE[1] += 1.;
			break;
		case GLFW_KEY_DOWN:
			translationVelocityNWSE[2] += 1.;
			break;
		case GLFW_KEY_RIGHT:
			translationVelocityNWSE[3] += 1.;
			break;
		case GLFW_KEY_KP_ADD:
			fovAdjustment[1] += glm::radians(1.);
			break;
		case GLFW_KEY_KP_SUBTRACT:
			fovAdjustment[0] += glm::radians(1.);
			break;
		case GLFW_KEY_KP_8:
			cursorVelocityULDR[0] += 1.;
			break;
		case GLFW_KEY_KP_4:
			cursorVelocityULDR[1] += 1.;
			break;
		case GLFW_KEY_KP_2:
			cursorVelocityULDR[2] += 1.;
			break;
		case GLFW_KEY_KP_6:
			cursorVelocityULDR[3] += 1.;
			break;
		case GLFW_KEY_W:
			// W: Move forward
			translationVelocityFLBR[0] += 1.;
			break;
		case GLFW_KEY_A:
			translationVelocityFLBR[1] += 1.;
			break;
		case GLFW_KEY_S:
			translationVelocityFLBR[2] += 1.;
			break;
		case GLFW_KEY_D:
			translationVelocityFLBR[3] += 1.;
			break;
		}
	} else if (action == GLFW_RELEASE) {
		/// (Do nothing until the key is lifted)
		switch (key) {
		case GLFW_KEY_ESCAPE:
			// ESC : exit the program
			glfwSetWindowShouldClose(window, BOOL_TRUE_FLAG);
			break;
		case GLFW_KEY_1:
		case GLFW_KEY_2:
		case GLFW_KEY_3:
		case GLFW_KEY_4:
		case GLFW_KEY_5:
		case GLFW_KEY_6:
		case GLFW_KEY_7:
		case GLFW_KEY_8:
		case GLFW_KEY_9: {
			ostringstream _camSel;
			int _ci = key - (int)GLFW_KEY_1;
			cameraSwitching = BOOL_FALSE_FLAG;
			cameraIndex = std::min(_ci, (int)cameras.size() - 1);
			_camSel << "Camera " << cameraIndex + 1 << " selected";
			console_print(_camSel.str());
		}
			break;
		case GLFW_KEY_0: {
			ostringstream _camSel;
			cameraSwitching = BOOL_FALSE_FLAG;
			cameraIndex = std::min(10, (int)cameras.size() - 1);
			_camSel << "Camera " << cameraIndex << " selected";
			console_print(_camSel.str());
		}
			break;
		case GLFW_KEY_MINUS:
			if (_shift) {
			} else {
				// - : decrease simulation speed
				speedDown();
			}
				break;
		case GLFW_KEY_EQUAL:
			if (_shift) {
				// + : increase simulation speed
				speedUp();
			}
			break;
		case GLFW_KEY_Q: {
			// Q : print cam orientation
			if (cameraConsole.status()) {
				cameraConsole.disable();
				console_print("Camera console hidden");
			} else {
				cameraConsole.enable();
				console_print("Camera console enabled");
			}
			break;
		}
		case GLFW_KEY_W:
			// W: Move forward
			translationVelocityFLBR[0] = 0.;
			break;
		case GLFW_KEY_T: {
			// T : show target orientation
			if (targetConsole.status()) {
				targetConsole.disable();
				console_print("Target console hidden");
			} else {
				targetConsole.enable();
				console_print("Target console enabled");
			}
			break;
		}
		case GLFW_KEY_P:
			// P : Pause simulation
			if (paused) {
				unpause();
			} else {
				pause();
			}
			break;
		case GLFW_KEY_A:
			translationVelocityFLBR[1] = 0.;
			break;
		case GLFW_KEY_S:
			translationVelocityFLBR[2] = 0.;
			break;
		case GLFW_KEY_D:
			translationVelocityFLBR[3] = 0.;
			break;
		case GLFW_KEY_F:
			// F : Toggle FPS counter
			if (fpsConsole.status()) {
				fpsConsole.disable();
				console_print("FPS count hidden");
			} else {
				fpsConsole.enable();
				console_print("FPS count enabled");
			}
			break;
		case GLFW_KEY_G:
			// G : Toggle globe
			showGlobe = showGlobe ?
				BOOL_FALSE_FLAG : BOOL_TRUE_FLAG;
			console_print(showGlobe ?
				      "Globe enabled" : "Globe disabled");
			break;
		case GLFW_KEY_C:
			// C : toggle camera switching
			cameraSwitching = cameraSwitching?
				BOOL_FALSE_FLAG : BOOL_TRUE_FLAG;
			console_print(cameraSwitching ?
				      "Camera switching enabled" :
				      "Camera switching disabled");
			break;
		case GLFW_KEY_M:
			// M : Toggle mute
			sound.toggleMute();
			console_print(sound.muted ? "Muted" : "Unmuted");
			break;
		case GLFW_KEY_GRAVE_ACCENT:
			interactionMode = IMODE_TEXT;
			break;
		case GLFW_KEY_F11:
			// F11 : toggle full screen mode
			toggleFullscreen();
			break;
		case GLFW_KEY_KP_SUBTRACT: {
			// NUMPAD - : increase fov
			ostringstream _fovAdj;
			float _fov = cameras[cameraIndex].getFov();
			fovAdjustment[0] = 0.f;
			_fovAdj << "FOV increased: "
				<< (int)glm::degrees(_fov) << "º";
			console_print(_fovAdj.str());
		}
			break;
		case GLFW_KEY_KP_ADD: {
			// NUMPAD + : decrease fov
			ostringstream _fovAdj;
			float _fov = cameras[cameraIndex].getFov();
			fovAdjustment[1] = 0.f;
			_fovAdj << "FOV decreased: "
				<< (int)glm::degrees(_fov) << "º";
			console_print(_fovAdj.str());
		}
			break;
		case GLFW_KEY_KP_8:
			// KP up : camera up
			cursorVelocityULDR[0] = 0.;
			break;
		case GLFW_KEY_KP_4:
			// KP left : view left
			cursorVelocityULDR[1] = 0.;
			break;
		case GLFW_KEY_KP_2:
			// KP down : camera down
			cursorVelocityULDR[2] = 0.;
			break;
		case GLFW_KEY_KP_6:
			// KP right : view right
			cursorVelocityULDR[3] = 0.;
			break;
		case GLFW_KEY_PAGE_UP:
			// PgUp : Camera up
			cameraVelocityUADC[0] = 0.;
			break;
		case GLFW_KEY_HOME:
			// Home : Camera left
			cameraVelocityUADC[1] = 0.;
			break;
		case GLFW_KEY_PAGE_DOWN:
			// PgDn : Camera down
			cameraVelocityUADC[2] = 0.;
			break;
		case GLFW_KEY_END:
			// End : Camera right
			cameraVelocityUADC[3] = 0.;
			break;
		case GLFW_KEY_UP:
			// UP : move north
			translationVelocityNWSE[0] = 0.;
			printCoordinates();
			break;
		case GLFW_KEY_LEFT:
			// LEFT : move west
			translationVelocityNWSE[1] = 0.;
			printCoordinates();
			break;
		case GLFW_KEY_DOWN:
			// DOWN : move south
			translationVelocityNWSE[2] = 0.;
			printCoordinates();
			break;
		case GLFW_KEY_RIGHT:
			// RIGHT : move east
			translationVelocityNWSE[3] = 0.;
			printCoordinates();
			break;
		// ...
		}

	}
}
