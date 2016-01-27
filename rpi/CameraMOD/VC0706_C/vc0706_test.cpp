/*
 * Test program for vc0706.cpp
 * compile with: $g++ vc0706_test.cpp -o vc0706_test -lwiringPi
 */
#include <time.h>
#include <stdio.h>
#include "vc0706.cpp" // will probably be changed to vc0706.h eventually

using namespace std;

int main (int argc, char* argv[])
{

	Camera cam;
	
	clock_t begin, end;
	int j=1;
	
	if(argv[1][0]-'0' > 0)
		j=argv[1][0]-'0';

	if(cam.getVersion() != NULL)
	{
	  for (int i=0; i<j; i++) 
	  {
		begin = time(0);
		cam.reset();
		cam.takePicture(); // stores to ./images/<time>.jpeg
	  	cam.clearBuffer();
		end = time(0);
		cout << "it took about " << (double)end-begin << " seconds to take and store this picture." << endl;
	  }
	}
	

	return 0;
}
