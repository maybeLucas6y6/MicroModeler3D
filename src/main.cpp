#include "app.hpp"

int main()
{
	App myApp;
	if (myApp.Initialize())
	{
		while(myApp.IsRunning())
		{
			myApp.Update();
			myApp.Render();
		}
		myApp.Shutdown();
	}
	return 0;
}