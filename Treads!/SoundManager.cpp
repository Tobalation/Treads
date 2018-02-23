#include "SoundManager.hpp"

SoundManager::SoundManager(sf::SoundBuffer& sExpSndBuf,
	sf::SoundBuffer& bExpSndBuf,
	sf::SoundBuffer& dExSndBuf,
	sf::SoundBuffer& fSndBuf,
	sf::SoundBuffer& engSndBuf,
	sf::SoundBuffer& engidleSndBuf,
	sf::SoundBuffer& spwnSndBuf,
	sf::SoundBuffer& mineSndBuf,
	sf::SoundBuffer& stSndBuf) : smallExplosionSndBuf(sExpSndBuf), bigExplosionSndBuf(bExpSndBuf), deepExplosionSndBuf(dExSndBuf),
	fireSndBuf(fSndBuf), engineSndBuf(engSndBuf), engineIdleSndBuf(engidleSndBuf), spawnSndBuf(spwnSndBuf), mineSndBuf(mineSndBuf), startSndBuf(stSndBuf)
{
	SOUND = true;
	smallExplosion.setBuffer(smallExplosionSndBuf);
	bigExplosion.setBuffer(bigExplosionSndBuf);
	deepExplosion.setBuffer(deepExplosionSndBuf);
	fire.setBuffer(fireSndBuf);
	engineLoop.setBuffer(engineSndBuf);
	engineIdle.setBuffer(engineIdleSndBuf);
	spawn.setBuffer(spawnSndBuf);
	mineDeploy.setBuffer(mineSndBuf);
	start.setBuffer(startSndBuf);

	engineLoop.setLoop(true);
	engineIdle.setLoop(true);

	//Volume

	fire.setVolume(80);
	engineLoop.setVolume(50);
	engineIdle.setVolume(80);
}

void SoundManager::soundOn()
{
	SOUND = true;
	smallExplosion.setVolume(100);
	bigExplosion.setVolume(100);
	deepExplosion.setVolume(100);
	fire.setVolume(80);
	engineLoop.setVolume(50);
	engineIdle.setVolume(80);
	spawn.setVolume(100);
	mineDeploy.setVolume(100);
	start.setVolume(100);
}

void SoundManager::soundOff()
{
	SOUND = false;
	smallExplosion.setVolume(0);
	bigExplosion.setVolume(0);
	deepExplosion.setVolume(0);
	fire.setVolume(0);
	engineLoop.setVolume(0);
	engineIdle.setVolume(0);
	spawn.setVolume(0);
	mineDeploy.setVolume(0);
	start.setVolume(0);
}

