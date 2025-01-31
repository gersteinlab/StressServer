/*
 * University of Illinois Open Source License
 * Copyright 2009, 2011, 2012 Luthey-Schulten Group, 
 * All rights reserved.
 * 
 * Developed by: Luthey-Schulten Group
 *                           University of Illinois at Urbana-Champaign
 *                           http://www.scs.uiuc.edu/~schulten
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the Software), to deal with 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
 * of the Software, and to permit persons to whom the Software is furnished to 
 * do so, subject to the following conditions:
 * 
 * - Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimers.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimers in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * - Neither the names of the Luthey-Schulten Group, University of Illinois at
 * Urbana-Champaign, nor the names of its contributors may be used to endorse or
 * promote products derived from this Software without specific prior written
 * permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
 * OTHER DEALINGS WITH THE SOFTWARE.
 *
 * Author(s): Anurag Sethi, John Eargle
 * Modifications by: Shantao Li, Declan Clarke (lab of Mark Gerstein, 
 * Yale University, 2015)
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "network.h"

void constructNetwork(Graph *network, pdbFile *macromolecule)
{
	int i, j, k;
	float r;
	
	printf("->constructing Network\n");
	network->nres = macromolecule->nAtoms;
	network->nedges = 0;
	
	if ((network->dis = (int **) malloc(network->nres*sizeof(int *))) == NULL)
	{
		printf("ERROR: could not save network information\n");
		exit(1);
	}
	for (i=0; i<network->nres; i++)
	{
		if ((network->dis[i] = (int *) malloc(network->nres*sizeof(int *))) == NULL)
		{
			printf("ERROR: could not save network information\n");
			exit(1);
		}
	}
	
	//Finding all edges in network
	for (i=0; i<network->nres; i++)
	{
		network->dis[i][i] = 0;
		for (j=(i+1); j<network->nres; j++)
		{
			r = sqrt(((macromolecule->atom[i].x - macromolecule->atom[j].x)*(macromolecule->atom[i].x - macromolecule->atom[j].x)) + ((macromolecule->atom[i].y - macromolecule->atom[j].y)*(macromolecule->atom[i].y - macromolecule->atom[j].y)) + ((macromolecule->atom[i].z - macromolecule->atom[j].z)*(macromolecule->atom[i].z - macromolecule->atom[j].z)));			
			if (r < 8.0)
			{
				network->nedges++;
				network->dis[i][j] = 1;
				network->dis[j][i] = 1;
			} else
			{
				network->dis[i][j] = 0;
			}
		}
	}
	
	printf("Number of edges in the network are %d\n", network->nedges);
	
	if ((network->edge = (edgePtr *)malloc(network->nedges*sizeof(edgePtr))) == NULL)
	{
		printf("ERROR: could not save network information\n");
		exit(1);
	}
	
	k = 0;
	
	for (i=0; i<network->nres; i++)
	{
		for (j=(i+1); j<network->nres; j++)
		{
			if (network->dis[i][j] != 0)
			{
				network->edge[k] = (edgePtr)malloc(sizeof(EdgeStr));
				network->edge[k]->res1 = i;
				network->edge[k]->res2 = j;
				k++;
			}
		}
	}
	
	/*for (i=0; i<network->nres; i++)
	{
		for (j=0; j<network->nres;j++)
			printf("%d ", network->dis[i][j]);
		printf("\n");
	}*/
	
	//printf("Number of edges in the network are %d\n", k);
  	printf("<-constructing Network\n");
  	
  	return;
}

void floydWarshall(Graph *network, int *residues, int residueCount)
{
  	printf("->floydWarshall\n");
  	//printf("  residueCount = %d\n",residueCount);
  	int i, j, k;
	int BIG = 10000;
	

	if ((network->shortDis = (int **)malloc(residueCount*sizeof(int *))) == NULL)
	{
		printf("Error: No space to save shortest network distance\n");
		exit(1);
	}

	for (i=0; i<residueCount; i++)
	{
		if ((network->shortDis[i] = (int *)malloc(residueCount*sizeof(int))) == NULL)
		{
			printf("Error: No space to save shortest network distance\n");
			exit(1);
		}
	}
	
	/*for (i=0; i<network->nres; i++)
	{
		for (j=0; j<network->nres;j++)
			printf("%d ", network->dis[i][j]);
		printf("\n");
	}*/
	/* Initialization of shortest distance and shortest path matrices. */
	for (i=0; i<residueCount; i++) 
  	{
  		for (j=i; j<residueCount; j++) 
    	{
      		if (i == j) {
				network->shortDis[residues[i]][residues[j]] = 0;
      		} else if (network->dis[residues[i]][residues[j]] != 0) {
				network->shortDis[residues[i]][residues[j]] = network->dis[residues[i]][residues[j]];
				network->shortDis[residues[j]][residues[i]] = network->dis[residues[i]][residues[j]];
      		} else {
				network->shortDis[residues[i]][residues[j]] = BIG; /* Arbitrary large number */
				network->shortDis[residues[j]][residues[i]] = BIG; /* Arbitrary large number */
      		}
    	}
  	}
  	
  	/*for (i=0; i<network->nres; i++)
	{
		for (j=0; j<network->nres;j++)
			printf("%d ", network->shortDis[i][j]);
		printf("\n");
	}*/
	
  	printf("  Initialization done.\n");

  	/* Dynamic programming algorithm. */
  	for (k=0; k < residueCount; k++) {
    	for (i=0; i < residueCount; i++) {
      		for (j=i; j < residueCount; j++) {
				/*printf("%d %d %d \n", k, j, i);*/
				if (network->shortDis[residues[i]][residues[j]] > (network->shortDis[residues[i]][residues[k]] + network->shortDis[residues[k]][residues[j]])) {
	  				network->shortDis[residues[i]][residues[j]] = network->shortDis[residues[i]][residues[k]] + network->shortDis[residues[k]][residues[j]];
	  				network->shortDis[residues[j]][residues[i]] = network->shortDis[residues[i]][residues[j]];
				}
      		}
    	}
  	}
  	printf("  Distances calculated.\n");

	/*for (i=0; i<network->nres; i++)
	{
		for (j=0; j<network->nres;j++)
			printf("%d ", network->shortDis[i][j]);
		printf("\n");
	}*/
  
  	printf("<-floydWarshall\n");
  	return;
}

void calculateLC(Graph *network, int *residue, int residueCount)
{
	int i, j;
	int n1, n2, n3, n4;
	
	printf("->calculateLC\n");
	if ((network->LC = (float *)malloc(residueCount*sizeof(float))) == NULL)
	{
		printf("No memory space to save network information\n");
		exit(1);
	}
	
	for (i=0; i<residueCount; i++)
	{
		n1 = 0;
		n2 = 0;
		n3 = 0;
		n4 = 0;
		
		for (j=0; j<residueCount; j++)
		{
			if (network->shortDis[residue[i]][residue[j]] == 1)
			{
				n1++;
			} else if (network->shortDis[residue[i]][residue[j]] == 2)
			{
				n2++;
			} else if (network->shortDis[residue[i]][residue[j]] == 3)
			{
				n3++;
			} else if (network->shortDis[residue[i]][residue[j]] == 4)
			{
				n4++;
			}
		}
		
		network->LC[i] = n1 + (float)n2/4. + (float)n3/9. + (float)n4/16.;
		//printf("i %d LC[i] %f\n", i, network->LC[i]);
	}
	
	printf("<-calculateLC\n");
	return;
}