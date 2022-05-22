#pragma once
#include "renderer.h"
#include <string>
#include <input.h>



/**
	Abstract game interface which is retrieved 
	from the engine at startup by calling the function 
	<code>GetGame</game>.

	When the game is initialized, its Init() method is called.

	During runtime of the game, the engine calls the 
	"DoFrame" method of the game and provides it 
	with input, audio and graphics objects the game can interact with. 

	When the game is closed down, its "ShutDown" method is called from 
	the engine.

*/
class Game {


public:
	virtual void Init(Renderer& renderer) = 0;
	virtual void DoFrame(Renderer& renderer, FrameInput* input, long long frameTime) = 0;
	virtual void ShutDown() = 0;

	/**
	This method is called from the engine to retrieve the filename
	of the Intro image to load.
	Failing to deliver it will cause the engine to stop with an
	error message.
	*/
	virtual std::string GetIntroImageName() = 0;

};

/**
	This method is called from the engine to retrieve a
	pointer to the game instance. 
	The game is supposed to provide such an instance, otherwise
	the engine will just stop with an error message during 
	startup.
*/

Game* GetGame();


/**
	Encapsulates a python game module, which exposes methods called from the
	engine during initialization and frame execution.

*/
class ScriptGameModule {

public:
	ScriptGameModule(const std::string& name);
	void CallDoFrame();
	void CallInit();

private:


};

/**
	The main artifacts in the game world.
	GameObjects hold the state of the game. 
	They can (but don't need to) be rendered and 
	get a chance to update their state each frame. 
*/
class GameObject {
public:

private:
	XMFLOAT3 _worldPosition;

};

