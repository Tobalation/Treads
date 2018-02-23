#include "Mine.hpp"

Mine::Mine(Animation ani, float xpos, float ypos, float angle_val, int damage_val) : GameObject("bullet",16), animation(ani)
{
	x = xpos;
	y = ypos;
	angle = angle_val;
	damage = damage_val;

	vx = vy = 0.0f;
}

void Mine::Update(float deltaTime)
{
	animation.Update(deltaTime);
}

void Mine::Render(sf::RenderWindow & window, bool DEBUG)
{
	animation.setPostion(sf::Vector2f(x, y));
	animation.setRotation(angle);
	animation.drawSprite(window);
	if (DEBUG)
	{
		sf::CircleShape circle(radius);
		circle.setFillColor(sf::Color(255, 0, 0, 100));
		circle.setPosition(x, y);
		circle.setOrigin(radius, radius);
		window.draw(circle);
	}
}
