#include "Civil.h"

Civil::Civil()
	: player_(nullptr)
{
}

Civil::Civil(Player* player)
	: player_(player)
{
}

Civil::~Civil()
{
}

void Civil::Spawn()
{
	isUsed = true;
	hp = 2;
	int road_left_border = (SCREEN_WIDTH - road_width_) / 2;

	// place enemy on random x
	rect.x = rand() % road_width_ + road_left_border;
	rect.y = CAR_STARTING_Y;

	// give it a random speed
	int min_speed = 3;

	speed.vertical = rand() % (PLAYER_VERTICAL_SPEED - min_speed) + min_speed;
}

void Civil::Update()
{
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (player_->bullets[i].isUsed &&
			player_->bullets[i].rect.y < rect.y + rect.h &&
			player_->bullets[i].rect.y > rect.y &&
			player_->bullets[i].rect.x + player_->bullets[i].rect.w > rect.x &&
			player_->bullets[i].rect.x < rect.x + rect.w) {

			hp--;
			player_->bullets[i].isUsed = false;
			player_->bullets[i].rect.y = PLAYER_STARTING_Y;

			if (hp < 1) {
				Die();

				*bonus_points_ -= 500;
			}
		}
	}
	// move car
	Move();
	// die if car is out
	if (IsOut()) {
		Die();

		*bonus_points_ -= 500;
	}
}

void Civil::Move()
{
	if (!isUsed) {
		return;
	}
	// determine civil speed
	rect.y -= speed.vertical - player_->speed.vertical;

	int road_left_border = (SCREEN_WIDTH - road_width_) / 2;
	int road_right_border = (SCREEN_WIDTH + road_width_) / 2;

	// turn if on the grass
	if (rect.x < road_left_border) {
		rect.x += speed.horizontal;
	}
	else if (rect.x + rect.w > road_right_border) {
		rect.x -= speed.horizontal;
	}
}

void Civil::Die()
{
	isUsed = false;
	(*number_of_civils_)--;

	rect.x = CAR_STARTING_X;
	rect.y = CAR_STARTING_Y;
}

void Civil::SetInfo(int* number_of_civils, int* bonus_points, int road_width)
{
	number_of_civils_ = number_of_civils;
	bonus_points_ = bonus_points;
	road_width_ = road_width;
}
