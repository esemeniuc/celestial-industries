//
// Created by eric on 4/16/18.
//

#include "audiomanager.hpp"
#include "config.hpp" //for audio path

#include <SDL.h> //for sdl_init


namespace AudioManager {
// music and audio
	Mix_Music* menuMusic;
	Mix_Chunk* m_mouse_click;
	Mix_Chunk* m_error_sound;

	std::array<Mix_Chunk*, Model::MeshType::MESHTYPES_COUNT> attackSounds{}; //init to null
	std::array<Mix_Chunk*, Model::MeshType::MESHTYPES_COUNT> deathSounds{};

	void play_mouse_click_sound() {
		Mix_PlayChannel(-1, m_mouse_click, 0);
	}

	void play_error_sound() {
		Mix_PlayChannel(-1, m_error_sound, 0);
	}

	void playLaunchMenuMusic() {
		Mix_PlayMusic(menuMusic, -1); // Plays background music indefinitely
	}

	void pauseLaunchMenuMusic() {
		Mix_PauseMusic();
	}

	void loadUnitSounds() {
		auto loadAndCheck = [](Model::MeshType meshType, const char* filePath) {
			char path[100] = audio_path("");
			attackSounds[meshType] = Mix_LoadWAV(strcat(path, filePath));
			if (attackSounds[meshType] == nullptr) {
				logger(LogLevel::ERR) << "Failed to load audio file: " << filePath << '\n';
			}
		};

		loadAndCheck(Model::MeshType::GUN_TURRET, "attack/howdahPistol.ogg");
		loadAndCheck(Model::MeshType::FRIENDLY_RANGED_UNIT, "attack/gunTurret.ogg");
		loadAndCheck(Model::MeshType::ENEMY_RANGED_RADIUS_UNIT,
					 "attack/laser.ogg");//FIXME: should have a diff animation other than lasers

	}

	void playAttackSound(Model::MeshType meshType) {
		Mix_PlayChannel(-1, attackSounds[meshType], 0);
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

		loadUnitSounds();

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

		for (auto sound : attackSounds) {
			if (sound != nullptr) {
				Mix_FreeChunk(sound);
			}
		}

		for (auto sound : deathSounds) {
			if (sound != nullptr) {
				Mix_FreeChunk(sound);
			}
		}
		// cleans up all dynamically loaded library handles used by sdl mixer
		Mix_CloseAudio();
		Mix_Quit();
	}


}