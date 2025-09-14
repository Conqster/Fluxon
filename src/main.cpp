
#include "Core/Application.h"

extern Fluxon::Application* CreateApplication();

int main()
{
	auto app = CreateApplication();

	try
	{
		app->Launch();
		app->CleanUp();

		delete app;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}