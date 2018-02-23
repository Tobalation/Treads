#include "Animation.hpp"

Animation::Animation(sf::Texture &tex, int x, int y, int w, int h, int count, float Speed) : texture(tex), x(0.0f), y(0.0f), angle(0.0f)
{
	Frame = 0;
	speed = Speed;

	for (int i = 0; i<count; i++)
		frames.push_back(sf::IntRect(x + i*w, y, w, h));

	texture.setSmooth(true);
	sprite.setTexture(texture);
	sprite.setOrigin(w / 2, h / 2);
	sprite.setTextureRect(frames[0]);
}

void Animation::Update(float deltaTime)
{
	Frame += speed * deltaTime;
	int n = frames.size();
	if (Frame >= n) Frame -= n;
	if (n>0) sprite.setTextureRect(frames[int(Frame)]);
}

void Animation::drawSprite(sf::RenderWindow &window)
{
	sprite.setPosition(sf::Vector2f(x, y));
	sprite.setRotation(angle);
	window.draw(sprite);
}

bool Animation::isEnded(float deltaTime)
{
	return Frame + speed * deltaTime >= frames.size();
}