//
// Created by eric on 4/16/18.
//

#pragma once

#include <SDL_mixer.h>
#include <array>
#include "model.hpp" //for mesh type


namespace AudioManager {


// Audio and music
	extern Mix_Music* menuMusic;
	extern Mix_Music* currentSong;
	extern std::vector<Mix_Music*> mainGameMusic;
	extern Mix_Chunk* m_mouse_click;
	extern Mix_Chunk* m_error_sound;

	bool init();

	void play_mouse_click_sound();

	void play_error_sound();

	void startLaunchMenuMusic();

	void startLaunchMenuMusic();

	void startMainGameMusic();

	void pauseGameMusic();

	void resumeGameMusic();

	void stopCurrentSong();

	void playAttackSound(Model::MeshType meshType);

	void shutdown();

}
