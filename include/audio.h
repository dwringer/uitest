#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "AL/al.h"
#include "AL/alc.h"
#include "sndfile.hh"
#include <iostream>
#include <cstring>


#define BUFFER_COUNT 32
#define BUFFER_SIZE 8*1024


class WavePlayer {
public:
	WavePlayer(void);
	~WavePlayer(void);
	int isPlaying(void);
	void adjustVolume(float);
	void toggleMute(void);
	void join(void);
	void playBlocked(const char* filename);
	void playWav(const char* filename);
	void playWav(const char* filename, float pitch, float gain);
	void update(void);
	bool muted;
private:
	SndfileHandle file;
	ALuint buffers[BUFFER_COUNT];
	int bufferIdx;
	int fileIndex;
	const char* currentFilename;
	ALCcontext* context;
	ALCdevice* device;
	ALCenum error;
	ALuint source;
	void clearBuffersAndSource(void);
	void createBuffers(void);
	void createContext(void);
	ALenum getALFormatFromSndfile(SndfileHandle handle);
	void loadWav(const char* filename);
	void openDevice(void);
	void play(void);
	void setupListener(void);
	void setupListener(float atx, float aty, float atz,
			   float upx, float upy, float upz,
			   float posx, float posy, float posz,
			   float velx, float vely, float velz);
	void setupSource(void);
	void setupSource(float pitch, float gain,
			 float posx, float posy, float posz,
			 float velx, float vely, float velz,
			 bool loop);
	void showALErrors(void);
	bool playing;
	float currentGain;
};

#endif
