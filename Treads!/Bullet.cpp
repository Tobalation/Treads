#include "Bullet.hpp"
#include <cmath>

Bullet::Bullet(sf::Texture &texture, float xpos, float ypos, float angle_val, int damage_val) : GameObject("bullet",4), textureImage(texture)
{
	textureImage.setSmooth(true);
	sprite.setTexture(textureImage);
	sprite.setOrigin(textureImage.getSize().x / 2, textureImage.getSize().y / 2);

	x = xpos;
	y = ypos;
	angle = angle_val;
	damage = damage_val;
	move_speed = 1000.0f;

	vx = sin(angle * 3.141593f / 180.0f) * move_speed;
	vy = cos(angle * 3.141593f / 180.0f) * move_speed;
}

void Bullet::Update(float deltaTime)
{
	vx = sin(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
	vy = cos(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
	translate(vx, vy);
}

void Bullet::Render(sf::RenderWindow &window, bool DEBUG)
{
	sprite.setPosition(sf::Vector2f(x, y));
	sprite.setRotation(angle);
	window.draw(sprite);
	if (DEBUG)
	{
		sf::CircleShape circle(radius);
		circle.setFillColor(sf::Color(255, 0, 0, 100));
		circle.setPosition(x, y);
		circle.setOrigin(radius, radius);
		window.draw(circle);
	}
}