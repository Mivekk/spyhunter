#pragma once
#include "Car.h"
#include "Player.h"

class Enemy : public Car
{
public:
	Enemy();
	Enemy(Player* player);
	~Enemy();

	void Spawn() override;
	void Update() override;
	void Move() override;
	void Die() override;

	void SetInfo(int* number_of_enemies, int* bonus_points, int road_width);

private:
	Player* player_;
};