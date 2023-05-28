#pragma once
#include "Car.h"
#include "Player.h"

class Civil : public Car
{
public:
	Civil();
	Civil(Player* player);
	~Civil();

	void Spawn() override;
	void Update() override;
	void Move() override;
	void Die() override;

	void SetInfo(int* number_of_civils, int* bonus_points, int road_width);

private:
	Player* player_;
};