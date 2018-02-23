#pragma once
#include <SFML\Graphics.hpp>
#include <vector>

class Animation
{
public:
	Animation(sf::Texture &tex, int x, int y, int w, int h, int count, float Speed);

	void Update(float deltaTime);
	void drawSprite(sf::RenderWindow &window);

	void setRotation(float angle_val) { angle = angle_val; }
	void setPostion(sf::Vector2f vec) { x = vec.x, y = vec.y; }
	void setSpeed(float val) { speed = val; }

	int currentFrame() { return (int)Frame; }
	bool isEnded(float deltaTime);

private:
	float x, y, angle, Frame, speed;
	sf::Texture &texture;
	sf::Sprite sprite;
	std::vector<sf::IntRect> frames;
};