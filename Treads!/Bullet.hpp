#pragma once
#include "GameObject.hpp"

class Bullet : public GameObject
{
public:
	Bullet(sf::Texture &texture, float xpos = 0.0f, float ypos = 0.0f, float angle_val = 0.0f, int damage_val = 1);
	void Update(float deltaTime);
	void Render(sf::RenderWindow &window, bool DEBUG);

private:
	sf::Texture &textureImage;
	sf::Sprite sprite;
	float move_speed;
	int damage;
};