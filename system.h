//AntCode: system.h System Class (Revision Date: February 12, 2017)
#ifndef _SYSTEM_H
#define _SYSTEM_H
#include "header.h"
#include "cell.h"
#include "antcluster.h"
#include <boost/filesystem.hpp>

class System
{
  public:
    int RANDOMSEED, MAXSWEEPS,NG,NG2,NANT,NWALL, NSAMPLE;
    double WALLF,PWALL,ANTF, k_d;
    
/* NUMERICAL SETTINGS (INPUT)
 * int RANDOMSEED=seed for random number generator
 * int MAXSWEEPS=maximum number of MC sweeps (every sweep meaning trial movement of all ants
 * int NSAMPLE=sampling frequency in number of MC sweeps
 * 
 * MODEL PARAMETERS (INPUT)
 * double WALLF=fraction of walls (#wall/total possible walls)
 * double PWALL=probability of crossing wall (0 for completely block, 1 if completely free)
 * dount ANTF=fraction of ants (#ants/#cells)
 * int NG=number of cells in each direction
 * 
 * MODEL PARAMETERS (DERIVED)
 * int NG2=total #cells=round off NG*NG
 * int NWALL=total #wall=round off WALLF*(NG+1)*(NG+1)
 * int NANT=total #ant=round off ANTF*NG2
 */
     
    void ReadInput(int argc, char *argv[])
    {
        options_description desc("Usage:\nANT <options>");
        desc.add_options()
        ("help,h", "print usage message")
        ("MAXSWEEPS,s", value<int>(&MAXSWEEPS)->default_value(100), "max. no. of MC sweeps (default 100)")
	    ("RANDOMSEED,r", value<int>(&RANDOMSEED)->default_value(1), "seed for random number generator (default 1)")
	    ("NSAMPLE,S", value<int>(&NSAMPLE)->default_value(100), "sampling frequency in #MC sweeps (default 100)")
        ("WALLF,w", value<double>(&WALLF)->default_value(0.5), "fraction of walls (default 0.5)")
	    ("PWALL,p", value<double>(&PWALL)->default_value(0.0), "probability of crossing wall (0 for completely block, 1 if completely free) (default 0.0)")
        ("ANTF,a", value<double>(&ANTF)->default_value(0.5), "fraction of ants (default 0.5)")
        ("NG,n", value<int>(&NG)->default_value(10), "#cells in each direction (default 10) ")
        ("k_d,k_d", value<double>(&k_d)->default_value(0.001), "#probabilty of wall decay in a single MC step (default 0.001) ");
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help"))
        {
          cout << desc << "\n";
          exit(1);
        }
        
        //Calculate other parameters
        NG2=NG*NG;
	NANT=int(round(ANTF*double(NG2)));
	NWALL=int(round(WALLF*double(NG*(NG+1)*2)));
	cout<<"Input Read\n"<<NG2<<" cells, "<<NWALL<<" walls, "<<NANT<<" ants.\n";
    }
    
    list<AntCluster> AC;	//list of ant clusters, in the beginning all ants are an independent cluster
    void CreateAnts(); 
    void CreateAntClusters();
    
    vector<int> tesc;//escape time
    vector<vector<int>> wall_index; //keeping track of walls at the top of each column in the lattice
    int *walls;
    void CreateWalls();
    
    vector<Cell> C; //vector of cells
    void CreateCells();
    
    void Move();
    void writeGNU(int);
    
    void writeOutput();
};
#endif
