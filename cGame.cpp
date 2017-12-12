/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	int CurrentScore = 1;

	// Store the textures
	textureName = { "asteroid1", "asteroid2", "asteroid3", "asteroid4", "bullet","theRocket","theBackground"};
	texturesToUse = { "Images\\Present1.png", "Images\\Present2.png", "Images\\Present3.png", "Images\\Present4.png", "Images\\bullet.png", "Images\\PlayerSprite.png", "Images\\SantaBckGrnd.png"};
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge" };
	fontsToUse = { "Fonts/ChristmasSnow.ttf", "Fonts/ChristmasSnow.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Score: " };
	
	theTextureMgr->addTexture("Title", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "Box1", "Box2" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/JB-BckGrndMusic.wav", "Audio/Box1.wav", "Audio/Box2.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	theRocket.setSpritePos({ 370, 600 });
	theRocket.setTexture(theTextureMgr->getTexture("theRocket"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("theRocket")->getTWidth(), theTextureMgr->getTexture("theRocket")->getTHeight());
	theRocket.setRocketVelocity({ 0, 0 });

	// Create vector array of textures

	for (int astro = 0; astro < 3; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		//theAsteroids[astro]->setSpritePos({ 100 * (rand() % 5 + 1), 50 * (rand() % 5 + 1) });

		int RandSpawn = (rand() % 3);
		switch (RandSpawn)
		{

		case 0:
		{
			theAsteroids[astro]->setSpritePos({ 185, -80 });
		}
		break;

		case 1:
		{
			theAsteroids[astro]->setSpritePos({ 370, -80 });
		}
		break;

		case 2:
		{
			theAsteroids[astro]->setSpritePos({ 555, -80 });
		}
		break;

		default:
			break;
		}

		theAsteroids[astro]->setSpriteTranslation({ 0 , 2 });
		int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity({ 1 , 1 });
		theAsteroids[astro]->setActive(true);
	}

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	bool loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	// Render each asteroid in the vector array
	for (int draw = 0; draw < theAsteroids.size(); draw++)
	{
		theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
	}

	// Render each bullet in the vector array
	/*for (int draw = 0; draw < theBullets.size(); draw++)
	{
		theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
	}*/

	// Render the Title
	cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
	SDL_Rect pos = { 25, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	FPoint scale = { 10, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	
	// render the rocket
	theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Update the visibility and position of each asteriod
	vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
	while (asteroidIterator != theAsteroids.end())
	{
		if ((*asteroidIterator)->isActive() == false)
		{
			asteroidIterator = theAsteroids.erase(asteroidIterator);
		}
		else
		{
			(*asteroidIterator)->update(deltaTime);
			++asteroidIterator;
		}
	}


	// Update the visibility and position of each bullet
	/*vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
	while (bulletIterartor != theBullets.end())
	{
		if ((*bulletIterartor)->isActive() == false)
		{
			bulletIterartor = theBullets.erase(bulletIterartor);
		}
		else
		{
			(*bulletIterartor)->update(deltaTime);
			++bulletIterartor;
		}
	}*/
	/*
	==============================================================
	| Check for collisions
	==============================================================
	*/
	/*for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
	{
		//(*bulletIterartor)->update(deltaTime);
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
			{
				// if a collision set the bullet and asteroid to false
				(*asteroidIterator)->setActive(false);
				(*bulletIterartor)->setActive(false);
				theSoundMgr->getSnd("explosion")->play(0);
			}
		}
	}*/

	for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
	{
		if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &theRocket.getBoundingRect()))
		{
			//if a collision occurs between the asteroid and the rocket sets asteroid to false
			(*asteroidIterator)->setActive(false);
			theSoundMgr->getSnd("Box1")->play(0);
			theSoundMgr->getSnd("Box2")->play(0);
		}
	}


	// Update the Rockets position
	theRocket.update(deltaTime);
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;

				case SDL_BUTTON_RIGHT:
					break;

				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;

				case SDLK_LEFT:
				{
					theRocket.setSpritePos({ 185, 600 });
				}
				break;

				case SDLK_DOWN:
				{
					theRocket.setSpritePos({ 370, 600 });
				}
				break;

				case SDLK_RIGHT:
				{
					theRocket.setSpritePos({ 555, 600 });
				}
				break;
				
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

