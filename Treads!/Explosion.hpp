#pragma once
#include "GameObject.hpp"
#include "Animation.hpp"

class Explosion : public GameObject
{
public:
	Explosion(Animation exp_animation, float xpos, float ypos);
	void Update(float deltaTime);
	void Render(sf::RenderWindow &window, bool DEBUG);

private:
	Animation animation;
};