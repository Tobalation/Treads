#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <vector>
#include <string>

class GameObject
{
public:
	GameObject(std::string Objname, float R);

	virtual void Update(float deltaTime) {}
	virtual void Render(sf::RenderWindow &window, bool DEBUG);

	float getX() { return x; }
	float getY() { return y; }
	float getVX() { return vx; }
	float getVY() { return vy; }
	float getRadius() { return radius; }
	float getAngle() { return angle; }
	std::string getName() { return name; }
	bool isActive() { return active; }

	void setX(float xval) { x = xval; }
	void setY(float yval) { y = yval; }
	void setVX(float vxval) { vx = vxval; }
	void setVY(float vyval) { vy = vyval; }

	void translate(float vel_x, float vel_y)
	{
		x += vel_x;
		y -= vel_y;
	}
	void setAngle(float aval) { angle = aval; }
	void setRadius(float rval) { radius = rval; }
	void deactivate() { active = false; }

protected:

	bool active;

	float x, y, vx, vy, radius, angle;

	std::string name;
};