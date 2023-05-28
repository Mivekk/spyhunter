#pragma once
#include "Car.h"

struct Bullet {
	SDL_Rect rect = { PLAYER_STARTING_X, PLAYER_STARTING_Y, BULLET_WIDTH, BULLET_HEIGHT };
	bool isUsed = false;
	int speed = 5;
};

class Player : public Car
{
public:
	Player();
	~Player();
	
	void Update() override;
	void Move() override;
	void Die() override;

	void Shoot();

	void SetInfo(int road_width);

public:
	Bullet* bullets;

	bool isDead;
	bool game_over;
	bool unlimited_hp;

private:
	int dead_timer_;
};
