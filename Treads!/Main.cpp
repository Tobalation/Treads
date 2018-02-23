#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\Window.hpp>
#include <SFML\Audio.hpp>
#include <Windows.h>
#include <Dwmapi.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

#include "Player.hpp"
#include "Bullet.hpp"
#include "Mine.hpp"
#include "EnemyKamikaze.hpp"

#include "FloatingText.hpp"
#include "MenuItem.hpp"
#include "Animation.hpp"
#include "Explosion.hpp"
#include "ScoreRecord.hpp"
#include "SoundManager.hpp"

#pragma comment (lib, "Dwmapi.lib")

bool GAME_RUNNING = true;
bool SOUND = true;
bool VSYNC = false;

bool DEBUG_MODE = false;
bool PAUSED = false;
bool GAME_OVER = false;
bool GAME_STARTED = false;
bool ENTERING_SCORE = false;
bool CHEATS_LASER = false;
bool CHEATS_MULTIFIRE = false;
bool CHEATS_RAPIDFIRE = false;

const int FRAME_LIMIT = 90;
const int KAMIKAZE_VALUE = 100;
const int SPEEDER_VALUE = 250;

int gameScore = 0;
int gameScoreBenchmark = 1;
float timeSurvived = 0.0f;

std::vector<ScoreRecord> highScoreTable;

FILE * logFile;

//-------------------Helping functions------------------------//

//Collison detection
bool isColliding(GameObject *a, GameObject *b)
{
	return (b->getX() - a->getX())*(b->getX() - a->getX()) +
		(b->getY() - a->getY())*(b->getY() - a->getY()) <
		(a->getRadius() + b->getRadius())*(a->getRadius() + b->getRadius());
}

//Resource loading with mod support
bool loadTexture(sf::Texture& texture, std::string mod_location, std::string location)
{
	if (!texture.loadFromFile(mod_location))
	{
		if (!texture.loadFromFile(location))
		{
			fprintf(logFile, "Error: %s could not be loaded.",location.c_str());
			return false;
		}
		fprintf(logFile, "%s loaded\n",location.c_str());
		return true;
	}
	else
	{
		fprintf(logFile, "%s loaded\n", mod_location.c_str());
		return true;
	}	
}

bool loadSoundBuffer(sf::SoundBuffer& buffer, std::string mod_location, std::string location)
{
	if (!buffer.loadFromFile(mod_location))
	{
		if (!buffer.loadFromFile(location))
		{
			fprintf(logFile, "Error: %s could not be loaded.", location.c_str());
			return false;
		}
		fprintf(logFile, "%s loaded\n", location.c_str());
		return true;
	}
	else
	{
		fprintf(logFile, "%s loaded\n", mod_location.c_str());
		return true;
	}
		
}

//Highscore loading / saving
void loadHighScores(std::vector<ScoreRecord>& highScores)
{
	ScoreRecord record;
	std::ifstream inFile;
	inFile.open("data/highscores.sav");
	if (inFile.good())
	{
		while (highScores.size() < 10)
		{
			record.read(inFile);
			highScores.push_back(record);
			if (inFile.fail())
				break;
		}
	}
	else
	{
		for (unsigned int i = 0; i < 10; i++)
		{
			highScores.push_back(record);
		}
	}

	inFile.close();
}

void saveHighScores(std::vector<ScoreRecord>& highScores, std::string name, int score, float time)
{
	ScoreRecord newRecord(name,score,time);
	std::ofstream outFile;
	outFile.open("data/highscores.sav");

	for (unsigned int i = 0; i < highScores.size(); i++)
	{
		if (newRecord.getScore() > highScores[i].getScore())
		{
			highScores.insert(highScores.begin() + i, newRecord);
			highScores.pop_back();
			break;
		}
	}

	for (unsigned int i = 0; i < highScores.size(); i++)
	{
		highScores[i].write(outFile);
	}

	outFile.close();
}

//-----------------------------------------------------------//

int main()
{
	srand(time(NULL));

	int spawnChance, spawnLocation;
	
	std::string cheatCodeBuffer;
	std::string highscoreName;
	std::string debug;

	loadHighScores(highScoreTable);

	//Game run status logging
	logFile = fopen("log.txt", "w");

	//----------------------Splash screen-------------------------//

	sf::RenderWindow splashWindow(sf::VideoMode(512,512), "Treads!", sf::Style::None);

	MARGINS margins;
	margins.cxLeftWidth = -1;

	SetWindowLong(splashWindow.getSystemHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
	DwmExtendFrameIntoClientArea(splashWindow.getSystemHandle(), &margins);

	sf::Texture splashLogo;
	if (!splashLogo.loadFromFile("data/res/splash.png"))
		return EXIT_FAILURE;
	sf::Sprite splashLogoSpr;
	splashLogoSpr.setTexture(splashLogo);
	splashLogoSpr.setOrigin(splashLogo.getSize().x / 2, splashLogo.getSize().y / 2);
	splashLogoSpr.setPosition({ (float)splashWindow.getSize().x / 2,(float)splashWindow.getSize().y / 2 });

	splashWindow.clear(sf::Color::Transparent);
	splashWindow.draw(splashLogoSpr);
	splashWindow.display();
	sf::Clock splashTimer;

	while (splashTimer.getElapsedTime().asSeconds() < 2.0f) {}
	splashWindow.close();
	//----------------------------------------------------------//




	//-------------------------------------------------------//
	//-----------------Game Initialization-------------------//
	//-------------------------------------------------------//

	//--------------Window Initialization-------------------//
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	sf::RenderWindow MainWindow(sf::VideoMode::getDesktopMode() , "Treads!", sf::Style::None, settings);
	fprintf(logFile, "\n------window initialized------\n\n");

	MainWindow.setFramerateLimit(FRAME_LIMIT);
	MainWindow.setMouseCursorVisible(false);
	MainWindow.setVerticalSyncEnabled(false);

	sf::Clock gameClock;
	sf::Clock gameTime;
	float deltaTime, lastTime ,fps = 0.0f;
	//-------------------------------------------------------//

	//-----------------Text Initialization-------------------//
	sf::Font UIfont;
	if (!UIfont.loadFromFile("data/font/kenpixel_high_square.ttf"))
	{
		fprintf(logFile, "Error: font file missing.");
		return EXIT_FAILURE;
	}
	fprintf(logFile, "\n------loaded UIfont------\n\n");

	sf::Text loadingText;
	sf::Text Debug_text;
	sf::Text Score_text;
	sf::Text floating_text;
	sf::Text info_text;
	sf::Text menu_text;
	sf::Text scoreTable_text;
	sf::Text playerStat_text;

	loadingText.setFont(UIfont);
	loadingText.setCharacterSize(32);
	loadingText.setPosition({ (float)MainWindow.getSize().x / 2,(float)MainWindow.getSize().y / 2 });

	Debug_text.setFont(UIfont);
	Debug_text.setFillColor(sf::Color::Yellow);
	Debug_text.setOutlineThickness(1);
	Debug_text.setCharacterSize(16);
	Debug_text.setPosition({10,10});

	Score_text.setFont(UIfont);
	Score_text.setOutlineThickness(1);
	Score_text.setCharacterSize(32);
	Score_text.setPosition({(float)MainWindow.getSize().x / 2 ,40 });

	floating_text.setFont(UIfont);
	floating_text.setCharacterSize(20);

	info_text.setFont(UIfont);
	info_text.setCharacterSize(24);
	info_text.setPosition({ (float)MainWindow.getSize().x / 2 - 200,(float)MainWindow.getSize().y / 4});
	std::string infotext;
	infotext = "This is TREADS!...\n" \
		"The goal is to survive for as long as possible\n" \
		"Shoot the option targets to change options , press \"P\" to pause\n" \
		"Destroy the start target to begin!";
	info_text.setString(infotext);

	menu_text.setFont(UIfont);
	menu_text.setOutlineThickness(1);
	menu_text.setCharacterSize(18);

	scoreTable_text.setFont(UIfont);
	scoreTable_text.setOutlineThickness(1);
	scoreTable_text.setColor(sf::Color::Yellow);
	scoreTable_text.setCharacterSize(24);
	scoreTable_text.setPosition({ (float)MainWindow.getSize().x / 2,(float)MainWindow.getSize().y / 2});

	playerStat_text.setFont(UIfont);
	playerStat_text.setCharacterSize(16);

	//-------------------------------------------------------//

	//---------------Texture Initializtion-------------------//
	loadingText.setString("Loading Textures...");
	loadingText.setOrigin(loadingText.getLocalBounds().width / 2, loadingText.getLocalBounds().height / 2);
	MainWindow.clear(sf::Color::Black);
	MainWindow.draw(loadingText);
	MainWindow.display();

	sf::Texture backgroundTex;
	sf::Texture enKamTex;
	sf::Texture enSpeederTex;
	sf::Texture playerTex;
	sf::Texture shotTex;
	sf::Texture mineTex;
	sf::Texture sExplosionTex;
	sf::Texture bExplosionTex;
	sf::Texture logoTex;
	sf::Texture helpTex;
	sf::Texture menuItemTex;

	sf::Sprite logo;
	sf::Sprite help;
	sf::Sprite background;

	//enemyKamikazeTex
	if (!loadTexture(enKamTex,"data/mods/res/Enemy_kamikaze.png", "data/res/enemy_kamikaze.png"))
		return EXIT_FAILURE;

	//enemyShooterTex
	if (!loadTexture(enSpeederTex, "data/mods/res/Enemy_Speeder.png", "data/res/enemy_speeder.png"))
		return EXIT_FAILURE;
	
	//playerTex
	if (!loadTexture(playerTex, "data/mods/res/player.png", "data/res/player.png"))
		return EXIT_FAILURE;
	
	//shotTex
	if (!loadTexture(shotTex, "data/mods/res/shot.png", "data/res/shot.png"))
		return EXIT_FAILURE;

	//mineTex
	if (!loadTexture(mineTex, "data/mods/res/mine.png", "data/res/mine.png"))
		return EXIT_FAILURE;

	//smallExplosionTex
	if (!loadTexture(sExplosionTex, "data/mods/res/small_explosion.png", "data/res/small_explosion.png"))
		return EXIT_FAILURE;

	//bigExplosionTex
	if (!loadTexture(bExplosionTex, "data/mods/res/big_explosion.png", "data/res/big_explosion.png"))
		return EXIT_FAILURE;

	//menuItemTex
	if (!loadTexture(menuItemTex, "data/mods/res/menu_item.png", "data/res/menu_item.png"))
		return EXIT_FAILURE;

	Animation mineAni = Animation(mineTex, 0, 0, 16, 16, 2, 10.0f);
	Animation small_explosionAni = Animation(sExplosionTex, 0, 0, 138, 138, 51, 100.0f);
	Animation big_explosionAni = Animation(bExplosionTex, 0, 0, 266, 266, 52, 20.0f);
	Animation playerAni= Animation(playerTex, 0, 0, 64, 64, 1, 1.0f);
	Animation en_kamikazeAni = Animation(enKamTex, 0, 0, 64, 64, 1, 1.0f);
	Animation en_speederAni = Animation(enSpeederTex, 0, 0, 33, 64, 1, 1.0f);
	Animation menuItemAni = Animation(menuItemTex, 0, 0, 32, 32, 1, 1.0f);

	//backgroundTex
	if (!loadTexture(backgroundTex, "data/mods/res/background.png", "data/res/background.png"))
		return EXIT_FAILURE;
	backgroundTex.setRepeated(true);
	backgroundTex.setSmooth(true);
	background.setTexture(backgroundTex);
	background.setTextureRect({ 0,0,(int)MainWindow.getSize().x, (int)MainWindow.getSize().y });

	//logoTex
	if (!loadTexture(logoTex, "data/mods/res/logo.png", "data/res/logo.png"))
		return EXIT_FAILURE;
	logoTex.setSmooth(true);
	logo.setTexture(logoTex);
	logo.setOrigin(logo.getLocalBounds().width / 2, logo.getLocalBounds().height / 2);
	logo.setPosition({ (float)MainWindow.getSize().x / 2,(float)MainWindow.getSize().y / 6 });
	
	//help texture
	if (!loadTexture(helpTex, "data/mods/res/help.png", "data/res/help.png"))
		return EXIT_FAILURE;
	helpTex.setSmooth(true);
	help.setTexture(helpTex);
	help.setOrigin(help.getLocalBounds().width / 2, help.getLocalBounds().height / 2);
	help.setPosition({ (float)MainWindow.getSize().x / 3,(float)MainWindow.getSize().y / 2 + 50});

	fprintf(logFile, "\n------loaded all textures------\n\n");
	//-------------------------------------------------------//

	//----------------Sound Initialization-------------------//
	loadingText.setString("Loading Sounds...");
	loadingText.setOrigin(loadingText.getLocalBounds().width / 2, loadingText.getLocalBounds().height / 2);
	MainWindow.clear(sf::Color::Black);
	MainWindow.draw(loadingText);
	MainWindow.display();
	
	sf::SoundBuffer fireSndBuf;
	sf::SoundBuffer smallExplosionSndBuf;
	sf::SoundBuffer bigExplosionSndBuf;
	sf::SoundBuffer deepExplosionSndBuf;
	sf::SoundBuffer engineSndBuf;
	sf::SoundBuffer engineIdleSndBuf;
	sf::SoundBuffer spawnSndBuf;
	sf::SoundBuffer mineSndBuf;
	sf::SoundBuffer startSndBuf;


	//fire sound
	if (!loadSoundBuffer(fireSndBuf, "data/mods/audio/fire.wav", "data/audio/fire.wav"))
		return EXIT_FAILURE;

	//small explosion sound
	if (!loadSoundBuffer(smallExplosionSndBuf, "data/mods/audio/small_explosion.wav", "data/audio/small_explosion.wav"))
		return EXIT_FAILURE;

	//big explosion sound
	if (!loadSoundBuffer(bigExplosionSndBuf, "data/mods/audio/big_explosion.wav", "data/audio/big_explosion.wav"))
		return EXIT_FAILURE;

	//Deep explosion sound
	if (!loadSoundBuffer(deepExplosionSndBuf, "data/mods/audio/deep_explosion.wav", "data/audio/deep_explosion.wav"))
		return EXIT_FAILURE;

	//engine sound
	if (!loadSoundBuffer(engineSndBuf, "data/mods/audio/engine_loop.wav", "data/audio/engine_loop.wav"))
		return EXIT_FAILURE;

	//engine idle sound
	if (!loadSoundBuffer(engineIdleSndBuf, "data/mods/audio/engine_idle.wav", "data/audio/engine_idle.wav"))
		return EXIT_FAILURE;

	//spawn sound
	if (!loadSoundBuffer(spawnSndBuf, "data/mods/audio/spawn.wav", "data/audio/spawn.wav"))
		return EXIT_FAILURE;

	//mine deployment sound
	if (!loadSoundBuffer(mineSndBuf, "data/mods/audio/mine_deploy.wav", "data/audio/mine_deploy.wav"))
		return EXIT_FAILURE;

	//start sound
	if (!loadSoundBuffer(startSndBuf, "data/mods/audio/start.wav", "data/audio/start.wav"))
		return EXIT_FAILURE;

	sf::Music backgroundMusic;
	if (!backgroundMusic.openFromFile("data/mods/audio/background/background.wav"))
	{
		if (!backgroundMusic.openFromFile("data/audio/background/background.wav"))
		{
			fprintf(logFile, "Error: data/audio/background/background.wav could not be loaded.");
			return EXIT_FAILURE;
		}
		fprintf(logFile, "data/audio/background/background.wav loaded\n");
	}
	else
	{
		fprintf(logFile, "data/mods/audio/background/background.wav loaded\n");
	}
	backgroundMusic.setLoop(true);
	backgroundMusic.setVolume(60);

	//An object to manage all sound effects that are stored in memory

	SoundManager soundManager(smallExplosionSndBuf,
		bigExplosionSndBuf,
		deepExplosionSndBuf,
		fireSndBuf,
		engineSndBuf,
		engineIdleSndBuf,
		spawnSndBuf,
		mineSndBuf,
		startSndBuf);

	soundManager.soundOn();

	fprintf(logFile, "\n------loaded all sounds------\n\n");
	//-------------------------------------------------------//


	//--------------Object Initialization-------------------//
	loadingText.setString("Loading Objects...");
	loadingText.setOrigin(loadingText.getLocalBounds().width / 2, loadingText.getLocalBounds().height / 2);
	MainWindow.clear(sf::Color::Black);
	MainWindow.draw(loadingText);
	MainWindow.display();
	
	Player* player = new Player(playerAni);
	player->setX((int)MainWindow.getSize().x / 2.f );
	player->setY((int)MainWindow.getSize().y / 2.f);

	MenuItem* start = new MenuItem(menuItemAni, "start", menu_text, "Start  Game", MainWindow.getSize().x / 2 + 140, MainWindow.getSize().y / 2 - 128);
	MenuItem* vsync = new MenuItem(menuItemAni, "vsync", menu_text, "Enable vsync", MainWindow.getSize().x / 2 + 200, MainWindow.getSize().y / 2 );
	MenuItem* sound = new MenuItem(menuItemAni, "sound", menu_text, "Disable  sound", MainWindow.getSize().x / 2 + 260, MainWindow.getSize().y / 2 + 128);
	MenuItem* exit = new MenuItem(menuItemAni, "exit", menu_text, "Exit", MainWindow.getSize().x / 3 - 80, MainWindow.getSize().y / 3 - 80);

	std::vector<GameObject*> gameObjects;

	std::vector<sf::Vector2i> spawnPoints;

	spawnPoints.push_back(sf::Vector2i(30, 30));
	spawnPoints.push_back(sf::Vector2i(MainWindow.getSize().x / 2, 30));
	spawnPoints.push_back(sf::Vector2i(MainWindow.getSize().x - 30, 30));
	spawnPoints.push_back(sf::Vector2i(30, MainWindow.getSize().y / 2));
	spawnPoints.push_back(sf::Vector2i(MainWindow.getSize().x - 30, MainWindow.getSize().y / 2));
	spawnPoints.push_back(sf::Vector2i(30, MainWindow.getSize().y - 30));
	spawnPoints.push_back(sf::Vector2i(MainWindow.getSize().x / 2, MainWindow.getSize().y - 30));
	spawnPoints.push_back(sf::Vector2i(MainWindow.getSize().x - 30, MainWindow.getSize().y - 30));
	
	gameObjects.push_back(player);
	gameObjects.push_back(start);
	gameObjects.push_back(vsync);
	gameObjects.push_back(sound);
	gameObjects.push_back(exit);

	fprintf(logFile, "\nloaded all  objects\n\n");
	//-------------------------------------------------------//

	/*-------------------------------------------------------*/

	//Play background audio
	backgroundMusic.play();
	gameClock.restart();

	//-------------------------------------------------------//
	//------------------Main Game loop-----------------------//
	//-------------------------------------------------------//

	while (MainWindow.isOpen())
	{
		//Calculate FPS and time between each frame
		deltaTime = gameClock.restart().asSeconds();
		fps = 1.f / deltaTime;
		lastTime = deltaTime;

		//Event Handling

		sf::Event event;

		while (MainWindow.pollEvent(event))
		{
			//Window closed
			if (event.type == sf::Event::Closed)
			{
				MainWindow.close();
			}
				
			//Pause if lost window focus
			if (event.type == sf::Event::GainedFocus)
			{
				PAUSED = false;
			}	

			if (event.type == sf::Event::LostFocus)
			{
				PAUSED = true;
			}
				
			//Handle text input
			if (event.type == sf::Event::TextEntered)
			{
				// Handle cheat code entering (ASCII letters only)
				if (event.text.unicode < 123 && event.text.unicode > 96)
				{
					cheatCodeBuffer += static_cast<char>(event.text.unicode);
				}

				//Handle high score name entering (ASCII letters only)
				if (ENTERING_SCORE && event.text.unicode < 123 && event.text.unicode > 96 && highscoreName.length() <= 10)
				{
					highscoreName += static_cast<char>(event.text.unicode);
				}
			}

			//---------Key press events------------//
			if (event.type == sf::Event::KeyPressed)
			{
				//Debug Mode
				if (event.key.code == sf::Keyboard::F1)
				{
					if (DEBUG_MODE)
					{
						DEBUG_MODE = false;
						FloatingText* textEffect = new FloatingText(floating_text, "DEBUG  MODE  DEACTIVATED", player->getX(), player->getY() - 10, 15.0f, 1);
						gameObjects.push_back(textEffect);
					}
						
					else
					{
						DEBUG_MODE = true;
						FloatingText* textEffect = new FloatingText(floating_text, "DEBUG  MODE  ACTIVATED", player->getX(), player->getY() - 10, 15.0f, 1);
						gameObjects.push_back(textEffect);
					}
				}

				//Pause the game
				else if (event.key.code == sf::Keyboard::P && !ENTERING_SCORE)
				{
					if (PAUSED)
						PAUSED = false;
					else
						PAUSED = true;
				}
				
				//Play again
				else if (event.key.code == sf::Keyboard::Y && GAME_OVER && !ENTERING_SCORE)
				{
					gameTime.restart();
					info_text.setPosition({ (float)MainWindow.getSize().x / 2 - 200,(float)MainWindow.getSize().y / 4 });
					gameScore = 0;
					GAME_STARTED = false;
					GAME_OVER = false;
					soundManager.soundOn();

					player = new Player(playerAni);
					player->setX((int)MainWindow.getSize().x / 2.f);
					player->setY((int)MainWindow.getSize().y / 2.f);

					start = new MenuItem(menuItemAni, "start", menu_text, "Start  Game", MainWindow.getSize().x / 2 + 140, MainWindow.getSize().y / 2 - 128);
					vsync = new MenuItem(menuItemAni, "vsync", menu_text, "Enable vsync", MainWindow.getSize().x / 2 + 200, MainWindow.getSize().y / 2);
					sound = new MenuItem(menuItemAni, "sound", menu_text, "Disable  sound", MainWindow.getSize().x / 2 + 260, MainWindow.getSize().y / 2 + 128);
					exit = new MenuItem(menuItemAni, "exit", menu_text, "Exit", MainWindow.getSize().x / 3 - 80, MainWindow.getSize().y / 3 - 80);
					gameObjects.push_back(player);
					gameObjects.push_back(start);
					gameObjects.push_back(vsync);
					gameObjects.push_back(sound);
					gameObjects.push_back(exit);
				}

				//submit high score
				else if (event.key.code == sf::Keyboard::Return && ENTERING_SCORE)
				{
					saveHighScores(highScoreTable, highscoreName, gameScore, timeSurvived);
					ENTERING_SCORE = false;
					GAME_OVER = true;
				}

				else if (event.key.code == sf::Keyboard::BackSpace && ENTERING_SCORE)
				{
					if (highscoreName.length() > 0)
						highscoreName.pop_back();
				}

				//Exit game when over
				else if (event.key.code == sf::Keyboard::N && GAME_OVER)
				{
					MainWindow.close();
				}

				//Fire shells
				else if (event.key.code == sf::Keyboard::Space)
				{
					if (player->isReloaded() && !CHEATS_MULTIFIRE)
					{
						player->fire();
						Bullet* fired_round = new Bullet(shotTex, player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle(), 1);
						gameObjects.push_back(fired_round);

						if (SOUND)
							soundManager.playFire();
					}

					else if (player->isReloaded() && CHEATS_MULTIFIRE)
					{
						player->fire();
						Bullet* roundN = new Bullet(shotTex, player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle(), 1);

						Bullet* roundS = new Bullet(shotTex, player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle() + 180, 1);

						Bullet* roundE = new Bullet(shotTex, player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle() + 90, 1);

						Bullet* roundW = new Bullet(shotTex, player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle() - 90, 1);

						gameObjects.push_back(roundN);
						gameObjects.push_back(roundS);
						gameObjects.push_back(roundE);
						gameObjects.push_back(roundW);

						if (SOUND)
							soundManager.playFire();
					}

					if (CHEATS_RAPIDFIRE)
					{
						Bullet* fired_round = new Bullet(shotTex, player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle(), 1);
						gameObjects.push_back(fired_round);

						if (SOUND)
							soundManager.playFire();
					}
				}

				//Land mine deployment
				else if (event.key.code == sf::Keyboard::LControl)
				{
					if (player->isReloaded() && player->getNumMines() > 0)
					{
						if(!CHEATS_MULTIFIRE)
							player->dropMine();
						player->fire();
						Mine* laid_mine = new Mine(mineAni, player->getX() - sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() + cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle(), 1);
						gameObjects.push_back(laid_mine);

						if (SOUND)
							soundManager.playMineDeploy();
					}

					if (CHEATS_RAPIDFIRE)
					{
						Mine* laid_mine = new Mine(mineAni, player->getX() - sin(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getY() + cos(player->getAngle() * 3.141593f / 180.0f) * player->getRadius()
							, player->getAngle(), 1);
						gameObjects.push_back(laid_mine);

						if (SOUND)
							soundManager.playMineDeploy();
					}
				}

				//Alt + F4 to terminate
				else if (event.key.code == sf::Keyboard::LAlt && event.key.code == sf::Keyboard::F4
					|| event.key.code == sf::Keyboard::F4 && event.key.code == sf::Keyboard::LAlt)
				{
					MainWindow.close();
				}
			}
		}

		//---------------Per frame updates-------------//

		soundManager.playEngineLoop();

		//CHEATS!
		if (cheatCodeBuffer.size() > 64)
			cheatCodeBuffer = "";

		if (cheatCodeBuffer.find("isuckataiming") != std::string::npos && !GAME_OVER)
		{
			CHEATS_LASER = true;
			cheatCodeBuffer = "";
			FloatingText* cheatEffect = new FloatingText(floating_text, "CHEAT ENABLED!", player->getX(), player->getY() - 10, 15.0f, 1);
			gameObjects.push_back(cheatEffect);
			soundManager.playStart();

		}

		else if (cheatCodeBuffer.find("fuckthisgame") != std::string::npos && !GAME_OVER)
		{
			cheatCodeBuffer = "";
			FloatingText* cheatEffect = new FloatingText(floating_text, "THAT'S  RUDE!", player->getX(), player->getY() - 10, 15.0f, 1);
			gameObjects.push_back(cheatEffect);
			soundManager.playStart();
		}

		else if (cheatCodeBuffer.find("iamneo") != std::string::npos && !GAME_OVER)
		{
			CHEATS_RAPIDFIRE = false;
			CHEATS_MULTIFIRE = true;
			cheatCodeBuffer = "";
			FloatingText* cheatEffect = new FloatingText(floating_text, "CHEAT ENABLED!", player->getX(), player->getY() - 10, 15.0f, 1);
			gameObjects.push_back(cheatEffect);
			soundManager.playStart();
		}

		else if (cheatCodeBuffer.find("minigun") != std::string::npos && !GAME_OVER)
		{
			CHEATS_MULTIFIRE = false;
			CHEATS_RAPIDFIRE = true;
			cheatCodeBuffer = "";
			FloatingText* cheatEffect = new FloatingText(floating_text, "CHEAT ENABLED!", player->getX(), player->getY() - 10, 15.0f, 1);
			gameObjects.push_back(cheatEffect);
			soundManager.playStart();
		}

		//Do Collison Handling and Update if not paused
		if (!PAUSED && GAME_RUNNING) 
		{

			//Bounds checking

			for (unsigned int i = 0; i < gameObjects.size(); i++)
			{
				//Bounds check for player
				if (gameObjects[i]->getName() == "player")
				{
					if (gameObjects[i]->getX() > MainWindow.getSize().x - gameObjects[i]->getRadius())
						gameObjects[i]->setX(gameObjects[i]->getX() - gameObjects[i]->getRadius());

					else if(gameObjects[i]->getX() < gameObjects[i]->getRadius())
						gameObjects[i]->setX(gameObjects[i]->getX() + gameObjects[i]->getRadius());

					if(gameObjects[i]->getY() > MainWindow.getSize().y - gameObjects[i]->getRadius())
						gameObjects[i]->setY(gameObjects[i]->getY() - gameObjects[i]->getRadius());

					else if(gameObjects[i]->getY() < gameObjects[i]->getRadius())
						gameObjects[i]->setY(gameObjects[i]->getY() + gameObjects[i]->getRadius());
				}

				//Bounds check for bullets
				else if (gameObjects[i]->getName() == "bullet")
				{
					if (gameObjects[i]->getX() > MainWindow.getSize().x - gameObjects[i]->getRadius() || gameObjects[i]->getX() < gameObjects[i]->getRadius()
						|| gameObjects[i]->getY() > MainWindow.getSize().y - gameObjects[i]->getRadius() || gameObjects[i]->getY() < gameObjects[i]->getRadius())
					{
						gameObjects[i]->deactivate();
					}
				}

				//Bounds check for enemies
				else if (gameObjects[i]->getName() == "kamikaze" || gameObjects[i]->getName() == "speeder")
				{
					if (gameObjects[i]->getX() > MainWindow.getSize().x + 50 - gameObjects[i]->getRadius() || gameObjects[i]->getX() + 50 < gameObjects[i]->getRadius()
						|| gameObjects[i]->getY() > MainWindow.getSize().y + 50 - gameObjects[i]->getRadius() || gameObjects[i]->getY() + 50 < gameObjects[i]->getRadius())
					{
						gameObjects[i]->deactivate();
					}
				}

				//Collision handling

				for (unsigned int j = 0; j < gameObjects.size(); j++)
				{
					//No need to check collison with yourself
					if (gameObjects[i] == gameObjects[j])
						continue;

					if (isColliding(gameObjects[i], gameObjects[j]))
					{
						//Bullet colliding with enemy
						if (gameObjects[i]->getName() == "bullet" && gameObjects[j]->getName() == "kamikaze")
						{
							Explosion* effect = new Explosion(small_explosionAni, gameObjects[i]->getX(), gameObjects[i]->getY());
							gameObjects.push_back(effect);
							FloatingText* scoreEffect = new FloatingText(floating_text, "+" + std::to_string(KAMIKAZE_VALUE), gameObjects[j]->getX(), gameObjects[j]->getY() - 10, 25.0f, 2);
							gameObjects.push_back(scoreEffect);
							gameScore += KAMIKAZE_VALUE;

							soundManager.playSExplosion();

							gameObjects[i]->deactivate();
							gameObjects[j]->deactivate();

							if (gameScore >= gameScoreBenchmark * 2000 && gameScore > 0)
							{
								gameScoreBenchmark += 1;
								player->addMines(10);
								FloatingText* textEffect = new FloatingText(floating_text, "+10 Mines", player->getX(), player->getY() - 10, 15.0f, 1);
								gameObjects.push_back(textEffect);
							}
						}

						else if (gameObjects[i]->getName() == "bullet" && gameObjects[j]->getName() == "speeder")
						{
							Explosion* effect = new Explosion(small_explosionAni, gameObjects[i]->getX(), gameObjects[i]->getY());
							gameObjects.push_back(effect);
							FloatingText* scoreEffect = new FloatingText(floating_text, "+" + std::to_string(SPEEDER_VALUE), gameObjects[j]->getX(), gameObjects[j]->getY() - 10, 25.0f, 2);
							gameObjects.push_back(scoreEffect);
							gameScore += SPEEDER_VALUE;

							soundManager.playDExplosion();

							gameObjects[i]->deactivate();
							gameObjects[j]->deactivate();
	
							if (gameScore >= gameScoreBenchmark * 2000 && gameScore > 0)
							{
								gameScoreBenchmark += 1;
								player->addMines(10);
								FloatingText* textEffect = new FloatingText(floating_text, "+10 Mines", player->getX(), player->getY() - 10, 15.0f, 1);
								gameObjects.push_back(textEffect);
							}
						}

						//Menu Item collision
						else if (gameObjects[i]->getName() == "bullet" && gameObjects[j]->getName() == "start")
						{
							FloatingText* menuEffect = new FloatingText(floating_text, "LETS DO THIS!", gameObjects[j]->getX(), gameObjects[j]->getY() - 10, 15.0f, 1);
							gameObjects.push_back(menuEffect);

							soundManager.playStart();

							gameObjects[i]->deactivate();
							gameObjects[j]->deactivate();
							for (unsigned int k = 0; k < gameObjects.size(); k++)
							{
								if (gameObjects[k]->getName() == "vsync" || gameObjects[k]->getName() == "sound" || gameObjects[k]->getName() == "exit")
									gameObjects[k]->deactivate();
							}
							GAME_STARTED = true;
						}

						else if (gameObjects[i]->getName() == "bullet" && gameObjects[j]->getName() == "vsync")
						{
							gameObjects[i]->deactivate();
							if (VSYNC)
							{
								VSYNC = false;
								MainWindow.setVerticalSyncEnabled(false);
								FloatingText* menuEffect = new FloatingText(floating_text, "VSYNC DISABLED", gameObjects[j]->getX(), gameObjects[j]->getY() - 10, 15.0f, 1);
								gameObjects.push_back(menuEffect);
							}
							else
							{
								VSYNC = true;
								MainWindow.setVerticalSyncEnabled(true);
								FloatingText* menuEffect = new FloatingText(floating_text, "VSYNC ENABLED", gameObjects[j]->getX(), gameObjects[j]->getY() - 10, 15.0f, 1);
								gameObjects.push_back(menuEffect);
							}	
						}

						else if (gameObjects[i]->getName() == "bullet" && gameObjects[j]->getName() == "sound")
						{
							gameObjects[i]->deactivate();
							if (SOUND)
							{
								backgroundMusic.stop();
								soundManager.soundOff();
								SOUND = false;
								FloatingText* menuEffect = new FloatingText(floating_text, "SOUND DISABLED", gameObjects[j]->getX(), gameObjects[j]->getY() - 10, 15.0f, 1);
								gameObjects.push_back(menuEffect);
							}
							else
							{
								backgroundMusic.play();
								soundManager.soundOn();
								SOUND = true;
								FloatingText* menuEffect = new FloatingText(floating_text, "SOUND ENABLED", gameObjects[j]->getX(), gameObjects[j]->getY() - 10, 15.0f, 1);
								gameObjects.push_back(menuEffect);
							}		
						}

						else if (gameObjects[i]->getName() == "bullet" && gameObjects[j]->getName() == "exit")
						{

							for (unsigned int i = 0; i < gameObjects.size(); i++)
							{
								delete gameObjects[i];
								gameObjects.erase(gameObjects.begin() + i);
							}
							MainWindow.close();
						}

						//Enemy colliding with player
						else if (gameObjects[i]->getName() == "kamikaze" && gameObjects[j]->getName() == "player")
						{
							Explosion* effect = new Explosion(big_explosionAni, gameObjects[j]->getX(), gameObjects[j]->getY());
							gameObjects.push_back(effect);

							gameObjects[i]->deactivate();
							gameObjects[j]->deactivate();

							soundManager.playBExplosion();

							if (gameScore > highScoreTable[highScoreTable.size() - 1].getScore())
							{
								timeSurvived = gameTime.restart().asSeconds();
								ENTERING_SCORE = true;
							}
							else
							{
								GAME_OVER = true;
							}
						}

						else if (gameObjects[i]->getName() == "speeder" && gameObjects[j]->getName() == "player")
						{
							Explosion* effect = new Explosion(big_explosionAni, gameObjects[j]->getX(), gameObjects[j]->getY());
							gameObjects.push_back(effect);

							gameObjects[i]->deactivate();
							gameObjects[j]->deactivate();

							soundManager.playBExplosion();

							if (gameScore > highScoreTable[highScoreTable.size() - 1].getScore())
							{
								timeSurvived = gameTime.restart().asSeconds();
								ENTERING_SCORE = true;
							}
							else
							{
								GAME_OVER = true;
							}
						}

						//Enemies colliding
						else if ((gameObjects[i]->getName() == "kamikaze" || gameObjects[i]->getName() == "speeder") && 
							(gameObjects[j]->getName() == "kamikaze" || gameObjects[j]->getName() == "speeder"))
						{
							if (gameObjects[i]->getX() > gameObjects[j]->getX())
								gameObjects[i]->setX(gameObjects[i]->getX() + 3);
							else
								gameObjects[i]->setX(gameObjects[i]->getX() - 3);

							if (gameObjects[i]->getY() > gameObjects[j]->getY())
								gameObjects[i]->setY(gameObjects[i]->getY() + 3);
							else
								gameObjects[i]->setY(gameObjects[i]->getY() - 3);
						}


						//Menu item colliding
						else if (gameObjects[i]->getName() == "player" && (gameObjects[j]->getName() == "start" || gameObjects[j]->getName() == "vsync"
							|| gameObjects[j]->getName() == "sound" || gameObjects[j]->getName() == "exit"))
						{
							if (gameObjects[i]->getX() > gameObjects[j]->getX())
								gameObjects[i]->setX(gameObjects[i]->getX() + 3);
							else
								gameObjects[i]->setX(gameObjects[i]->getX() - 3);

							if (gameObjects[i]->getY() > gameObjects[j]->getY())
								gameObjects[i]->setY(gameObjects[i]->getY() + 3);
							else
								gameObjects[i]->setY(gameObjects[i]->getY() - 3);
						}
					}
				}
			}


			//Running Game Logic Update

			//Spawn GameObjects Randomly according to score
			if (gameScore < 1000)
				spawnChance = rand() % 200;
			else if (gameScore < 2500)
				spawnChance = rand() % 180;
			else if (gameScore < 5000)
				spawnChance = rand() % 140;
			else if (gameScore < 7500)
				spawnChance = rand() % 100;
			else
				spawnChance = rand() % 80;

			if (spawnChance == 0 && !GAME_OVER && !ENTERING_SCORE && GAME_RUNNING && GAME_STARTED)
			{
				int spawnChoice;

				if (gameScore > 5000)
					spawnChoice = rand() % 4;
				else
					spawnChoice = rand() % 8;
				
				spawnLocation = rand() % 8;
				if (gameScore > 2000 && spawnChoice == 0)
				{
					EnemyKamikaze* newEnemy = new EnemyKamikaze(en_speederAni, 
						spawnPoints[spawnLocation].x, spawnPoints[spawnLocation].y, 0.0f, 16, 180.0f,"speeder");
					gameObjects.push_back(newEnemy);

					soundManager.playSpawn();
				}
				else
				{
					EnemyKamikaze* newEnemy = new EnemyKamikaze(en_kamikazeAni, 
						spawnPoints[spawnLocation].x, spawnPoints[spawnLocation].y, 0.0f, 20, 80.0f,"kamikaze");
					gameObjects.push_back(newEnemy);

					soundManager.playSpawn();
				}
			}

			//Update GameObjects

			for (unsigned int i = 0; i < gameObjects.size(); i++)
			{
				if (gameObjects[i]->getName() == "kamikaze" || gameObjects[i]->getName() == "speeder")
				{
					if (!GAME_OVER && !ENTERING_SCORE)
						gameObjects[i]->setAngle((atan2f(gameObjects[i]->getY() - player->getY(), gameObjects[i]->getX() - player->getX()) * 180 / 3.141593f) - 90.0f);
					else
						gameObjects[i]->deactivate();
					gameObjects[i]->Update(deltaTime);
				}
				else if(gameObjects[i]->getName() == "bullet" && (GAME_OVER || ENTERING_SCORE))
					gameObjects[i]->deactivate();
				else
					gameObjects[i]->Update(deltaTime);

				//Object deletion

				if (gameObjects[i]->isActive() == false)
				{
					delete gameObjects[i];
					gameObjects.erase(gameObjects.begin() + i);
				}
			}

		}

		//Rendering

		MainWindow.clear(sf::Color::Black);
		MainWindow.draw(background);

		//Render main screen
		if (!GAME_STARTED)
		{
			info_text.setString(infotext);
			MainWindow.draw(logo);
			MainWindow.draw(info_text);
			MainWindow.draw(help);
			if (PAUSED)
				MainWindow.draw(Score_text);
		}

		//Render bullets and mines first
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i]->isActive() && gameObjects[i]->getName() == "bullet")
				gameObjects[i]->Render(MainWindow, DEBUG_MODE);
		}

		//Render the player
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i]->isActive() && gameObjects[i]->getName() == "player")
				gameObjects[i]->Render(MainWindow, DEBUG_MODE);
		}

		//Render everything else
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i]->isActive() && gameObjects[i]->getName() != "player" && gameObjects[i]->getName() != "bullet")
				gameObjects[i]->Render(MainWindow,DEBUG_MODE);
		}

		//Player status text
		playerStat_text.setString("MINES: " + std::to_string(player->getNumMines()));
		playerStat_text.setOrigin(playerStat_text.getLocalBounds().width / 2, playerStat_text.getLocalBounds().height / 2);
		playerStat_text.setPosition({ player->getX(), player->getY() + 30 });

		//Set the color according to current number of mines
		if (player->getNumMines() > 5)
			playerStat_text.setColor(sf::Color::White);
		else if (player->getNumMines() < 5 && player->getNumMines() != 0)
			playerStat_text.setColor(sf::Color::Yellow);
		else if (player->getNumMines() == 0)
			playerStat_text.setColor(sf::Color::Red);

		if(!GAME_OVER && !ENTERING_SCORE)
			MainWindow.draw(playerStat_text);

		//Score counter
		Score_text.setCharacterSize(32);
		Score_text.setPosition({ (float)MainWindow.getSize().x / 2 ,40 });
		Score_text.setColor(sf::Color::White);
		Score_text.setString("score: " + std::to_string(gameScore));
		
		//If paused set Score counter to paused
		if (PAUSED)
		{
			Score_text.setPosition({ (float)MainWindow.getSize().x / 2 ,40 });
			Score_text.setColor(sf::Color::Yellow);
			Score_text.setString("paused");
		}

		//If game is over set Score counter to game over
		else if(GAME_OVER)
		{
			std::string table = "---------- HIGHSCORES ----------\n";
			for (unsigned int i = 0; i < highScoreTable.size(); i++)
			{
				table += highScoreTable[i].getName() + "\t" + std::to_string(highScoreTable[i].getScore()) 
					+ "\t" + std::to_string(highScoreTable[i].getTime()) +"\n";
			}
			scoreTable_text.setOrigin(scoreTable_text.getLocalBounds().width / 2,0);
			scoreTable_text.setString(table);

			Score_text.setCharacterSize(48);
			Score_text.setPosition({ (float)MainWindow.getSize().x / 2,(float)MainWindow.getSize().y / 3});
			Score_text.setColor(sf::Color::Red);
			Score_text.setString("GAME OVER");

			info_text.setOrigin(info_text.getLocalBounds().width / 2, info_text.getLocalBounds().height / 2);
			info_text.setPosition({ (float)MainWindow.getSize().x / 2, (float)MainWindow.getSize().y / 3 + 100});
			info_text.setString("Do you want to try again (Y/N)?");
			
			MainWindow.draw(info_text);
			MainWindow.draw(scoreTable_text);
		}

		//If a new highscore is reached set Score counter to new highscore
		else if (ENTERING_SCORE)
		{
			Score_text.setCharacterSize(48);
			Score_text.setPosition({ (float)MainWindow.getSize().x / 2,(float)MainWindow.getSize().y / 3 });
			Score_text.setColor(sf::Color::Green);
			Score_text.setString("NEW HIGH SCORE");

			info_text.setOrigin(info_text.getLocalBounds().width / 2, info_text.getLocalBounds().height / 2);
			info_text.setPosition({ (float)MainWindow.getSize().x / 2, (float)MainWindow.getSize().y / 2 + 15});
			info_text.setString("You survived for  " + std::to_string(timeSurvived) + "  seconds..." + "\nEnter your name:  " + highscoreName);

			MainWindow.draw(info_text);
		}

		Score_text.setOrigin(Score_text.getLocalBounds().width / 2, Score_text.getLocalBounds().height / 2);
		
		if (GAME_RUNNING && GAME_STARTED)
			MainWindow.draw(Score_text);

		//Set debug string
		debug = "-----DEBUG MODE-----\n FPS: " + std::to_string(fps) + 
			 "\nNumber of GameObjects: " + std::to_string(gameObjects.size()) + "\nCheat buffer status: " + cheatCodeBuffer + "\n\n";

		for(unsigned int i = 0; i < gameObjects.size(); i++)
		{
			debug += "[" + std::to_string(i) + "]" + " " + gameObjects[i]->getName()
				+ " (" + std::to_string(gameObjects[i]->getX()) + "," + std::to_string(gameObjects[i]->getY()) + ") \n";
		}

		Debug_text.setString(debug);

		//If currently in Debug mode then render debug information
		if (DEBUG_MODE)
		{
			for (unsigned int i = 0; i < spawnPoints.size(); i++)
			{
				sf::CircleShape circle(10);
				circle.setFillColor(sf::Color(0,0,0,100));
				circle.setPosition(spawnPoints[i].x, spawnPoints[i].y);
				circle.setOrigin(10, 10);
				MainWindow.draw(circle);
			}

			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(player->getX(), player->getY())),
				sf::Vertex(sf::Vector2f(player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * 2000.0f
									  , player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * 2000.0f))
			};
			line[0].color = sf::Color::Red;
			line[1].color = sf::Color::Transparent;
			MainWindow.draw(line, 2, sf::Lines);

			MainWindow.draw(Debug_text);
		}

		//Render green cheat laser for those who suck at aiming
		if (CHEATS_LASER && !DEBUG_MODE)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(player->getX(), player->getY())),
				sf::Vertex(sf::Vector2f(player->getX() + sin(player->getAngle() * 3.141593f / 180.0f) * 2000.0f
				, player->getY() - cos(player->getAngle() * 3.141593f / 180.0f) * 2000.0f))
			};
			line[0].color = sf::Color::Green;
			line[1].color = sf::Color::Transparent;
			MainWindow.draw(line, 2, sf::Lines);
		}
		
		//Finally display the window
		MainWindow.display();
	}
	/*-------------------------------------------------------*/

	//Game worked successfully and nothing went wrong... yay!
	backgroundMusic.stop();
	fprintf(logFile, "game exit success\n");
	fclose(logFile);
	return EXIT_SUCCESS;
}