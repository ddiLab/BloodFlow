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

#include "math.h"
#include "stdlib.h"
#include "bond_harmonic_contract.h"
#include "atom.h"
#include "neighbor.h"
#include "domain.h"
#include "comm.h"
#include "force.h"
#include "memory.h"
#include "error.h"
#include "update.h"

using namespace LAMMPS_NS;

/* ---------------------------------------------------------------------- */

BondHarmonicContract::BondHarmonicContract(LAMMPS *lmp) : Bond(lmp) {
  ts = NULL; activated = NULL;
}

/* ---------------------------------------------------------------------- */

BondHarmonicContract::~BondHarmonicContract()
{
  if (allocated && !copymode) {
    memory->destroy(setflag);
    memory->destroy(k);
    memory->destroy(r0);
    memory->destroy(v0);
    memory->destroy(ls);
    memory->destroy(nevery);
    memory->destroy(e0);
    memory->destroy(c0);
  }
  if(nmolecule > 0 && !copymode){
    memory->destroy(ts);
    memory->destroy(activated);
  }
}

/* ---------------------------------------------------------------------- */
void BondHarmonicContract::init_style()
{
  int i,n;

  int nlocal = atom->nlocal;
  tagint *molecule = atom->molecule;
 
  n = 0;
  nmolecule = 0;
  for (i = 0; i < nlocal; i++){
    n = MAX(n,molecule[i]); 
  }
  MPI_Allreduce(&n,&nmolecule,1,MPI_INT,MPI_MAX,world);
  //printf("max # of molecules: %d\n", nmolecule);
  
  memory->create(ts,nmolecule+1,"Bond:ts");
  memory->create(activated,nmolecule+1,"Bond:activated");
  
  for (n = 1; n < nmolecule + 1; n++)
    activated[n] = 0;
}

/* ---------------------------------------------------------------------- */
void BondHarmonicContract::check_activation(bigint ntimestep, bigint actTS)
{
  int n;
  for (n = 1; n < nmolecule + 1; n++)
  {
    if (activated[n] == 0 && ntimestep > actTS ){
        activated[n] = 1;
        ts[n] = actTS;
    }
  }
}

/* ---------------------------------------------------------------------- */

void BondHarmonicContract::compute(int eflag, int vflag)
{
  int i1,i2,n,type;
  double delx,dely,delz,ebond,fbond;
  double rsq,r,dr,rk;
  
  int molID;
  bigint ntimestep;
  
  ebond = 0.0;
  if (eflag || vflag) ev_setup(eflag,vflag);
  else evflag = 0;

  double **x = atom->x;
  double **f = atom->f;
  int **bondlist = neighbor->bondlist;
  int nbondlist = neighbor->nbondlist;
  tagint *molecule = atom->molecule;
  int nlocal = atom->nlocal;
  int newton_bond = force->newton_bond;
  
  double dt = update->dt;
  double ls_now;
  ntimestep = update->ntimestep;
  check_activation(ntimestep,0);

  for (n = 0; n < nbondlist; n++) {
    i1 = bondlist[n][0];
    i2 = bondlist[n][1];
    type = bondlist[n][2];

    molID = molecule[i1];
    
    //if(activated[molID] && (ntimestep % nevery[type]== 0)){
    
    delx = x[i1][0] - x[i2][0];
    dely = x[i1][1] - x[i2][1];
    delz = x[i1][2] - x[i2][2];

    rsq = delx*delx + dely*dely + delz*delz;
    r = sqrt(rsq);
    

    dr = r - r0[type];
    if(activated[molID] && (ntimestep % nevery[type]== 0)){
      ls_now = (ntimestep - ts[molID])*dt;
      if (ls_now > ls[type] ) ls_now = ls[type];
      //dr = r - (r0[type] - v0[type]*ls_now);
      dr = r0[type]*e0[type]*(1-exp(c0[type]*ls_now));
    }
    rk = k[type] * dr;
    //if(comm->me==0) printf("r0=%g, ls_now=%g, dr=%g\n",r0[type],ls_now,dr);
    // force & energy

    if (r > 0.0) fbond = -2.0*rk/r;
    else fbond = 0.0;

    if (eflag) ebond = rk*dr;

    // apply force to each of 2 atoms

    if (newton_bond || i1 < nlocal) {
      f[i1][0] += delx*fbond;
      f[i1][1] += dely*fbond;
      f[i1][2] += delz*fbond;
    }

    if (newton_bond || i2 < nlocal) {
      f[i2][0] -= delx*fbond;
      f[i2][1] -= dely*fbond;
      f[i2][2] -= delz*fbond;
    }

    if (evflag) ev_tally(i1,i2,nlocal,newton_bond,ebond,fbond,delx,dely,delz);
  //}
  }
}

/* ---------------------------------------------------------------------- */

void BondHarmonicContract::allocate()
{
  allocated = 1;
  int n = atom->nbondtypes;

  memory->create(k,n+1,"bond:k");
  memory->create(r0,n+1,"bond:r0");
  memory->create(v0,n+1,"bond:v0");
  memory->create(ls,n+1,"bond:ls");
  memory->create(nevery,n+1,"bond:nevery");
  memory->create(e0,n+1,"bond:e0");
  memory->create(c0,n+1,"bond:c0");

  memory->create(setflag,n+1,"bond:setflag");
  for (int i = 1; i <= n; i++) setflag[i] = 0;
}

/* ----------------------------------------------------------------------
   set coeffs for one or more types
------------------------------------------------------------------------- */

void BondHarmonicContract::coeff(int narg, char **arg)
{
  if (narg != 8) error->all(FLERR,"Incorrect args for bond coefficients");
  if (!allocated) allocate();

  int ilo,ihi;
  force->bounds(arg[0],atom->nbondtypes,ilo,ihi);

  double k_one = force->numeric(FLERR,arg[1]);
  double r0_one = force->numeric(FLERR,arg[2]);
  double v0_one = force->numeric(FLERR,arg[3]);
  double ls_one = force->numeric(FLERR,arg[4]);
  double nevery_one = force->numeric(FLERR,arg[5]);
  double e0_one = force->numeric(FLERR,arg[6]);
  double c0_one = force->numeric(FLERR,arg[7]);

  int count = 0;
  for (int i = ilo; i <= ihi; i++) {
    k[i] = k_one;
    r0[i] = r0_one;
    v0[i] = v0_one;
    ls[i] = ls_one;
    nevery[i] = nevery_one;
    e0[i] = e0_one;
    c0[i] = c0_one;
    setflag[i] = 1;
    count++;
  }

  if (count == 0) error->all(FLERR,"Incorrect args for bond coefficients");
}

/* ----------------------------------------------------------------------
   return an equilbrium bond length
------------------------------------------------------------------------- */

double BondHarmonicContract::equilibrium_distance(int i)
{
  return r0[i];
}

/* ----------------------------------------------------------------------
   proc 0 writes out coeffs to restart file
------------------------------------------------------------------------- */

void BondHarmonicContract::write_restart(FILE *fp)
{
  fwrite(&k[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&r0[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&v0[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&ls[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&nevery[1],sizeof(int),atom->nbondtypes,fp);
  fwrite(&e0[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&c0[1],sizeof(double),atom->nbondtypes,fp);
}

/* ----------------------------------------------------------------------
   proc 0 reads coeffs from restart file, bcasts them
------------------------------------------------------------------------- */

void BondHarmonicContract::read_restart(FILE *fp)
{
  allocate();

  if (comm->me == 0) {
    fread(&k[1],sizeof(double),atom->nbondtypes,fp);
    fread(&r0[1],sizeof(double),atom->nbondtypes,fp);
    fread(&v0[1],sizeof(double),atom->nbondtypes,fp);
    fread(&ls[1],sizeof(double),atom->nbondtypes,fp);
    fread(&nevery[1],sizeof(int),atom->nbondtypes,fp);
    fread(&e0[1],sizeof(double),atom->nbondtypes,fp);
    fread(&c0[1],sizeof(double),atom->nbondtypes,fp);
  }
  MPI_Bcast(&k[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&r0[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&v0[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&ls[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&nevery[1],atom->nbondtypes,MPI_INT,0,world);
  MPI_Bcast(&e0[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&c0[1],atom->nbondtypes,MPI_DOUBLE,0,world);

  for (int i = 1; i <= atom->nbondtypes; i++) setflag[i] = 1;
}

/* ----------------------------------------------------------------------
   proc 0 writes to data file
------------------------------------------------------------------------- */

void BondHarmonicContract::write_data(FILE *fp)
{
  for (int i = 1; i <= atom->nbondtypes; i++)
    fprintf(fp,"%d %g %g %g %g %d %g %g\n",i,k[i],r0[i],v0[i],ls[i],nevery[i],e0[i],c0[i]);
}

/* ---------------------------------------------------------------------- */

double BondHarmonicContract::single(int type, double rsq, int i, int j,
                        double &fforce)
{/*
  double r = sqrt(rsq);
  double dr = r - r0[type];
  double rk = k[type] * dr;
  fforce = 0;
  if (r > 0.0) fforce = -2.0*rk/r;
  return rk*dr;*/
  return 0.0;
}
