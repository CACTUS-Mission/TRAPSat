/*
 * Test program for vc0706.cpp
 * compile with: $g++ vc0706_test.cpp -o vc0706_test -lwiringPi
 */
#include <stdio.h>
#include "vc0706.cpp" // will probably be changed to vc0706.h eventually

using namespace std;

int main ()
{

	Camera cam;

	//char *vptr = NULL;
	//char *picfile = NULL;

	if(cam.getVersion() != NULL)
	{
		cam.takePicture();
	}
	

	return 0;
}
