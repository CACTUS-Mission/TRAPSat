/*
 * Test program for vc0706.cpp
 * compile with: $g++ vc0706_test.cpp -o vc0706_test -lwiringPi
 */
//#include <time.h>
#include <iostream>
#include <string>
//#include <cstring>
#include "vc0706.h"

using namespace std;

int main (int argc, char* argv[])
{

	Camera cam;
	//cam.takePicture("images/time_test.jpg");
	
	clock_t begin, end;
	int j=1;

	if(argv[1][0]-'0' > 0)
		j=argv[1][0]-'0';

	if(cam.getVersion() != NULL)
	{
	  for (int i=0; i<j; i++) 
	  {
		//char * path;
		//sprintf(path, "images/%d.jpg", i);
		//cout << "file: " << path << endl;
		std::string path = "images/"+std::to_string(i)+".jpg";
		//begin = time(0);
		//cam.reset();
		cam.takePicture(path.c_str()); // stores to ./images/i.jpeg
	  	//cam.clearBuffer();
		//end = time(0);
		//cout << "it took about " << (double)end-begin << " seconds to take and store this picture." << endl;
	  }
	}
	

	return 0;
}
