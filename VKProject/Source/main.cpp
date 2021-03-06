#include <iostream>

#include "Core/Application.h"

int main()
{
	Application* app = new Application;

	try
	{
		app->Run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
		
		delete app;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}