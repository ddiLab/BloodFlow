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

#ifdef BOND_CLASS

BondStyle(harmonic/contract,BondHarmonicContract)

#else

#ifndef LMP_BOND_HARMONIC_CONTRACT_H
#define LMP_BOND_HARMONIC_CONTRACT_H

#include "stdio.h"
#include "bond.h"

namespace LAMMPS_NS {

class BondHarmonicContract : public Bond {
 public:
  BondHarmonicContract(class LAMMPS *);
  virtual ~BondHarmonicContract();
  void init_style();
  virtual void compute(int, int);
  void coeff(int, char **);
  double equilibrium_distance(int);
  void check_activation(bigint,bigint);
  void write_restart(FILE *);
  void read_restart(FILE *);
  void write_data(FILE *);
  double single(int, double, int, int, double &);

 protected:
  double *k,*r0,*v0,*ls;
  int nmolecule;
  int *nevery;
  int *ts;
  int *activated;

  virtual void allocate();
};

}

#endif
#endif

/* ERROR/WARNING messages:

E: Incorrect args for bond coefficients

Self-explanatory.  Check the input script or data file.

*/
