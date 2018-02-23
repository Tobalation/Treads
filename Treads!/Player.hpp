#pragma once
#include "GameObject.hpp"
#include "Animation.hpp"
#include "Bullet.hpp"

class Player : public GameObject
{
public:

	Player(Animation animation);

	void Update(float deltaTime);
	void Render(sf::RenderWindow &window, bool DEBUG);
	void fire();
	void dropMine() { if(mines>0)mines--; }
	void addMines(int amt) { mines += amt; }

	int getNumMines() { return mines; }
	bool isReloaded() { return reloaded; }

private:
	Animation animation;

	bool reloaded;
	int mines, LTRACK, RTRACK;
	float move_speed, turn_speed, reload_time;
};