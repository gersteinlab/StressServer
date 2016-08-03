import numpy as np
import sys
import os
#from math import exp
#from Bio.PDB import *

## Author: Declan Clarke (lab of Mark Gerstein, Yale University, 2015)

'''
This script simply takes a contact map (generated by make_contact_map.py) and modifies it such
that it is exclusively a binary contact map - ie, in the orig contact map, a line starts w/info
such as the following:
3 4  LEU B      
then it generates a new contact map w/o this (the binary-only file format is needed by gncommunities).
'''

###  Assign input variable:
input_non_binary_contact_map_to_read = sys.stdin
#input_non_binary_contact_map_to_read = open(input_non_binary_contact_map, "r")


for line in input_non_binary_contact_map_to_read:
	#print line
	line_elements = list()
	line_elements = line.split()
	i = 4
	while i < len(line_elements):
		sys.stdout.write(str(line_elements[i]) + " ",)
		i += 1
	sys.stdout.write("\n")
