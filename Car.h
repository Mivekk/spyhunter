#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"

extern "C" {
#include "./SDL2-2.0.10/include/SDL.h"
#include "./SDL2-2.0.10/include/SDL_main.h"
}

struct Speed {
	int horizontal;
	int vertical;
};

class Car
{
public:
	Car();
	virtual ~Car();

	bool IsOut();
	void HandleCollision(Car* player, int points);

	inline virtual void Spawn() {};
	inline virtual void Update() {};
	inline virtual void Move() {};
	inline virtual void Die() {};

public:
	SDL_Surface* surface;
	SDL_Texture* texture;

	SDL_Rect rect;
	Speed speed;

	bool isUsed;
	int hp;

protected:
	int* number_of_enemies_;
	int* number_of_civils_;
	int* bonus_points_;

	int road_width_;
};