#include <stdio.h>
#include "vc0706.cpp" // will probably be changed to vc0706.h eventually

using namespace std;

int main ()
{

	Camera cam;

	Char *vptr = NULL;
	Char *picfile = NULL;

	if(retptr=cam.getVersion() != NULL)
	{
		if(picfile=cam.takePicture() != NULL)
			cout << "picture stored at " << *picfile << endl;
	}
	

	return 0;
}