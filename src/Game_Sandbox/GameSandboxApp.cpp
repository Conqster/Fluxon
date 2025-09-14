#include "Core/Application.h"



class GameSandbox : public Fluxon::Application
{
public: 
	GameSandbox() : Fluxon::Application() {

	}

	~GameSandbox() {

	}
};

Fluxon::Application* CreateApplication()
{
	return new GameSandbox();
}

