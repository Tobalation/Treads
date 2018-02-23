#include "EnemyKamikaze.hpp"
#include <cmath>

EnemyKamikaze::EnemyKamikaze(Animation animation, float xpos, float ypos, float angle_val, float rad, float spd, std::string type) : GameObject(type,rad), animation(animation), move_speed(spd)
{
	x = xpos;
	y = ypos;
	angle = angle_val;
	damage = 1;

	vx = vy = 0.0f;
}

void EnemyKamikaze::Update(float deltaTime)
{
	if (angle < 0)
		angle = 360 - (-angle);
	else if (angle > 359)
		angle = 0;

	vx = sin(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
	vy = cos(angle * 3.141593f / 180.0f) * move_speed * deltaTime;

	animation.Update(deltaTime);
	translate(vx, vy);
}

void EnemyKamikaze::Render(sf::RenderWindow & window, bool DEBUG)
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
