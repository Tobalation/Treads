#include "FloatingText.hpp"

FloatingText::FloatingText(sf::Text textObj, std::string txt, float xpos, float ypos, float spd, sf::Uint8 life) : GameObject("scoretext",1),text(textObj)
		, string(txt), speed(spd), lifeTime(life)
{
	text.setString(string);
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	x = xpos;
	y = ypos;
	vx = vy = angle = 0.0f;
	alpha = 255;
}

void FloatingText::Update(float deltaTime)
{
	if (alpha == 1)
		active = false;

	alpha -= lifeTime;

	vy = speed * deltaTime;
	translate(vx, vy);
}

void FloatingText::Render(sf::RenderWindow & window, bool DEBUG)
{
	text.setColor({ 255,255,255,alpha});
	text.setPosition(sf::Vector2f(x, y));
	window.draw(text);
}
