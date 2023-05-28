#include "Car.h"

Car::Car()
	: surface(nullptr), texture(nullptr), rect({ CAR_STARTING_X, CAR_STARTING_Y, CAR_WIDTH, CAR_HEIGHT }), 
	speed({ CAR_HORIZONTAL_SPEED, CAR_VERTICAL_SPEED }), isUsed(false), hp(0), 
	number_of_enemies_(0), number_of_civils_(0), bonus_points_(nullptr), road_width_(0)
{
}

Car::~Car()
{
}


bool Car::IsOut()
{
	// check if player is out of the road
	int car_left_border = rect.x;
	int car_right_border = rect.x + rect.w;

	// is out of bounds
	if (car_left_border < (SCREEN_WIDTH - road_width_) / 2 - 2 * CAR_WIDTH && 
		car_left_border > -CAR_WIDTH) {

		rect.x = PLAYER_STARTING_X;
		return true;
	}
	else if (car_right_border > (SCREEN_WIDTH + road_width_) / 2 + 2 * CAR_WIDTH && 
			 car_right_border < SCREEN_WIDTH + CAR_WIDTH) {

		rect.x = PLAYER_STARTING_X;
		return true;
	}

	return false;
}

void Car::HandleCollision(Car* car, int points)
{
	int car_left_border = car->rect.x;
	int car_right_border = car->rect.x + car->rect.w;
	int car_top_border = car->rect.y;
	int car_bot_border = car->rect.y + car->rect.h;

	int left_border = rect.x;
	int right_border = rect.x + rect.w;
	int top_border = rect.y;
	int bot_border = rect.y + rect.h;

	int margin = 5;
	// touches in x-axis
	if (car_right_border - margin > left_border &&
		car_left_border + margin < right_border) {

		// car is on top of the enemy
		if (car_bot_border < top_border + margin &&
			car_bot_border > top_border - margin) {
			
			//rect.y = car_bot_border + margin;
			Die();
			*bonus_points_ += points;
		}
		// car is under the enemy
		else if (car_top_border + margin > bot_border &&
				 car_top_border - margin < bot_border) {

			//rect.y = car_top_border - rect.h - margin;
			car->Die();
		}
	}
	// touches in y-axis
	if (car_top_border - margin < bot_border &&
		car_bot_border + margin > top_border) {

		// left side
		if (car_right_border - margin > left_border &&
			car_right_border + margin < left_border + rect.w / 2) {
			rect.x = car_right_border;
		}
		// right side
		else if (car_left_border + margin < right_border &&
				 car_left_border - margin > left_border + rect.w / 2) {
			rect.x = car_left_border - rect.w;
		}
	}
}

