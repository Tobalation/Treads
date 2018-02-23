#include "MenuItem.hpp"

MenuItem::MenuItem(Animation animation, std::string obj_name, sf::Text textObj, std::string lbl_txt, float xpos, float ypos)
	: GameObject(obj_name,30.0f), animation(animation), label(textObj), labelText(lbl_txt)
{
	label.setString(labelText);
	label.setOrigin(label.getLocalBounds().width / 2, label.getLocalBounds().height / 2);
	x = xpos;
	y = ypos;
	vx = vy = angle = 0.0f;
}

void MenuItem::Update(float deltaTime)
{
	animation.Update(deltaTime);
}

void MenuItem::Render(sf::RenderWindow & window, bool DEBUG)
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
	label.setPosition(sf::Vector2f(x, y + 20));
	window.draw(label);
}
