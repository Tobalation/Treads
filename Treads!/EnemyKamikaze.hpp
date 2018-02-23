#pragma once
#include "GameObject.hpp"
#include "Animation.hpp"

class EnemyKamikaze : public GameObject
{
public:
	EnemyKamikaze(Animation animation, float xpos = 0.0f, float ypos = 0.0f, float angle_val = 0.0f, float rad = 20, float spd = 80.0f, std::string type = "kamikaze");

	void Update(float deltaTime);
	void Render(sf::RenderWindow &window, bool DEBUG);

private:
	Animation animation;

	float move_speed;
	int damage;
};