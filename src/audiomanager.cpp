//
// Created by eric on 4/16/18.
//

#include "audiomanager.hpp"
#include "config.hpp" //for audio path

#include <SDL.h> //for sdl_init


namespace AudioManager {
	//audio
	Mix_Chunk* m_mouse_click;
	Mix_Chunk* m_error_sound;
	std::array<Mix_Chunk*, 2> buildingPlacementSound;

	// music
	Mix_Music* menuMusic;
	Mix_Music* currentSong;
	std::vector<Mix_Music*> mainGameMusic;
	Mix_Chunk* winSound;
	Mix_Chunk* loseSound;

	std::array<Mix_Chunk*, Model::MeshType::MESHTYPES_COUNT> attackSounds{}; //init to null
	std::array<Mix_Chunk*, Model::MeshType::MESHTYPES_COUNT> deathSounds{};

	void play_mouse_click_sound() {
		Mix_PlayChannel(-1, m_mouse_click, 0);
	}

	void play_error_sound() {
		Mix_PlayChannel(-1, m_error_sound, 0);
	}

	void playPlaceBuildingSound() {
		int randomNum = int(std::rand() % buildingPlacementSound.size());

		Mix_PlayChannel(-1, buildingPlacementSound[randomNum], 0);
	}

	void playWinSound() {
		Mix_PlayChannel(-1, winSound, 0);

	}

	void playLoseSound() {
		Mix_PlayChannel(-1, loseSound, 0);
	}

	void startLaunchMenuMusic() {
		currentSong = menuMusic;
		Mix_PlayMusic(menuMusic, -1); // Plays background music indefinitely
	}

	void pauseGameMusic() {
		Mix_PauseMusic();
	}

	void resumeGameMusic() {
		Mix_ResumeMusic();
	}

	void stopCurrentSong() {
		Mix_HookMusicFinished(NULL); //don't automatically play another song
		Mix_HaltMusic();
	}

	Mix_Music* getRandomSong() {
		auto randIt = mainGameMusic.begin();
		std::advance(randIt, std::rand() % mainGameMusic.size());
		return *randIt;
	}

	void startMainGameMusic() {
		auto cycleThruSongs = []() {
			logger(LogLevel::DEBUG) << "song finished, playing next song\n";
			currentSong = getRandomSong();
			Mix_FadeInMusic(currentSong, 1, 2000);
		};

		Mix_VolumeMusic(MIX_MAX_VOLUME / 4); //25% volume for game music
		Mix_HookMusicFinished(cycleThruSongs);
		currentSong = getRandomSong();
		Mix_FadeInMusic(currentSong, 1, 2000); //fade it gradually
	}

	bool loadUnitSounds() {
		auto loadAndCheck = [](Model::MeshType meshType, const char* filePath) -> bool {
			char path[100] = audio_path("");
			attackSounds[meshType] = Mix_LoadWAV(strcat(path, filePath));
			if (attackSounds[meshType] == nullptr) {
				logger(LogLevel::ERR) << "Failed to load audio file: " << filePath << '\n';
				return false;
			}
			return true;
		};

		return loadAndCheck(Model::MeshType::GUN_TURRET, "attack/gunTurret.ogg") &&
			   loadAndCheck(Model::MeshType::FRIENDLY_RANGED_UNIT, "attack/howdahPistol.ogg") &&
			   loadAndCheck(Model::MeshType::ENEMY_RANGED_RADIUS_UNIT,
							"attack/laser.ogg");//FIXME: should have a diff animation other than lasers
	}

	bool loadBuildingPlacementSounds() {
		buildingPlacementSound[0] = Mix_LoadWAV(audio_path("building/placeBuilding1.ogg"));
		buildingPlacementSound[1] = Mix_LoadWAV(audio_path("building/placeBuilding2.ogg"));
		if (buildingPlacementSound[0] == nullptr ||
			buildingPlacementSound[1] == nullptr) {
			logger(LogLevel::ERR) << "Failed to load building placement sound\n";
			return false;
		}

		return true;
	}

	bool loadSongs() {
		const char* songs[] = {"Bog-Creatures-On-the-Move_Looping.ogg",
							   "Spells-a-Brewin_Looping.ogg",
							   "Fantasy_Game_Background_Looping.ogg"};

		for (auto song : songs) {
			char path[100] = audio_path("gameplayMusic/");
			Mix_Music* temp = Mix_LoadMUS(strcat(path, song));
			if (temp == nullptr) {
				logger(LogLevel::ERR) << "Failed to load audio file: " << song << '\n';
				return false;
			} else {
				mainGameMusic.push_back(temp);
			}
		}
		return true;
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
		winSound = Mix_LoadWAV(audio_path("youwin.ogg"));
		loseSound = Mix_LoadWAV(audio_path("youlose.ogg"));

		// LoadWAV is actually capable of loading other audio formats as well, the name is not accurate
		// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_19.html#SEC19
		m_mouse_click = Mix_LoadWAV(audio_path("buttons/click.ogg"));
		m_error_sound = Mix_LoadWAV(audio_path("buttons/uiError.ogg"));

		if (menuMusic == nullptr ||
			winSound == nullptr ||
			loseSound == nullptr ||
			m_error_sound == nullptr ||
			m_mouse_click == nullptr) {
			logger(LogLevel::ERR) << "Failed to load sounds, make sure the data directory is present";
			return false;
		}

		return loadUnitSounds() && loadSongs() && loadBuildingPlacementSounds();
	}

	void shutdown() {
		if (menuMusic != nullptr) {
			Mix_FreeMusic(menuMusic);
		}

		if (winSound != nullptr) {
			Mix_FreeChunk(winSound);
		}
		if (loseSound != nullptr) {
			Mix_FreeChunk(loseSound);
		}

		if (m_mouse_click != nullptr) {
			Mix_FreeChunk(m_mouse_click);
		}

		if (m_error_sound != nullptr) {
			Mix_FreeChunk(m_error_sound);
		}

		for (auto sound : mainGameMusic) {
			if (sound != nullptr) {
				Mix_FreeMusic(sound);
			}
		}

		for (auto sound : buildingPlacementSound) {
			if (sound != nullptr) {
				Mix_FreeChunk(sound);
			}
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