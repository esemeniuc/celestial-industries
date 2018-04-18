//
// Created by eric on 4/16/18.
//

#pragma once

#include <SDL_mixer.h>
#include <array>
#include "model.hpp" //for mesh type


namespace AudioManager {

	//sound effects
	extern Mix_Chunk* m_mouse_click;
	extern Mix_Chunk* m_error_sound;
	extern std::array<Mix_Chunk*, 2> buildingPlacementSound;

	// music
	extern Mix_Music* menuMusic;
	extern Mix_Music* currentSong;
	extern std::vector<Mix_Music*> mainGameMusic;

	bool init();

	//user interaction stuff
	void play_mouse_click_sound();

	void play_error_sound();

	void playAttackSound(Model::MeshType meshType);

	void playPlaceBuildingSound();

	//game music stuff
	void startLaunchMenuMusic();

	void startMainGameMusic();

	void pauseGameMusic();

	void resumeGameMusic();

	void stopCurrentSong();

	void shutdown();

}
