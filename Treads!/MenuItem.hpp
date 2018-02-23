#pragma once
#include "GameObject.hpp"
#include "Animation.hpp"

class MenuItem : public GameObject
{
public:
	MenuItem(Animation animation, std::string obj_name, sf::Text textObj, std::string lbl, float xpos, float ypos);
	void Update(float deltaTime);
	void Render(sf::RenderWindow &window, bool DEBUG);

private:
	Animation animation;
	sf::Text label;
	std::string labelText;
};