//ANTCODE: main.cpp Main Program (Revision Date: January 15,2017)
//Main program, combines all other code files.
#include "header.h" 
#include "system.h"
#include <boost/filesystem.hpp>


int main(int argc, char *argv[])
{
	time_t start, end;
	time(&start); //real computational time 
	
	System sys;
	sys.ReadInput(argc,argv);
	sys.CreateAnts();
	sys.CreateWalls();
	sys.CreateCells();
	sys.Move();
	sys.writeOutput();


        time(&end);
	cout<<"Time Elapsed="<<difftime(end,start)<<endl;
        return 0;
}
