#include "Game.h"

Game::Game()
	: window_(nullptr), renderer_(nullptr), charset_surface_(nullptr),
	player_surface_(nullptr), charset_texture_(nullptr), player_texture_(nullptr),
	event_({}), isPaused_(false), isLoadFileMenuOpen_(false), saved_games(),
	distance_(0), absolute_distance_(0), time_(0), score_(0), bonus_points_(0), 
	current_road_width_(STARTING_ROAD_WIDTH), next_road_width_(STARTING_ROAD_WIDTH), 
	chosen_saved_game_(1), number_of_enemies(0), number_of_civils(0), 
	next_enemy_spawnpoint(ENEMIES_SPAWNPOINT), next_civil_spawnpoint(CIVILS_SPAWNPOINT),
	additional_car_spawnpoint(PLAYER_NEW_CAR)
{
	srand(time(NULL));

	// init enemies
	for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
		enemies_[i] = Enemy(&player_);
	}
	// init civils
	for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
		civils_[i] = Civil(&player_);
	}
}

Game::~Game()
{
}

bool Game::Init()
{
	// initialize sdl
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return false;
	}

	// create window
	window_ = SDL_CreateWindow("Spy Hunter", SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window_ == nullptr) {
		printf("SDL_Window error: %s\n", SDL_GetError());
		return false;
	}

	// create renderer
	renderer_ = SDL_CreateRenderer(window_, -1, 0);
	if (renderer_ == nullptr) {
		printf("SDL_Renderer error: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer_, SCREEN_WIDTH, SCREEN_HEIGHT);

	// load all images
	if (!LoadTextures()) {
		return false;
	}

	return true;
}

void Game::Run()
{
	int t1, t2;
	double delta;

	t1 = SDL_GetTicks();

	// main game loop
	bool quit = false;
	while (!quit) {
		// calculate time elapsed between frames
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		// restrict game to 60fps
		if (delta < SCREEN_MS_BY_FPS) {
			SDL_Delay(SCREEN_MS_BY_FPS - delta);
		}

		HandleInput(quit);

		// if game is paused dont update and render
		if (!isPaused_) {
			Update();
		}

		Render();
	}
}

void Game::Stop()
{
	SDL_DestroyWindow(window_);
	SDL_DestroyRenderer(renderer_);

	SDL_FreeSurface(charset_surface_);
	SDL_FreeSurface(player_.surface);

	SDL_DestroyTexture(charset_texture_);
	SDL_DestroyTexture(player_.texture);

	SDL_Quit();
}

bool Game::LoadTextures()
{
	// load charset from bmp file
	if (!LoadSingleTexture(charset_surface_, charset_texture_, "./bitmaps/cs8x8.bmp")) return false;

	// load car from bmp file
	if (!LoadSingleTexture(player_.surface, player_.texture, "./bitmaps/autko.bmp")) return false;

	// load enemy from bmp file
	for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
		if (!LoadSingleTexture(enemies_[i].surface, enemies_[i].texture, "./bitmaps/enemy.bmp")) return false;
	}

	// load nonenemy from bmp file
	for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
		if (!LoadSingleTexture(civils_[i].surface, civils_[i].texture, "./bitmaps/civil.bmp")) return false;
	}

	return true;
}

bool Game::LoadSingleTexture(SDL_Surface*& surface, SDL_Texture*& texture, const char* src)
{
	surface = SDL_LoadBMP(src);
	if (surface == nullptr) {
		Stop();
		return false;
	}
	SDL_SetColorKey(surface, true, 0);

	texture = SDL_CreateTextureFromSurface(renderer_, surface);

	return true;
}

void Game::NewGame()
{
	// set car starting position
	player_.rect.x = PLAYER_STARTING_X;
	player_.rect.y = PLAYER_STARTING_Y;

	// reset all values
	distance_ = 0; 
	absolute_distance_ = 0;
	time_ = 0; 
	score_ = 0; 
	isPaused_ = false;
	isLoadFileMenuOpen_ = false;
	bonus_points_ = 0;
	current_road_width_ = STARTING_ROAD_WIDTH;
	next_road_width_ = STARTING_ROAD_WIDTH;
	next_enemy_spawnpoint = ENEMIES_SPAWNPOINT;
	next_civil_spawnpoint = CIVILS_SPAWNPOINT;
	number_of_enemies = 0;
	number_of_civils = 0;
	player_.hp = 3;
	player_.unlimited_hp = true;
	player_.game_over = false;

	// init enemies
	for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
		enemies_[i].rect.x = CAR_STARTING_X;
		enemies_[i].rect.y = CAR_STARTING_Y;
		enemies_[i].isUsed = false;
	}
	// init civils
	for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
		civils_[i].rect.x = CAR_STARTING_X;
		civils_[i].rect.y = CAR_STARTING_Y;
		civils_[i].isUsed = false;
	}

	additional_car_spawnpoint = PLAYER_NEW_CAR;
}

void Game::HandleInput(bool& quit)
{
	// handling events
	while (SDL_PollEvent(&event_)) {
		switch (event_.type) {
			case SDL_KEYDOWN:
				if (event_.key.keysym.sym == SDLK_ESCAPE) { 
					quit = true; 
				}
				else if (event_.key.keysym.sym == SDLK_n) {
					NewGame();
				}
				else if (event_.key.keysym.sym == SDLK_p) {
					isPaused_ = !isPaused_;
					isLoadFileMenuOpen_ = false;
				}
				else if (event_.key.keysym.sym == SDLK_s) {
					isPaused_ = true;
				}
				else if (event_.key.keysym.sym == SDLK_l) {
					isPaused_ = true;
					isLoadFileMenuOpen_ = true;
				}
				else if (event_.key.keysym.sym == SDLK_UP) {
					if (isLoadFileMenuOpen_) chosen_saved_game_--;
				}
				else if (event_.key.keysym.sym == SDLK_DOWN) {
					if (isLoadFileMenuOpen_) chosen_saved_game_++;
				}
				else if (event_.key.keysym.sym == SDLK_RETURN) {
					if (isLoadFileMenuOpen_) {
						LoadGame(saved_games[chosen_saved_game_]);
					}
				}
				else if (event_.key.keysym.sym == SDLK_SPACE) {
					if (!player_.isDead) {
						player_.Shoot();
					}
				}
				break;
			case SDL_KEYUP:
				if (event_.key.keysym.sym == SDLK_s) {
					SaveGame();
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
		}
	}

	// movement
	// if game is paused dont move car
	if (isPaused_) return;

	player_.Move();
}

void Game::SetNextRoad()
{
	distance_ += player_.speed.vertical;
	if (distance_ > SCREEN_HEIGHT) {
		distance_ = 0;

		current_road_width_ = next_road_width_;
		// determine next random road width
		next_road_width_ += (rand() % 2 ? 50 : -50);
		if (next_road_width_ <= 300) next_road_width_ = 300;
		else if (next_road_width_ >= 600) next_road_width_ = 600;
	}
}

void Game::ManageEnemies()
{
	// spawn next enemy at some point
	if (score_ > next_enemy_spawnpoint) {
		next_enemy_spawnpoint += ENEMIES_SPAWNPOINT;

		for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
			// look for first not used enemy and spawn it there			
			if (!enemies_[i].isUsed) {
				number_of_enemies++;
				enemies_[i].Spawn();
				break;
			}
		}
	}

	// iterate over every enemy
	for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
		int correct_road_width = (distance_ > enemies_[i].rect.y) ?
			next_road_width_ : current_road_width_;
		enemies_[i].SetInfo(&number_of_enemies, &bonus_points_, correct_road_width);

		// update enemy accordingly to each road_width
		enemies_[i].Update();

		// if used enemy is too far behind despawn it
		if (enemies_[i].isUsed && enemies_[i].rect.y > SCREEN_HEIGHT * 3 / 2) {
			enemies_[i].Die();
		}

		if (enemies_[i].isUsed) {
			// check collision with player
			enemies_[i].HandleCollision(&player_, 500);

			// check collision with other cars
			for (int j = 0; j < MAX_CIVILS_AT_ONCE; j++) {
				if (civils_[j].isUsed) {
					enemies_[i].HandleCollision(&civils_[j], 0);
				}
			}
		}
	}
}

void Game::ManageCivils()
{
	// spawn civil at some point
	if (score_ > next_civil_spawnpoint) {
		next_civil_spawnpoint += CIVILS_SPAWNPOINT;

		for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
			if (!civils_[i].isUsed) {
				number_of_civils++;
				civils_[i].Spawn();
				break;
			}
		}
	}

	// iterate over every civil
	for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
		int correct_road_width = (distance_ > civils_[i].rect.y) ?
			next_road_width_ : current_road_width_;
		civils_[i].SetInfo(&number_of_civils, &bonus_points_, correct_road_width);

		// update civil accordingly to each road_width
		civils_[i].Update();

		// if used civil is too far behind despawn it
		if (civils_[i].isUsed && civils_[i].rect.y > SCREEN_HEIGHT * 3 / 2) {
			civils_[i].Die();
		}

		// check if civil collides with player
		if (civils_[i].isUsed) {
			civils_[i].HandleCollision(&player_, -500);

			// check collision with other cars
			for (int j = 0; j < MAX_ENEMIES_AT_ONCE; j++) {
				if (enemies_[j].isUsed) {
					civils_[i].HandleCollision(&enemies_[j], 0);
				}
			}
		}
	}
}

void Game::SaveGame()
{
	// get current date
	time_t current_time = time(0);
	tm* time_values = gmtime(&current_time);

	// convert date to buffer
	char buffer[128];
	sprintf(buffer, "%04d-%02d-%02d %02d %02d %02d",
		time_values->tm_year + 1900, time_values->tm_mon + 1, time_values->tm_mday,
		time_values->tm_hour + 1, time_values->tm_min, time_values->tm_sec);
	strcat(buffer, ".txt");

	char fileName[128] = SAVED_GAMES_FOLDER_NAME;
	strcat(fileName, buffer);

	// write all necessary info to the file
	FILE* file = fopen(fileName, "w");
	if (file == nullptr) {
		printf("Couldn't open file");
		return;
	}
	fprintf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n", isPaused_, distance_, absolute_distance_, 
		time_, score_, current_road_width_, next_road_width_);
	// write player info
	fprintf(file, "%d\n%d %d %d %d\n%d %d\n%d\n", player_.game_over, player_.rect.x, player_.rect.y,
		player_.rect.w, player_.rect.h, player_.speed.horizontal, player_.speed.vertical, player_.hp);

	for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
		// write enemies info
		fprintf(file, "%d\n%d %d %d %d\n%d %d\n%d\n%d\n", number_of_enemies, enemies_[i].rect.x, enemies_[i].rect.y,
			enemies_[i].rect.w, enemies_[i].rect.h, enemies_[i].speed.horizontal, 
			enemies_[i].speed.vertical, enemies_[i].isUsed, enemies_[i].hp);
	}
	for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
		// write civils info
		fprintf(file, "%d\n%d %d %d %d\n%d %d\n%d\n%d\n", number_of_civils, civils_[i].rect.x, civils_[i].rect.y,
			civils_[i].rect.w, civils_[i].rect.h, civils_[i].speed.horizontal,
			civils_[i].speed.vertical, civils_[i].isUsed, civils_[i].hp);
	}

	fclose(file);

	// append savedGamesInfo file 
	file = fopen(SAVED_GAMES_FOLDER_NAME "savedGamesInfo.txt", "a");
	if (file == nullptr) {
		printf("Couldn't open savedGamesInfo file");
		return;
	}
	fprintf(file, "%s\n", fileName);
	fclose(file);
}

void Game::LoadGame(char* fileName)
{
	// open file provided by saved_games[] array
	FILE* file = fopen(fileName, "r");
	if (file == nullptr) {
		printf("Couldn't open file");
		return;
	}
	
	int tmp;
	// read every property of file
	int result = fscanf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%d", &tmp, &distance_, &absolute_distance_,
		&time_, &score_, &current_road_width_, &next_road_width_);
	isPaused_ = tmp;

	// scan player info
	fscanf(file, "%d\n%d %d %d %d\n%d %d %d\n", &tmp, &player_.rect.x, &player_.rect.y,
		&player_.rect.w, &player_.rect.h, &player_.speed.horizontal, &player_.speed.vertical, &player_.hp);
	player_.game_over = tmp;

	for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
		// scan enemies info
		fscanf(file, "$d\n%d %d %d %d\n%d %d\n%d\n%d\n", number_of_enemies, &enemies_[i].rect.x, 
			&enemies_[i].rect.y, &enemies_[i].rect.w, &enemies_[i].rect.h, &enemies_[i].speed.horizontal,
			&enemies_[i].speed.vertical, &tmp, &enemies_[i].hp);
		enemies_[i].isUsed = tmp;
	}
	for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
		int tmp;
		// scan civils info
		fscanf(file, "$d\n%d %d %d %d\n%d %d\n%d\n%d\n", number_of_civils, &civils_[i].rect.x, 
			&civils_[i].rect.y, &civils_[i].rect.w, &civils_[i].rect.h, &civils_[i].speed.horizontal,
			&civils_[i].speed.vertical, &tmp, &civils_[i].hp);
		enemies_[i].isUsed = tmp;
	}

	if (result == -1) {
		printf("Error during reading contents of file");
		return;
	}

	fclose(file);
}

void Game::Update()
{
	// increment time
	time_++;

	if (time_ / 60 >= 20) {
		player_.unlimited_hp = false;
	}

	// increment absolute distance
	absolute_distance_ += player_.speed.vertical;

	// set size of next road
	SetNextRoad();

	ManageEnemies();
	ManageCivils();

	int correct_road_width = (distance_ > player_.rect.y) ?
		next_road_width_ : current_road_width_;
	player_.SetInfo(current_road_width_);
	player_.Update();

	if (player_.game_over) {
		isPaused_ = true;
	}

	if ((score_ + bonus_points_) > additional_car_spawnpoint) {
		additional_car_spawnpoint += PLAYER_NEW_CAR;
		player_.hp++;
	}
}

void Game::Render()
{
	SDL_RenderClear(renderer_);

	// draw roads
	DrawRoad(distance_, current_road_width_);
	DrawRoad(distance_ - SCREEN_HEIGHT, next_road_width_);

	// draw player
	SDL_RenderCopy(renderer_, player_.texture, NULL, &player_.rect);
	
	// draw bullets
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (player_.bullets[i].isUsed) {
			DrawRectangle(player_.bullets[i].rect, { 252, 88, 236, 255 });
		}
	}

	// draw enemies
	for (int i = 0; i < MAX_ENEMIES_AT_ONCE; i++) {
		SDL_RenderCopy(renderer_, enemies_[i].texture, NULL, &enemies_[i].rect);
	}
	
	// draw civils
	for (int i = 0; i < MAX_CIVILS_AT_ONCE; i++) {
		SDL_RenderCopy(renderer_, civils_[i].texture, NULL, &civils_[i].rect);
	}

	// draw menu
	DrawMenu();

	// draw time elapsed
	char text[128];
	sprintf(text, "%d", time_ / 60);
	DrawText(SCREEN_WIDTH / 2 - 30, 30, 30, text);

	// draw score
	score_ = absolute_distance_ / 200 * 50;
	int total_score = score_ + bonus_points_;
	sprintf(text, "%d", total_score > 0 ? total_score : 0);
	DrawText(10, 10, 30, text);

	// draw author info
	DrawText(SCREEN_WIDTH - 220, 10, 10, "Lukasz Machutt 193517");

	// draw amount of cars
	if (player_.unlimited_hp) {
		DrawText(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 25, 20, "Unlimited cars");
	}
	else {
		sprintf(text, "%d cars", player_.hp);
		DrawText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 25, 20, text);
	}

	// draw paused message
	if (isLoadFileMenuOpen_) {
		DrawSavedGames();
	}
	else if (isPaused_) {
		if (player_.game_over) {
			DrawText(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 25, 50, "GAME OVER");
		}
		else {
			DrawText(SCREEN_WIDTH / 2 - 275, SCREEN_HEIGHT / 2 - 25, 50, "GAME PAUSED");
		}
	}
	// draw controls
	DrawControls();

	// render everything to the screen
	SDL_RenderPresent(renderer_);
}

void Game::DrawSavedGames()
{
	// open file with info about saved games
	FILE* file = fopen(SAVED_GAMES_FOLDER_NAME "savedGamesInfo.txt", "r");
	if (file == nullptr) {
		printf("Could not open savedGamesInfo file");
		return;
	}

	// draw loading menu
	DrawRectangle({ LOAD_GAME_STARTING_X , LOAD_GAME_STARTING_Y,
		LOAD_GAME_WINDOW_WIDTH, LOAD_GAME_WINDOW_HEIGHT }, { 128, 128, 128, 255 });
	DrawText(LOAD_GAME_STARTING_X, LOAD_GAME_STARTING_Y, 30, "Saved games");

	// gap between next game pos to load
	int next_saved_game_pos = 40;

	// create fileName array
	char fileName[MAX_FILE_LENGTH];

	// read every line in the savedGameInfo file
	int i = 1;
	while (fgets(fileName, sizeof(fileName), file) != nullptr) {
		// remove last "\n" character
		if (fileName[strlen(fileName) - 1] == '\n') fileName[strlen(fileName) - 1] = '\0';

		// draw green rectangle as a cursor for user to pick which game to load 
		if (chosen_saved_game_ == i) {
			DrawRectangle({ LOAD_GAME_STARTING_X, LOAD_GAME_STARTING_Y + next_saved_game_pos, 5, 30 },
						  { 0, 255, 0, 255 });
		}
		// draw name of every saved game
		DrawText(LOAD_GAME_STARTING_X + 10, LOAD_GAME_STARTING_Y + next_saved_game_pos, 
			12, fileName + strlen(SAVED_GAMES_FOLDER_NAME));

		// save that saved game fileName to saved_games array so we can use it in LoadGame() function
		strcpy(saved_games[i], fileName);

		next_saved_game_pos += 40;
		i++;
	}
	// restrict user so they cannot set cursor out of bounds
	if (chosen_saved_game_ < 1) chosen_saved_game_ = 1;
	if (chosen_saved_game_ >= i) chosen_saved_game_ = i - 1;

	fclose(file);
}

void Game::DrawControls()
{
	DrawText(SCREEN_WIDTH - 180, SCREEN_HEIGHT - 110, 10, "ARROWS = movement");
	DrawText(SCREEN_WIDTH - 180, SCREEN_HEIGHT - 100, 10, "Space = shoot");
	DrawText(SCREEN_WIDTH - 180, SCREEN_HEIGHT - 90, 10, "ESC = quit");
	DrawText(SCREEN_WIDTH - 180, SCREEN_HEIGHT - 80, 10, "N = new game");
	DrawText(SCREEN_WIDTH - 180, SCREEN_HEIGHT - 70, 10, "S = save game");
	DrawText(SCREEN_WIDTH - 180, SCREEN_HEIGHT - 60, 10, "L = load game");
	DrawText(SCREEN_WIDTH - 180, SCREEN_HEIGHT - 50, 10, "P = pause game");
}

void Game::DrawText(int x, int y, int text_size, const char* text)
{
	SDL_Rect srcRectCharset = { 0, 0, 8, 8 };
	SDL_Rect dstRectCharset = { x, y, text_size, text_size };

	for (int i = 0; i < strlen(text); i++) {
		srcRectCharset.x = (text[i] % 16) * 8;
		srcRectCharset.y = (text[i] / 16) * 8;

		dstRectCharset.x = x;
		dstRectCharset.y = y;

		SDL_RenderCopy(renderer_, charset_texture_, &srcRectCharset, &dstRectCharset);

		x += text_size;
	}
}

void Game::DrawRectangle(SDL_Rect dstRect, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer_, &dstRect);
}

void Game::DrawRoad(int road_pos, int road_width)
{
	SDL_Color green = { 0, 57, 0, 255 };
	SDL_Color black = { 0, 0, 0, 255 };

	// green background
	DrawRectangle({ 0, road_pos, SCREEN_WIDTH, SCREEN_HEIGHT }, green);
	
	// road
	DrawRectangle({ (SCREEN_WIDTH - road_width) / 2, road_pos, road_width, SCREEN_HEIGHT }, black);
}

void Game::DrawMenu()
{
	SDL_Color red = { 161, 14, 0, 255 };
	SDL_Color yellow = { 199, 145, 55, 255 };

	// upper top yellow bar
	DrawRectangle({ 0, 0, SCREEN_WIDTH, 5 }, yellow);

	// top red bar
	DrawRectangle({ 0, 5, SCREEN_WIDTH, 55 }, red);

	// lower top yellow bar
	DrawRectangle({ 0, 60, SCREEN_WIDTH, 12 }, yellow);

	// bottom red bar
	DrawRectangle({ 0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 30 }, red);

	// bottom yellow bar
	DrawRectangle({ 0, SCREEN_HEIGHT - 35, SCREEN_WIDTH, 5 }, yellow);
}
