#include <Application.h>

int main()
{
	Application app;

	try
	{
		app.Run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}