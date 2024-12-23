Legal Notice
============

 Copyright (C) 2017
 Indian Institute of Technology, Roorkee (www.iitr.ac.in)
 The University of Sheffield (www.sheffield.ac.uk)

 This file is part of Entropy based Termination Criterion for 
 Multi-objective Evolutionary Algorithms

 GNU Lesser General Public License Usage
 This file may be used under the terms of the GNU Lesser General
 Public License version 2.1 as published by the Free Software
 Foundation and appearing in the file LICENSE.LGPL included in the
 packaging of this file.  Please review the following information to
 ensure the GNU Lesser General Public License version 2.1 requirements
 will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.


Introduction
============
Thanks for downloading this software. You can find out more information about the algorithm by checking the following papers:
1) D. K. Saxena, Arnab Sinha, J. A. Duro, Q. Zhang, "Entropy-Based Termination Criterion for Multiobjective Evolutionary Algorithms", vol. 20, no. 4, pp. 485-498, 2015
2) J. A. Duro, D. K. Saxena, "Timing the Decision Support for Real-World Many-Objective Optimization Problems", In: Trautmann H. et al. (eds) Evolutionary Multi-Criterion Optimization. EMO 2017. Lecture Notes in Computer Science, vol 10173. Springer, 2017

This software applies a dissimilarity measure to quantify the difference between two population of non-dominated solutions for a given multi-objective optimization problem. Each solution is represented by an objective vector. 


Instructions towards use of the Algorithm
=========================================
(Note: There is a Debug and Release folder with Makefiles. The Makefile in the Debug folder compiles the software with debug flags on, and no debug flags are used when the Makefile of the Rease folder is used.)

Step-I: Cleaning the compilation
  make clean

Step-II: Compilation
  make

Step-III: Execution
  ./entropy_measure/Release/entropy_measure [number_objectives] [number_bins] [parent_file] [child_file]


Sample example
==============
Consider that one intends to run the algorithm on CTP1 problem. Assuming that the user choses to define the number of objectives as 2, the number of bins as 20, the parent_file as ctp1_moea_1.out, and the child_file as ctp1_moea_2.out, the command to use is:

  ./entropy_measure/Release/entropy_measure 2 20 ctp1_moea_1.out ctp1_moea_2.out
 
The output is
  I: 1.689506e-01
  O: 6.220245e+00
  D: 6.389195e+00

The above values correspond to the difference between the two populations, with respect to:
  I: intersection region
  O: non-intersection region
  D: dissimilarity measure


External Libraries
==================

   m


Compilers
=========

   g++

