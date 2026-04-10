//ANTCODE: system.cpp System Class Function Definitions (Revision Date: February 12, 2017)
#include "system.h"

void System::CreateAnts()
{
    const gsl_rng_type * gsl_T;
    gsl_rng * gsl_r;
    gsl_T = gsl_rng_default;
    gsl_r = gsl_rng_alloc (gsl_T);
    gsl_rng_set(gsl_r,/*RANDOMSEED*/ time(NULL)); //seed generation changes every second 
    int* cellindex=new int[NG2];//index of all possible positions
    for(int i=0; i<NG2; i++)
      cellindex[i]=i;
    int* filledcells=new int[NANT];//index of all possible positions
    gsl_ran_shuffle(gsl_r, cellindex, NG2, sizeof(int));
    gsl_ran_choose(gsl_r, filledcells, NANT, cellindex, NG2, sizeof(int));
    for(int i=0; i<NANT; i++) //iterating over ants  
    {
      AntCluster ac;
      ac.cells.push_back(filledcells[i]);
      AC.push_back(ac);  //stores info for all the ants.
    }
  //  cout<<"Ants created\n";
	delete[] filledcells;
	delete[] cellindex;
    gsl_rng_free (gsl_r);
}

void System::CreateWalls()
{
    const gsl_rng_type * gsl_T;
    gsl_rng * gsl_r;
    gsl_T = gsl_rng_default;
    gsl_r = gsl_rng_alloc (gsl_T);
    gsl_rng_set(gsl_r,time(NULL));
    int ntot=NG*(NG+1)*2;//i< NG*(NG+1) if vertical, i>= NG*(NG+1)even if horizontal
    int* wallindex=new int[ntot]; //to store the index for all the cell boudaries.
    walls=new int[NWALL]; //to store the index for all the walls among all the cell boundaries.
    for(int i=0; i<ntot; i++)
      wallindex[i]=i;
    gsl_ran_shuffle(gsl_r, wallindex, ntot, sizeof(int));
    gsl_ran_choose(gsl_r, walls, NWALL, wallindex, ntot, sizeof(int));
   /* for(int i=0; i<NWALL; i++)
      cout<<"wall # "<<i+1<<" with index"<<walls[i]<<endl;*/
  //  cout<<"Walls created\n";
    gsl_rng_free (gsl_r);
	delete[] wallindex;
}

void System::CreateCells()
{
    //create without wall and ant entries
    for(int i=0; i<NG2; i++)
    {
	Cell c;
	c.row=int(i/NG);
	c.col=i%NG;
	//cout<<"i= "<<i<<" row= "<<c.row<<" col= "<<c.col<<endl;
	
	//Left Neighbor
	if(c.col!=0)
	  c.nbrs.push_back(i-1);
	else
	  c.nbrs.push_back(i - 1 + NG); //PBC
	//Right Neighbor
	if(c.col!=NG-1)
	  c.nbrs.push_back(i+1);
	else
	  c.nbrs.push_back(i + 1 - NG); //PBC
	//Top Neighbor
	if(c.row!=NG-1)
	  c.nbrs.push_back(i+NG);
	else
	  c.nbrs.push_back(-1);
	//Bottom Neighbor
	if(c.row!=0)
	  c.nbrs.push_back(i-NG);
	else
	  c.nbrs.push_back(-1);
	C.push_back(c); //c is an individual cell, C is an entire lattice storing info for all the cells
    }
    //fill ants 
    list<AntCluster>::iterator it;
    for (it=AC.begin(); it!=AC.end(); it++)
    {
      list<int>::iterator it2;
      for(it2=(*it).cells.begin(); it2!=(*it).cells.end(); it2++)
	  C[(*it2)].isAnt=true; //marks true for all the cells occupied ants in the matrix.
    }
    //put wall 
    for(int i=0; i<NWALL; i++)
    {
	int itemp, irow, icol;
	int w=walls[i]; //w is index of the i'th wall 
	if(w<NG2) //vertical wall (either left or right)
	{
	  itemp=w; 
	  irow=int(itemp/(NG)); //In any given row there are NG walls.
	  icol=itemp%(NG); //Goes from 0 to NG-1
	 // cout<<"irow="<<irow<<" icol="<<icol<<endl;
	  if(icol!=NG) 
	    C[irow*NG+icol].isWall[0]=true; //left wall 
	  if(icol!=0)
	    C[irow*NG+icol-1].isWall[1]=true; //right wall
		//2 if conditions bcz they same wall is shared by 2 cells unless it is the outermost cells.
	  else//for PBC
		C[irow * NG + icol + NG - 1].isWall[1] = true; //marks right wall for icol=0	
	}
	else //horizontal wall
	{
	  itemp=w-NG*(NG+1);
	  irow=int(itemp/NG);
	  icol=itemp%NG;
	  //cout<<"irow="<<irow<<" icol="<<icol<<endl;
	  if(irow!=NG)
	    C[irow*NG+icol].isWall[3]=true;//bottom wall
	  if(irow!=0)
	    C[(irow-1)*NG+icol].isWall[2]=true;//top wall
	}
    }
    //check if cells are blocked  
    for(int i=0; i<NG2; i++)
	C[i].findperm();
    //cout<<endl;
   if(PWALL==0.0 && WALLF>0.0) //i.e. wall is completely blocking.
    {
	for(int i=0; i<NG2; i++) //iterating over all the cells.
	{
	    double tot_perm=C[i].perm;
	    //check one cell
	    if(tot_perm==1.0)
	    {
		C[i].isBlock=true; //ith cell is blocked as Pwall = 0.0
		continue;
	    }
	    else
	    {
		//check for up to two cells
		for(int j=0; j<4; j++)
		{
		    if(C[i].nbrs[j]!=-1 && C[i].isWall[j]==false) //neighboring cell present and no common wall
		    {
			tot_perm=C[i].perm+C[C[i].nbrs[j]].perm; 
			if(tot_perm==6.0/4.0) 
			{
			  C[i].isBlock=true;
			  break; 
			}
		    }
	      }
	    }
	      
	}
    }
  //  cout<<"Cells created\n";
}

void System::Move()
{
  const gsl_rng_type * gsl_T;
  gsl_rng * gsl_r;
  gsl_T = gsl_rng_default;
  gsl_r = gsl_rng_alloc (gsl_T);
  gsl_rng_set(gsl_r,time(NULL));
  int nantc=NANT;//number of remaining clusters, a code need to be added to find initial no. of clusters
  int nant=NANT; //number of remaining ants
  list<AntCluster>::iterator it; //iterates over ant clusters
  list<int>::iterator it2;
  int sample_count=1;

  // wall_index[t][i] = top-most cell index in column i with an intact top wall at time t, -1 if none
  wall_index.assign(MAXSWEEPS + 1, vector<int>(NG, -1));
  for(int i=0;i<NG;i++)
  {
	wall_index[0][i]=NG*(NG-1)+i; //start scan from the top row of each column
  }
  for(int k=0; k<MAXSWEEPS; k++) //monte carlo steps 
  {
	for(int i=0;i<NG;i++) //iterating over the columns in the lattice.
	{
		int j = wall_index[k][i];
		while(j>=0 && C[j].isWall[2]==false)
		{
			j -= NG;
		}
		if(j<0)
		{
			wall_index[k+1][i] = -1;
			continue;
		}
		else if(C[j].isWall[2]==true)
		{
			double r = gsl_rng_uniform(gsl_r);  // r in [0, 1)
			if(r<k_d){
				C[j].isWall[2]=false;
				j -= NG;
				while(j>=0 && C[j].isWall[2]==false)
					j -= NG;
			}
			wall_index[k+1][i]=j;
		}	
	}

   //  cout<<k<<".jpeg: "<<nant<<" ants in "<<AC.size()<<" clusters\n";
      if((k)%NSAMPLE==0)
		writeGNU(k);
      if(nant==0) //when all have escaped 
      {
		cout<<"tesc="<<k<<endl;
		break;
      }

     for(int j=0; j<AC.size(); j++) //moves all the ant cluster in every sweep
      {
	      nantc=AC.size();
	      //choose an antcluster randomly
	      int antcindex=gsl_rng_uniform_int(gsl_r,nantc); //generating a random number btw 0 to nantc-1
	      it=AC.begin(); 
	      for(int l=0; l<antcindex; l++)
		    it++; //it points to ant cluster at the antcindex.
	      
	      //move all cells in that cluster if possible, store all new cells in an vector
	      vector<int> newcells; //new set of cells occupied by cluster after movement.
	      vector<int> oldcells; 
	      //choose random number between 0 to 3
	      int r=gsl_rng_uniform_int(gsl_r,4); 
	      for(it2=(*it).cells.begin(); it2!=(*it).cells.end(); it2++) //iterating over cells in the cluster
	      {
		  int cellindex=(*it2); //cell index of the iterating cell
		//  cout<<"cellindex="<<cellindex<<endl;
		  oldcells.push_back(cellindex); //stores the old cell index of the cluster.
		  double move_r; 
		  if(C[cellindex].isWall[r]) 
		    move_r=gsl_rng_uniform_pos(gsl_r); //generate a random number btw 0 to 1 
		  else
		    move_r=0.0; 
		  //check whether movement is possible with walls
		  if(move_r>PWALL)//even if one of the cell can't move, leave this cluster
		    break;
		  else
		    newcells.push_back(C[cellindex].nbrs[r]); //stores the new index of the ant.
	      }
	      //cout<<"out of loop"<<endl;
	      if(newcells.size()!=(*it).cells.size()) //go to next cluster if all ants 
	      {                                       //in the clusters could'nt be displaced
		  continue;
	      } //no else to this if
	      bool newwall=false;
	      //check whether there are wall between new cells
	      for(int m=0; m<newcells.size(); m++)
	      {
			if(newcells[m]==-1) //if ant has escaped move to the next ant.
		    	continue;
		//all neigbors of newcells[m]
		for(int l=0; l<4; l++)
		{
		  if(C[newcells[m]].nbrs[l]==-1) //no nbr in this direction, move to the next direction. 
		    continue;
		  for(int n=0; n<newcells.size(); n++) 
		  {
		      if(C[newcells[m]].nbrs[l]==newcells[n] && C[newcells[m]].isWall[l]==true) 
			  //check if the new move results is a neighbour and a wall between them.
		      {
			double move_r=gsl_rng_uniform_pos(gsl_r);
			//cout<<"wall found"<<endl;
			if(move_r>PWALL)
			{
			  newwall=true;
			  break;
			}
		      }
		  }
		  if(newwall)
		    break;
		}
		if(newwall)
		  break;
	      }
	      if(newwall)
		continue;
	      
	      //now check if the new cells are not occupied by other clusters
	      int i;
	      //temporary set all the isAnt of oldcells to false 
	      for(int m=0; m<oldcells.size(); m++)
		  C[oldcells[m]].isAnt=false; //why false
	      for(i=0; i<newcells.size(); i++)
	      {
		  if(newcells[i]==-1) //ant has escaped the lattice 
		    continue; //move to the next ant new location.
		  else if(C[newcells[i]].isAnt)
		    break; //if ant at the new location, movement not possible.
	      }
	      if(i<newcells.size()) //in case the loop breaks 
	      {
		  for(int m=0; m<oldcells.size(); m++)
		      C[oldcells[m]].isAnt=true;
		  continue; //go to next cluster
	      }
	      else //all conditions are met, cluster can move.
	      {
		  i=0; 
		  it2=(*it).cells.begin(); 
		  while(it2!=(*it).cells.end())
		  {
		      int cellindex=(*it2);
		      if(newcells[i]==-1) //ant has escaped the lattice
		      {  
				if(r==3) //exit from the bottom-not allowed
				{
					for(int m=0; m<oldcells.size(); m++)
		      			C[oldcells[m]].isAnt=true;
					break;
				}
				else
				{
					tesc.push_back(k); 
					nant--;
						if((*it).cells.size()==1) 
						{
						//cout<<"cluster escape\n";
						it=AC.erase(it); 
						break; //entire cluster has escaped 
						}
						else //deletes the ant
						it2=((*it).cells).erase(it2);  //deletes and moves to the next ant
				}		
		      } 
		      else 
		      {
				
			  C[newcells[i]].isAnt=true; 
			  (*it2)=newcells[i]; //updating the index of the cell list
			  it2++; //moves to the next cell.
		      }
		      i++; 
		  }
		// cout<<"i="<<i<<"\tnewcells.size="<<newcells.size()<<endl;
	    }
      }

      //check if all remaining cells are blocked
      int block=0; //number of blocked ants
      for(int j=0; j<NG2; j++) 
      {
	  if(C[j].isBlock && C[j].isAnt)
	      block++; 
      }
      if(block==nant && nant!=0) //Final condition
      {
	  cout<<"ants blocked in remaining cells"<<endl;
	  break;
      }
  }
  cout<<nant<<" ants remaining"<<endl;
  gsl_rng_free (gsl_r);
}

void System::writeGNU(int index)
{
    ofstream out;
    char FileName[100]; 
    sprintf(FileName,"%d.dat",index);
    out.open(FileName);
    //write ant data
    for(int i=0; i<NG2; i++)
	if(C[i].isAnt)
	    out<<C[i].col+0.5<<" "<<C[i].row+0.5<<endl; //0.5?
    out.close();
    
    sprintf(FileName,"%d.gnu",index);
    out.open(FileName);
    out<<"set terminal jpeg"<<endl;
    out<<"unset border"<<endl;
    out<<"set size square 1,1"<<endl;
    out<<"set xrange["<<0<<":"<<NG+1<<"]"<<endl;
    out<<"set yrange["<<0<<":"<<NG+1<<"]"<<endl;
    out<<"set output \""<<index<<".jpeg\""<<endl;
    out<<"unset tics"<<endl;
    out<<"unset key"<<endl;
    //write walls
    int w,irow, icol;
    for(int i=0; i<NWALL; i++)
    {
	if(walls[i]<NG*(NG+1))//vertical wall
	{
	    w=walls[i];
	    irow=int(w/(NG+1));
	    icol=w%(NG+1);
	    out<<"set arrow from "<<icol<<","<<irow<<" to "<<icol<<","<<irow+1<<" nohead lc rgb \'black\'"<<endl;
	}
	else//horizontal wall
	{
	    w=walls[i]-NG*(NG+1);
	    irow=int(w/NG);
	    icol=w%NG;
	    out<<"set arrow from "<<icol<<","<<irow<<" to "<<icol+1<<","<<irow<<" nohead lc rgb \'black\'"<<endl;
	}
    }
    out<<"p \""<<index<<".dat\" w p ps 3 pt 3 lc rgb \'black\'"<<endl; //??
    out.close();
}

void System::writeOutput() 
{
    char FileName[100];
    sprintf(FileName,"rand_%d.dat",RANDOMSEED); //modifies the filename 
    //find tesc_av  
    ofstream out; // creates an output file stream object out.
    out.open(FileName); 
    int n=NANT;
	int m = tesc.size(); //number of ants escaped
    int index=0;
    for (int i = 0; i <= MAXSWEEPS; i++) //to ensure movements in the last timestep are also tracked
		{
			out << setw(6) << "t" << "\t" << setw(12) << i << "\t" << setw(6) << "NANT" << "\t" << setw(12) << n << "\t" << endl;
			while (index < m && tesc[index] == i)
			{
				n--;
				index++;
			}
		}
	out.close();

	ofstream areaOut;
	areaOut.open("PolymerArea.txt");
	for (int i = 0; i <= MAXSWEEPS; i++) //polymer wall area vs time
	{
		int area = 0; //total number of cells below the top of the walls in the lattice.
		for(int j=0;j<NG;j++)
		{
			int num_cell = 0;
			if (wall_index[i][j] >= 0)
				num_cell = wall_index[i][j]/NG + 1;
			area+= num_cell;
		}
		areaOut << setw(6) << "t" << "\t" << i << "\t" << setw(6) << "Area" << "\t" << area << endl;
	}
	areaOut.close();
}

