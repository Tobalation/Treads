#include "Explosion.hpp"

Explosion::Explosion(Animation exp_animation, float xpos, float ypos) : GameObject("explosion",1), animation(exp_animation)
{
	x = xpos;
	y = ypos;
	angle = 0.0f;
}

void Explosion::Update(float deltaTime)
{
	animation.Update(deltaTime);
	if (animation.isEnded(deltaTime))
		active = false;
}

void Explosion::Render(sf::RenderWindow & window, bool DEBUG)
{
	animation.setPostion(sf::Vector2f(x, y));
	animation.setRotation(angle);
	animation.drawSprite(window);
}
