#include "Player.hpp"
#include <cmath>

Player::Player(Animation animation) : GameObject("player", 15), animation(animation)
{
	mines = 10;
	reloaded = true;
	reload_time = 25;
	move_speed = 125.0f;
	turn_speed = 110.0f;
	angle = 0.0f;
	vx = vy = 0.0f;
}

void Player::Update(float deltaTime)
{
	//Movement Handling
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8) || sf::Keyboard::isKeyPressed(sf::Keyboard::I))
		LTRACK = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5) || sf::Keyboard::isKeyPressed(sf::Keyboard::K))
		LTRACK = -1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9) || sf::Keyboard::isKeyPressed(sf::Keyboard::O))
		RTRACK = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6) || sf::Keyboard::isKeyPressed(sf::Keyboard::L))
		RTRACK = -1;

	//FORWARD
	if (LTRACK == 1 && RTRACK == 1)
	{
		vx = sin(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
		vy = cos(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
	}
	//BACK
	else if (LTRACK == -1 && RTRACK == -1)
	{
		vx = sin(angle * 3.141593f / 180.0f) * -move_speed * deltaTime;
		vy = cos(angle * 3.141593f / 180.0f) * -move_speed * deltaTime;
	}
	//ROTATE LEFT
	else if (LTRACK == -1 && RTRACK == 1)
	{
		angle -= turn_speed * deltaTime;
	}
	//ROTATE RIGHT
	else if (LTRACK == 1 && RTRACK == -1)
	{
		angle += turn_speed * deltaTime;
	}
	//RIGHT FORWARD
	else if (LTRACK == 1 && RTRACK == 0)
	{
		angle += turn_speed * deltaTime / 2;
		vx = sin(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
		vy = cos(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
	}
	//LEFT FORWARD
	else if (LTRACK == 0 && RTRACK == 1)
	{
		angle -= turn_speed * deltaTime / 2;
		vx = sin(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
		vy = cos(angle * 3.141593f / 180.0f) * move_speed * deltaTime;
	}
	//LEFT BACK
	else if (LTRACK == -1 && RTRACK == 0)
	{
		angle -= turn_speed  * deltaTime / 2;
		vx = sin(angle * 3.141593f / 180.0f) * -move_speed * deltaTime;
		vy = cos(angle * 3.141593f / 180.0f) * -move_speed * deltaTime;
	}
	//RIGHT BACK
	else if (LTRACK == 0 && RTRACK == -1)
	{
		angle += turn_speed * deltaTime / 2;
		vx = sin(angle * 3.141593f / 180.0f) * -move_speed * deltaTime;
		vy = cos(angle * 3.141593f / 180.0f) * -move_speed * deltaTime;
	}

	translate(vx, vy);

	animation.Update(deltaTime);

	//Reload Update
	if (!reloaded && reload_time > 0)
		reload_time -= 1 * deltaTime * 100;
	if (reload_time < 0)
	{
		reloaded = true;
	}
		

	if (angle < 0)
		angle = 360 - (-angle);
	else if (angle > 359)
		angle = 0;

	LTRACK = 0;
	RTRACK = 0;
	vx = vy = 0;
}

void Player::Render(sf::RenderWindow &window, bool DEBUG)
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

void Player::fire()
{
	if (reloaded)
	{
		reloaded = false;
		reload_time = 25;
	}
}