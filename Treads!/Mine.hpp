#pragma once
#include "GameObject.hpp"
#include "Animation.hpp"

class Mine : public GameObject
{
public:
	Mine(Animation ani, float xpos = 0.0f, float ypos = 0.0f, float angle = 0.0f, int damage_val = 1);
	void Update(float deltaTime);
	void Render(sf::RenderWindow &window, bool DEBUG);

private:
	Animation animation;
	int damage;
};