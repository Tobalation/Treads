#include "GameObject.hpp"

GameObject::GameObject(std::string Objname, float R) : name(Objname), radius(R)
{
	active = true;
}

void GameObject::Render(sf::RenderWindow &window, bool DEBUG)
{
}