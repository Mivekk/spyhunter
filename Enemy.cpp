#include "Enemy.h"

Enemy::Enemy()
	: player_(nullptr)
{
}

Enemy::Enemy(Player* player)
	: player_(player)
{
}

Enemy::~Enemy()
{
}


void Enemy::Spawn()
{
	isUsed = true;
	hp = 5;
	int road_left_border = (SCREEN_WIDTH - road_width_) / 2;

	// place enemy on random x
	rect.x = rand() % road_width_ + road_left_border;
	rect.y = CAR_STARTING_Y;

	// give it a random speed
	int min_speed = 3;

	speed.vertical = rand() % (PLAYER_VERTICAL_SPEED - min_speed) + min_speed;
}

void Enemy::Update()
{
	for (int i = 0; i < MAX_BULLETS; i++) {
		Bullet* bullet = &player_->bullets[i];
		// check collision with bullet
		if (bullet->isUsed &&
			bullet->rect.y < rect.y + rect.h &&
			bullet->rect.y > rect.y &&
			bullet->rect.x + bullet->rect.w > rect.x &&
			bullet->rect.x < rect.x + rect.w) {

			hp--;
			bullet->isUsed = false;
			bullet->rect.y = PLAYER_STARTING_Y;

			// if hp is under 1 car is dead
			if (hp < 1) {
				Die();

				*bonus_points_ += 500;
			}
		}
	}
	// move car
	Move();
	// die if car is out
	if (IsOut()) {
		Die();

		*bonus_points_ += 500;
	}
}

void Enemy::Move()
{
	if (!isUsed) {
		return;
	}
	// determine enemy speed
	rect.y -= speed.vertical - player_->speed.vertical;

	int road_left_border = (SCREEN_WIDTH - road_width_) / 2;
	int road_right_border = (SCREEN_WIDTH + road_width_) / 2;

	// if player is in 100 radius of enemy. enemy chases it
	if (player_->rect.y - rect.y < 100) {
		if (rect.x < player_->rect.x) {
			rect.x += speed.horizontal;
		}
		else if (rect.x > player_->rect.x) {
			rect.x -= speed.horizontal;
		}
	}

	// turn if on the grass
	if (rect.x < road_left_border) {
		rect.x += speed.horizontal;
	}
	else if (rect.x + rect.w > road_right_border) {
		rect.x -= speed.horizontal;
	}
}

void Enemy::Die()
{
	isUsed = false;
	(*number_of_enemies_)--;

	rect.x = CAR_STARTING_X;
	rect.y = CAR_STARTING_Y;
}

void Enemy::SetInfo(int* number_of_enemies, int* bonus_points, int road_width)
{
	road_width_ = road_width;
	number_of_enemies_ = number_of_enemies;
	bonus_points_ = bonus_points;
}
