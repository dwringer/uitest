This is a simple integration of OpenGL, OpenAL-Soft,
and various utility libraries on Windows, targeted
for MinGW 32-bit.

Dependencies:
glew
glfw
glm
libsndfile
openal-soft
tiny_obj_loader
stb_image
freetype

Required headers and 32-bit Windows binaries are
included for each library, along with the appropriate
licensing information (found in "licenses/").

Compilation:
Make sure the 32-bit MinGW toolchain is in your PATH,
then compile by running "make.bat".