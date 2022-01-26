#include <SDL/SDL_mixer.h>
#include <map>
#include <string>
#include <vector>

#pragma once

namespace gengine {
	class soundEffect {
	public:
		friend class audioEngine;

		//Play the audio file
		//@param loop: if loop == -1, loop forever, other plays loop times
		//@param channel: if channel == -1, takes one of the available channels
		void play(int loop = 0, int channel = -1);
	private:
		Mix_Chunk* _chunk = nullptr;
	};

	class music {
	public:
		friend class audioEngine;

		//Play the audio file
		//@param loop: if loop == -1, loop forever
		//Otherwise plays loop times
		void play(int loop = 0);

		static void pause();
		static void stop();
		static void resume();
	private:
		Mix_Music* _music = nullptr;
	};

	class audioEngine
	{
	public:
		static void init();
		static void destroy();

		static soundEffect loadSoundEffect(const std::string& filePath);
		static music loadMusic(const std::string& filePath);

		static void setChannelVolume(int channel, int volume); //On a scale of 0 to 100
		static int getChannelVolume(int channel);

		static int getAudioFileLength(const std::string& filePath);

	private:
		static std::map<std::string, soundEffect*> _effectMap;
		static std::map<std::string, music*> _musicMap;

		static const int CHANNEL_NUM = 256;
		static int _channelVolume[CHANNEL_NUM];


		static bool _isInitialized;
	};
}

