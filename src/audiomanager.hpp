//
// Created by eric on 4/16/18.
//

#pragma once

#include <SDL_mixer.h>

namespace AudioManager {


// Audio and music
	extern Mix_Music* menuMusic;
	extern Mix_Chunk* m_mouse_click;
	extern Mix_Chunk* m_error_sound;

	bool init();

	void play_mouse_click_sound();

	void play_error_sound();
	void playLaunchMenuMusic();
	void pauseLaunchMenuMusic();

	void shutdown();

}
