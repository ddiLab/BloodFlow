/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#ifdef FIX_CLASS

FixStyle(lbm/boundary,FixLbmBoundary)

#else

#ifndef LMP_FIX_LBM_BOUNDARY_H
#define LMP_FIX_LBM_BOUNDARY_H

#include "fix.h"
#include <vector>

using namespace std;

namespace LAMMPS_NS {

class FixLbmBoundary : public Fix {
 public:
  struct Boundary{
    int index;
    char * type[30];
    double data[3];//pressure/velocity,etc.
    void (FixLbmBoundary::*bcDynamics)(double ****);
  }; 
  
  vector <Boundary> BC;

  FixLbmBoundary(class LAMMPS *, int, char **);
  ~FixLbmBoundary();
  int setmask();
  void init();

  void velXlo(double ****);
  void velXhi(double ****);
  void velYlo(double ****);
  void velYhi(double ****);
  void velZlo(double ****);
  void velZhi(double ****);
  void preXlo(double ****);
  void preXhi(double ****);
  void preYlo(double ****);
  void preYhi(double ****);
  void preZlo(double ****);
  void preZhi(double ****);
  void wall(double ****);

};

}

#endif
#endif

/* ERROR/WARNING messages:

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

E: Wall defined twice in fix wall/srd command

Self-explanatory.

E: Cannot use fix wall/srd in periodic dimension

Self-explanatory.

E: Cannot use fix wall/srd zlo/zhi for a 2d simulation

Self-explanatory.

E: Cannot use fix wall/srd without fix srd

Self-explanatory.

E: Variable name for fix wall/srd does not exist

Self-explanatory.

E: Variable for fix wall/srd is invalid style

Only equal-style variables can be used.

*/
