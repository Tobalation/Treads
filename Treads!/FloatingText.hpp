#pragma once
#include "GameObject.hpp"

class FloatingText : public GameObject
{
public:
	FloatingText(sf::Text textObj, std::string txt, float xpos, float ypos, float spd, sf::Uint8 life);
	
	void Update(float deltaTime);
	void Render(sf::RenderWindow &window, bool DEBUG);

private:
	sf::Text text;
	sf::Uint8 alpha;
	std::string string;
	float speed, lifeTime;
};
