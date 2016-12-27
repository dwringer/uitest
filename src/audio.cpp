#include "audio.h"


WavePlayer::WavePlayer(void) {
	currentFilename = "";
	bufferIdx = 0;
	fileIndex = 0;
	playing = false;;
	muted = false;
	device = alcOpenDevice(NULL);
	createContext();
	setupListener();
	setupSource();
	createBuffers();
}


WavePlayer::~WavePlayer(void) {
	clearBuffersAndSource();
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}


void WavePlayer::adjustVolume(float new_gain) {
	currentGain = new_gain;
	alSourcef(source, AL_GAIN, new_gain);
}


void WavePlayer::createContext(void) {
	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context))
		std::cerr << "could not make context current" << std::endl;
	showALErrors();
}


void WavePlayer::setupListener(void) {
	setupListener(0.0f, 0.0f, 1.0f,
		      0.0f, 1.0f, 0.0f,
		      0.0f, 0.0f, 1.0f,
		      0.0f, 0.0f, 0.0f);
}


void WavePlayer::setupListener(float atx, float aty, float atz,
			       float upx, float upy, float upz,
			       float posx, float posy, float posz,
			       float velx, float vely, float velz) {
	ALfloat listenerOrigin[] = {atx, aty, atz, upx, upy, upz};
	alListener3f(AL_POSITION, posx, posy, posz);
	showALErrors();
	alListener3f(AL_VELOCITY, velx, vely, velz);
	showALErrors();
	alListenerfv(AL_ORIENTATION, listenerOrigin);
	showALErrors();
}


void WavePlayer::setupSource(float pitch, float gain,
			     float posx, float posy, float posz,
			     float velx, float vely, float velz,
			     bool loop) {
	alGenSources((ALuint)1, &source);
	showALErrors();
	alSourcef(source, AL_PITCH, pitch);
	showALErrors();
	alSourcef(source, AL_GAIN, gain);
	showALErrors();
	alSource3f(source, AL_POSITION, posx, posy, posz);
	showALErrors();
	alSource3f(source, AL_VELOCITY, velx, vely, velz);
	showALErrors();
	alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	showALErrors();
}


void WavePlayer::setupSource(void) {
	setupSource(1.0f, 1.0f,
		    0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f,
		    AL_FALSE);
}


void WavePlayer::createBuffers(void) {
	alGenBuffers(BUFFER_COUNT, buffers);
	showALErrors();
}


void WavePlayer::loadWav(const char* filename) {
	file = SndfileHandle(filename);
	currentFilename = filename;
	ALenum format = getALFormatFromSndfile(file);
	short data[BUFFER_SIZE];
	int _read;
	bufferIdx = 0;
	fileIndex = 0;
	for (int i = 0; i < BUFFER_COUNT; i++) {
		memset(data, 0, BUFFER_SIZE * sizeof(short));
		_read = file.read(data, (int)BUFFER_SIZE);
		fileIndex += _read;
		alBufferData(buffers[bufferIdx],
			     format,
			     data,
			     _read * sizeof(short),
			     (ALsizei)file.samplerate());
		showALErrors();
		bufferIdx = (bufferIdx + 1) % BUFFER_COUNT;
	}
	alSourceQueueBuffers(source, BUFFER_COUNT, buffers);
}


void WavePlayer::toggleMute(void) {
	if (muted) {
		alSourcef(source, AL_GAIN, currentGain);
	} else {
		alSourcef(source, AL_GAIN, 0.f);
	}
	muted = (muted) ? false : true;
};


void WavePlayer::play() {
	playing = 1;
	alSourcePlay(source);
	showALErrors();
}


void WavePlayer::playWav(const char* filename) {
	/* To play a .wav, call this then update() until finished playing */
	clearBuffersAndSource();
	setupSource();
	createBuffers();
	loadWav(filename);
	currentGain = 1.f;
	if (muted) {
		muted = false;
		toggleMute();
	}
	play();
}


void WavePlayer::playWav(const char* filename, float pitch, float gain) {
	/* To play a .wav, call this then update() until finished playing */
	clearBuffersAndSource();
	setupSource(pitch, gain, 0, 0, 0, 0, 0, 0, AL_FALSE);
	createBuffers();
	loadWav(filename);
	currentGain = gain;
	if (muted) {
		muted = false;
		toggleMute();
	}
	play();
}


void WavePlayer::clearBuffersAndSource() {
	alDeleteSources(1, &source);
	alDeleteBuffers(BUFFER_COUNT, &buffers[0]);
}


int WavePlayer::isPlaying() {
	return playing;
}


void WavePlayer::join() {
	while (isPlaying()) showALErrors();
}


void WavePlayer::playBlocked(const char* filename) {
	playWav(filename);
	join();
}


void WavePlayer::update() {
	/* This must be called frequently enough to keep the buffers filled */
	if (currentFilename == "") return;
	int doneBuffers;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &doneBuffers);
	if (doneBuffers <= 0) return;
	if (doneBuffers == BUFFER_COUNT) {
		currentFilename = "";
		playing = 0;
		return;
	}
	//	SndfileHandle file(currentFilename);
	int _channels = file.channels();
	if (_channels == 0) return;
	if (fileIndex/_channels >= file.frames()) return;
	file.seek(fileIndex/_channels, 0);
	short data[BUFFER_SIZE];
	ALenum format = getALFormatFromSndfile(file);
	int _read;
	while(doneBuffers--) {
		memset(data, 0, BUFFER_SIZE * sizeof(short));
		_read = file.read(data, (int)BUFFER_SIZE);
		fileIndex += _read;
		alSourceUnqueueBuffers(source, 1, &buffers[bufferIdx]);
		alBufferData(buffers[bufferIdx],
			     format,
			     data,
			     _read * sizeof(short),
			     (ALsizei)file.samplerate());
		alSourceQueueBuffers(source, 1, &buffers[bufferIdx]);
		showALErrors();
		bufferIdx = (bufferIdx + 1) % BUFFER_COUNT;
	}
}


ALenum WavePlayer::getALFormatFromSndfile(SndfileHandle handle) {
	int rtn;
	switch (handle.format() & SF_FORMAT_TYPEMASK) {
	case SF_FORMAT_WAV:
		switch(handle.format() & SF_FORMAT_SUBMASK) {
		case SF_FORMAT_PCM_S8:
			rtn = (handle.channels() < 2) ?
				AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
			break;
		case SF_FORMAT_PCM_16:
			rtn = (handle.channels() < 2) ?
				AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			break;
		default:
			std::cerr << "Not an 8- or 16- bit file" << std::endl;
			return -1;
		}
		break;
	default:
		std::cerr << "Not a WAVE file!" << std::endl;
		break;
	}
	return rtn;
}


void WavePlayer::showALErrors(void) {
	error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "OpenAL error:" << '\t';
		switch(error) {
		case AL_INVALID_NAME:
			std::cerr << "invalid name";
			break;
		case AL_INVALID_ENUM:
			std::cerr << "invalid enum";
			break;
		case AL_INVALID_VALUE:
			std::cerr << "invalid value";
			break;
		case AL_INVALID_OPERATION:
			std::cerr << "invalid operation";
			break;
		case AL_OUT_OF_MEMORY:
			std::cerr << "out of memory";
			break;
		default:
			std::cerr << "unknown error";
		}
		std::cerr << std::endl;
	}

}
