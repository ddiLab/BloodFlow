/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fix_brownian.h"
#include "atom.h"
#include "comm.h"
#include "force.h"
#include "update.h"
#include "error.h"
#include "math.h"

using namespace LAMMPS_NS;
using namespace FixConst;

/* ---------------------------------------------------------------------- */

FixBROWNIAN::FixBROWNIAN(LAMMPS *lmp, int narg, char **arg) :
  Fix(lmp, narg, arg)
{
  if (narg < 7) error->all(FLERR,"Illegal fix langevin command");
  nevery = atoi(arg[3]);
  diffCoef = atof(arg[4]);
  seed = atoi(arg[5]);
  molFlag = 0;
  random = new RanMars(lmp,seed + comm->me);
  if ( strcmp(arg[6],"molecule")==0) molFlag = 1;
}

/* ---------------------------------------------------------------------- */
FixBROWNIAN::~FixBROWNIAN(){
  delete random;
}
/* ---------------------------------------------------------------------- */

int FixBROWNIAN::setmask()
{
  int mask = 0;
  mask |= INITIAL_INTEGRATE;
  //mask |= FINAL_INTEGRATE;
  //mask |= INITIAL_INTEGRATE_RESPA;
  //mask |= FINAL_INTEGRATE_RESPA;
  return mask;
}

/* ---------------------------------------------------------------------- */

void FixBROWNIAN::init()
{
  dtv = update->dt;
  dtf = 0.5 * update->dt * force->ftm2v;
  //diffCoef=sqrt(2*diffCoef*dtv*nevery);//dx=sqrt(2*D*dt)
  diffCoef=sqrt(24*diffCoef*dtv*nevery);//using uniform dist
  
  int nlocal = atom->nlocal;
  tagint *molecule = atom->molecule;
 
  int n = 0;
  int i;
  nmolecule = 0;
  for (i = 0; i < nlocal; i++){
    n = MAX(n,molecule[i]); 
  }
  MPI_Allreduce(&n,&nmolecule,1,MPI_INT,MPI_MAX,world);
  printf("nmolecule %d\n",nmolecule);
  //printf("max # of molecules: %d\n", nmolecule);
  

}

/* ----------------------------------------------------------------------
   allow for both per-type and per-atom mass
------------------------------------------------------------------------- */

void FixBROWNIAN::initial_integrate(int vflag)
{
  int molId;
  bigint ntimestep;
  ntimestep = update->ntimestep;
  // update nevery time step
  if (ntimestep % nevery == 0){
    // update v and x of atoms in group
    double **x = atom->x;
    //double **v = atom->v;
    //double **f = atom->f;
    //double *rmass = atom->rmass;
    //double *mass = atom->mass;
    //int *type = atom->type;
    tagint *molecule = atom->molecule;
    int *mask = atom->mask;
    int nlocal = atom->nlocal;
    double dx,dy,dz;
    if (igroup == atom->firstgroup) nlocal = atom->nfirst;
    if (molFlag){
      for (int j = 1; j< nmolecule+1; j++){
        //dx= diffCoef * random->gaussian();
        //dy= diffCoef * random->gaussian();
        //dz= diffCoef * random->gaussian();
        dx= diffCoef * (random->uniform()-0.5);
        dy= diffCoef * (random->uniform()-0.5);
        dz= diffCoef * (random->uniform()-0.5);
        for (int i = 0; i < nlocal; i++){
          if (mask[i] & groupbit) {
            molId = molecule[i];
            if (j == molId){
              x[i][0] +=dx; 
              x[i][1] +=dy;
              x[i][2] +=dz;
            }
          }
        }
      }
    }else{
      for (int i = 0; i < nlocal; i++)
        if (mask[i] & groupbit) {
          //dx= diffCoef * random->gaussian();
          //dy= diffCoef * random->gaussian();
          //dz= diffCoef * random->gaussian();
          dx= diffCoef * (random->uniform()-0.5);
          dy= diffCoef * (random->uniform()-0.5);
          dz= diffCoef * (random->uniform()-0.5);
          x[i][0] +=dx; 
          x[i][1] +=dy;
          x[i][2] +=dz;
        }
      }
    }
}

/* ---------------------------------------------------------------------- */
/*
void FixBROWNIAN::final_integrate()
{
  double dtfm;

  // update v of atoms in group

  double **v = atom->v;
  double **f = atom->f;
  double *rmass = atom->rmass;
  double *mass = atom->mass;
  int *type = atom->type;
  int *mask = atom->mask;
  int nlocal = atom->nlocal;
  if (igroup == atom->firstgroup) nlocal = atom->nfirst;

  if (rmass) {
    for (int i = 0; i < nlocal; i++)
      if (mask[i] & groupbit) {
        dtfm = dtf / rmass[i];
        v[i][0] += dtfm * f[i][0];
        v[i][1] += dtfm * f[i][1];
        v[i][2] += dtfm * f[i][2];
      }

  } else {
    for (int i = 0; i < nlocal; i++)
      if (mask[i] & groupbit) {
        dtfm = dtf / mass[type[i]];
        v[i][0] += dtfm * f[i][0];
        v[i][1] += dtfm * f[i][1];
        v[i][2] += dtfm * f[i][2];
      }
  }
}*/
/* ---------------------------------------------------------------------- */
/*
void FixBROWNIAN::initial_integrate_respa(int vflag, int ilevel, int iloop)
{
  dtv = step_respa[ilevel];
  dtf = 0.5 * step_respa[ilevel] * force->ftm2v;

  // innermost level - NVE update of v and x
  // all other levels - NVE update of v

  if (ilevel == 0) initial_integrate(vflag);
  else final_integrate();
}
*/
/* ---------------------------------------------------------------------- */
/*
void FixBROWNIAN::final_integrate_respa(int ilevel, int iloop)
{
  dtf = 0.5 * step_respa[ilevel] * force->ftm2v;
  final_integrate();
}*/

/* ---------------------------------------------------------------------- */

void FixBROWNIAN::reset_dt()
{
  dtv = update->dt;
  dtf = 0.5 * update->dt * force->ftm2v;
}
