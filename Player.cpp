#include "Player.h"

Player::Player()
	: bullets(new Bullet[MAX_BULLETS]),
	isDead(false), game_over(false), unlimited_hp(true), dead_timer_(0)
{
	rect = { PLAYER_STARTING_X, PLAYER_STARTING_Y, CAR_WIDTH, CAR_HEIGHT };
	speed = { PLAYER_HORIZONTAL_SPEED, PLAYER_VERTICAL_SPEED };

	hp = 3;
}

Player::~Player()
{
	delete[] bullets;
}

void Player::Update()
{
	if (isDead && SDL_GetTicks() - dead_timer_ > 2000) {
		isDead = false;
		speed = { PLAYER_HORIZONTAL_SPEED, PLAYER_VERTICAL_SPEED };
	}

	if (IsOut()) {
		Die();
	}

	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!bullets[i].isUsed) {
			bullets[i].rect.x = rect.x + CAR_WIDTH / 2;
			if (bullets[i].rect.x < 0) {
				bullets[i].isUsed = false;
				bullets[i].rect.y = PLAYER_STARTING_Y;
			}
		}
		else {
			bullets[i].rect.y -= BULLET_SPEED;
		}
	}
}

void Player::Move()
{
	if (isDead) { 
		return;
	}

	auto keyStates = SDL_GetKeyboardState(NULL);
	if (keyStates[SDL_SCANCODE_UP]) {
		speed.vertical = 8;
	}
	else if (keyStates[SDL_SCANCODE_DOWN]) {
		speed.vertical = 2;
	}
	else {
		speed.vertical = PLAYER_VERTICAL_SPEED;
	}

	if (keyStates[SDL_SCANCODE_RIGHT]) {
		rect.x += speed.horizontal;
	}
	if (keyStates[SDL_SCANCODE_LEFT]) {
		rect.x -= speed.horizontal;
	}
}

void Player::Shoot()
{
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!bullets[i].isUsed) {
			bullets[i].isUsed = true;
			break;
		}
	}
}

void Player::SetInfo(int road_width)
{
	road_width_ = road_width;
}

void Player::Die()
{
	speed = { 0, 0 };
	isDead = true;
	dead_timer_ = SDL_GetTicks();

	if (!unlimited_hp) {
		hp--;
		if (!hp) {
			game_over = true;
		}
	}
}
