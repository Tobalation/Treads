#pragma once
#include <SFML\Audio.hpp>
#include <SFML\Graphics.hpp>

class SoundManager
{
public:
	SoundManager(sf::SoundBuffer& sExpSndBuf,
		sf::SoundBuffer& bExpSndBuf,
		sf::SoundBuffer& dExSndBuf,
		sf::SoundBuffer& fSndBuf,
		sf::SoundBuffer& engSndBuf,
		sf::SoundBuffer& engidleSndBuf,
		sf::SoundBuffer& spwnSndBuf,
		sf::SoundBuffer& mineSndBuf,
		sf::SoundBuffer& stSndBuf);

	void soundOn();
	void soundOff();
	void playSExplosion() { smallExplosion.play(); }
	void playBExplosion() { bigExplosion.play(); }
	void playDExplosion() { deepExplosion.play(); }
	void playFire() { fire.play(); }
	void playEngineLoop()
	{
		if (engineLoop.getStatus() != sf::Sound::Playing && (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8) || sf::Keyboard::isKeyPressed(sf::Keyboard::I)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5) || sf::Keyboard::isKeyPressed(sf::Keyboard::K)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9) || sf::Keyboard::isKeyPressed(sf::Keyboard::O)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6) || sf::Keyboard::isKeyPressed(sf::Keyboard::L)))
		{
			engineIdle.setVolume(50);
			engineLoop.play();
		}
		else if(!(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8) || sf::Keyboard::isKeyPressed(sf::Keyboard::I)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5) || sf::Keyboard::isKeyPressed(sf::Keyboard::K)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9) || sf::Keyboard::isKeyPressed(sf::Keyboard::O)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6) || sf::Keyboard::isKeyPressed(sf::Keyboard::L)))
		{
			engineIdle.setVolume(80);
			engineLoop.pause();
		}

		if (!SOUND)
			engineIdle.setVolume(0);

		if (engineIdle.getStatus() != sf::Sound::Playing)
		{
			engineIdle.play();
		}
	}
	void playSpawn() { spawn.play(); }
	void playMineDeploy() { mineDeploy.play(); }
	void playStart() { start.play(); }

private:
	bool SOUND;
	sf::SoundBuffer& smallExplosionSndBuf;
	sf::SoundBuffer& bigExplosionSndBuf;
	sf::SoundBuffer& deepExplosionSndBuf;
	sf::SoundBuffer& fireSndBuf;
	sf::SoundBuffer& engineSndBuf;
	sf::SoundBuffer& engineIdleSndBuf;
	sf::SoundBuffer& spawnSndBuf;
	sf::SoundBuffer& mineSndBuf;
	sf::SoundBuffer& startSndBuf;

	sf::Sound smallExplosion;
	sf::Sound bigExplosion;
	sf::Sound deepExplosion;
	sf::Sound fire;
	sf::Sound engineLoop;
	sf::Sound engineIdle;
	sf::Sound spawn;
	sf::Sound mineDeploy;
	sf::Sound start;
};
