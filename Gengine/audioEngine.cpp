#include "audioEngine.h"
#include "errors.h"
#include <iostream>

namespace gengine {
	std::map<std::string, soundEffect*> audioEngine::_effectMap;
	std::map<std::string, music*> audioEngine::_musicMap;
	int audioEngine::_channelVolume[CHANNEL_NUM];

	bool audioEngine::_isInitialized;

	void soundEffect::play(int loops, int channel) {
		if (Mix_PlayChannel(channel, _chunk, loops) == -1) {
			fatalError("Mix_PlayChannel error:" + std::string(Mix_GetError()));
		}
	}

	void music::play(int loops) {
		if (Mix_PlayMusic(_music, loops) == -1) {
			fatalError("Mix_PlayMusic error:" + std::string(Mix_GetError()));
		}
	}

	void music::pause() {
		Mix_PauseMusic();
	}

	void music::stop() {
		Mix_HaltMusic();
	}

	void music::resume() {
		Mix_ResumeMusic();
	}

	void audioEngine::init() {
		if (_isInitialized) {
			fatalError("Tried to initialize the audio engine twice!");
		}

		if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1) {
			fatalError("Mix_Init error: " + std::string(Mix_GetError()));
		}

		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024) == -1) {
			fatalError("Mix_OpenAudio error: " + std::string(Mix_GetError()));
		}
		Mix_AllocateChannels(CHANNEL_NUM);

		for (int i = 0; i < CHANNEL_NUM; i++) {
			_channelVolume[i] = 100;
		}

		_isInitialized = true;
	}

	void audioEngine::destroy() {
		if (_isInitialized == true) {
			_isInitialized = false;

			for (auto& it : _effectMap) {
				Mix_FreeChunk(it.second->_chunk);
			}

			for (auto& it : _musicMap) {
				Mix_FreeMusic(it.second->_music);
			}
			_effectMap.clear();
			_musicMap.clear();

			Mix_CloseAudio();
			Mix_Quit();
		}
	}

	soundEffect audioEngine::loadSoundEffect(const std::string& filePath) {
		//Try to find the audio in the cache
		auto iterator = _effectMap.find(filePath);

		//Failed to find the soundEffect
		if (iterator == _effectMap.end()) {
			soundEffect* newSoundEffect = new soundEffect;

			newSoundEffect->_chunk = Mix_LoadWAV(filePath.c_str());

			_effectMap.insert(std::make_pair(filePath, newSoundEffect));
		}

		return *_effectMap[filePath];
	}

	music audioEngine::loadMusic(const std::string& filePath) {
		//Try to find the audio in the cache
		auto iterator = _musicMap.find(filePath);

		//Failed to find the soundEffect
		if (iterator == _musicMap.end()) {
			music* newMusic = new music;

			newMusic->_music = Mix_LoadMUS(filePath.c_str());

			_musicMap.insert(std::make_pair(filePath, newMusic));
		}

		return *_musicMap[filePath];
	}

	void audioEngine::setChannelVolume(int channel, int volume) {
		float volumeScaler = (float)volume;
		if (volume > 100) {
			volumeScaler = 100;
		}
		else if (volume < 0) {
			volumeScaler = 0;
		}

		int actualVolume = round(volumeScaler * 128 / 100);
		Mix_Volume(channel, actualVolume);

		if (volume < 0) {
			_channelVolume[channel] = 0;
		}
		else {
			_channelVolume[channel] = volume;
		}
	}

	int audioEngine::getChannelVolume(int channel) {
		return _channelVolume[channel];
	}

	int audioEngine::getAudioFileLength(const std::string& filePath) {
		return (int)_effectMap[filePath]->_chunk->alen / 176.4;
	}


}
