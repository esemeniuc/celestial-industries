//
// Created by eric on 4/16/18.
//

#include "world.hpp"
#include "audiomanager.hpp"

namespace AudioManager {
// music and audio
	Mix_Music* menuMusic;
	Mix_Chunk* m_mouse_click;
	Mix_Chunk* m_error_sound;

	void play_mouse_click_sound() {
		Mix_PlayChannel(-1, m_mouse_click, 0);
	}

	void play_error_sound() {
		Mix_PlayChannel(-1, m_error_sound, 0);
	}

	void playLaunchMenuMusic() {
		Mix_PlayMusic(menuMusic, -1);
	}

	void pauseLaunchMenuMusic() {
		Mix_PauseMusic();
	}

	bool init() {
		// Loading music and sounds
		if (SDL_Init(SDL_INIT_AUDIO) < 0) {
			logger(LogLevel::ERR) << "Failed to initialize SDL Audio\n";
			return false;
		}

		// load support for the OGG audio format
		int flags = MIX_INIT_OGG;
		int initted = Mix_Init(flags);
		if ((initted & flags) != flags) { // Bitwise & intentional; follows SDL docs
			logger(LogLevel::ERR) << "Mix_Init: Failed to init required ogg support!\n";
			logger(LogLevel::ERR) << "Mix_Init: " << Mix_GetError() << '\n';
		}

// open 44.1KHz, signed 16bit, system byte order,
// stereo audio, using 2048 byte chunks
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
			logger(LogLevel::ERR) << "Failed to open audio device";
			return false;
		}

		menuMusic = Mix_LoadMUS(audio_path("launchMenu/game_sound_track.ogg"));
		// LoadWAV is actaully capable of loading other audio formats as well, the name is not accurate
		// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_19.html#SEC19
		m_mouse_click = Mix_LoadWAV(audio_path("buttons/click.ogg"));
		m_error_sound = Mix_LoadWAV(audio_path("buttons/uiError.ogg"));

		if (menuMusic == nullptr) {
			logger(LogLevel::ERR) << "Failed to load sounds, make sure the data directory is present";
			return false;
		}

		return true;
	}

	void shutdown() {
		if (menuMusic != nullptr) {
			Mix_FreeMusic(menuMusic);
		}

		if (m_mouse_click != nullptr) {
			Mix_FreeChunk(m_mouse_click);
		}

		if (m_error_sound != nullptr) {
			Mix_FreeChunk(m_error_sound);
		}

		// cleans up all dynamically loaded library handles used by sdl mixer
		Mix_CloseAudio();
		Mix_Quit();
	}


}