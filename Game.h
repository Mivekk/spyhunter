#pragma once
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "Player.h"
#include "Enemy.h"
#include "Civil.h"

class Game
{
public:
	bool Init();
	void Run();
	void Stop();

	Game();
	~Game();

private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;

	SDL_Surface* charset_surface_;
	SDL_Surface* player_surface_;

	SDL_Texture* charset_texture_;
	SDL_Texture* player_texture_;

	SDL_Event event_;

	Player player_;
	Enemy enemies_[MAX_ENEMIES_AT_ONCE];
	Civil civils_[MAX_CIVILS_AT_ONCE];

	bool isPaused_;
	bool isLoadFileMenuOpen_;
	char saved_games[MAX_SAVED_GAMES][MAX_FILE_LENGTH];
	int distance_;
	int absolute_distance_;
	int time_;
	int score_;
	int bonus_points_;
	int current_road_width_;
	int next_road_width_;
	int chosen_saved_game_;
	int number_of_enemies;
	int number_of_civils;
	int next_enemy_spawnpoint;
	int next_civil_spawnpoint;
	int additional_car_spawnpoint;

private:
	bool LoadTextures();
	bool LoadSingleTexture(SDL_Surface*& surface, SDL_Texture*& texture, const char* src);

	void NewGame();
	void SetNextRoad();
	void ManageEnemies();
	void ManageCivils();

	void SaveGame();
	void LoadGame(char* fileName);

	void HandleInput(bool& quit);
	void Update();
	void Render();

	void DrawText(int x, int y, int text_size, const char* text);
	void DrawRectangle(SDL_Rect dstRect, SDL_Color color);
	void DrawRoad(int road_pos, int road_width);
	void DrawMenu();
	void DrawSavedGames();
	void DrawControls();
};