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
#include "bond_hill.h"
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

BondHill::BondHill(LAMMPS *lmp) : Bond(lmp) {
  ts = NULL; activated = NULL;
}

/* ---------------------------------------------------------------------- */
void BondHill::init_style()
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

BondHill::~BondHill()
{
  if (allocated && !copymode) {
    memory->destroy(setflag);
    memory->destroy(F0);
    memory->destroy(V0);
    memory->destroy(Ha);
    memory->destroy(Hb);
    memory->destroy(ls);
  }
  if(nmolecule > 0 && !copymode){
    memory->destroy(ts);
    memory->destroy(activated);
  }
}

/* ---------------------------------------------------------------------- */
void BondHill::check_activation(bigint ntimestep, bigint actTS)
{
  int n;
  for (n = 1; n < nmolecule + 1; n++)
  {
    if (activated[n] == 0 && ntimestep > actTS ){
        activated[n] = 1;
        ts[n] = actTs; 
    }
  }
}
/* ---------------------------------------------------------------------- */

void BondHill::compute(int eflag, int vflag)
{
  int i1,i2,n,type,molID;
  double delx,dely,delz,dv[3],vshort,xdotv,tmp,ebond,fbond;
  double rsq,r;
  double dt;
  bigint ntimestep;

  ebond = 0.0;
  if (eflag || vflag) ev_setup(eflag,vflag);
  else evflag = 0;

  double **x = atom->x;
  double **v = atom->v;
  double **f = atom->f;
  int **bondlist = neighbor->bondlist;
  int nbondlist = neighbor->nbondlist;
  tagint *molecule = atom->molecule;
  tagint glb_id;
  tagint *tag = atom->tag;
  int nlocal = atom->nlocal;
  int newton_bond = force->newton_bond;
    
  dt = update->dt;
  ntimestep = update->ntimestep;
  check_activation(ntimestep,0);

  for (n = 0; n < nbondlist; n++) {
    i1 = bondlist[n][0];
    i2 = bondlist[n][1];
    type = bondlist[n][2];
    molID = molecule[i1];
    glb_id = tag[i1];

    if (activated[molID]){
      delx = x[i1][0] - x[i2][0];
      dely = x[i1][1] - x[i2][1];
      delz = x[i1][2] - x[i2][2];
      
      dv[0] = v[i1][0] - v[i2][0];
      dv[1] = v[i1][1] - v[i2][1];
      dv[2] = v[i1][2] - v[i2][2];

      xdotv = delx*dv[0] + dely*dv[1] + delz*dv[2];
      rsq = delx*delx + dely*dely + delz*delz;
      r = sqrt(rsq);
      
      if (r > 0.0){
        if (xdotv < 0.0){ //contraction
          vshort = -xdotv/r;
          if (vshort > V0[type]){
            fbond = 0.0;
            //vshort = V0[type];
            tmp = V0[type]/r;
            v[i1][0] = -delx*tmp;
            v[i1][1] = -dely*tmp;
            v[i1][2] = -delz*tmp;
            v[i2][0] = delx*tmp;
            v[i2][1] = dely*tmp;
            v[i2][2] = delz*tmp;
          }else{
            fbond = -(Hb[type]*(Ha[type]+F0[type])/(vshort + Hb[type]) - Ha[type])/r;
          }
        }else{
          fbond = -F0[type]/r;
        }
      }
      //if (comm->me ==0) printf("cell %d force %g v=%g \n",molID,fbond,vshort);
      if (eflag) ebond = Hb[type]*(Ha[type] + F0[type])*dt; 
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
      }
  }
}

/* ---------------------------------------------------------------------- */

void BondHill::allocate()
{
  allocated = 1;
  int n = atom->nbondtypes;

  memory->create(F0,n+1,"bond:F0");
  memory->create(V0,n+1,"bond:V0");
  memory->create(Ha,n+1,"bond:Ha");
  memory->create(Hb,n+1,"bond:Hb");
  memory->create(ls,n+1,"bond:ls");

  memory->create(setflag,n+1,"bond:setflag");
  for (int i = 1; i <= n; i++) setflag[i] = 0;
}

/* ----------------------------------------------------------------------
   set coeffs for one or more types
------------------------------------------------------------------------- */

void BondHill::coeff(int narg, char **arg)
{
  if (narg != 6) error->all(FLERR,"Incorrect args for bond coefficients");
  if (!allocated) allocate();

  int ilo,ihi;
  force->bounds(arg[0],atom->nbondtypes,ilo,ihi);

  double F0_one = force->numeric(FLERR,arg[1]);
  double V0_one = force->numeric(FLERR,arg[2]);
  double Ha_one = force->numeric(FLERR,arg[3]);
  double Hb_one = force->numeric(FLERR,arg[4]);
  double ls_one = force->numeric(FLERR,arg[5]);

  int count = 0;
  for (int i = ilo; i <= ihi; i++) {
    F0[i] = F0_one;
    V0[i] = V0_one;
    Ha[i] = Ha_one;
    Hb[i] = Hb_one;
    ls[i] = ls_one;
    setflag[i] = 1;
    count++;
  }

  if (count == 0) error->all(FLERR,"Incorrect args for bond coefficients");
}

/* ----------------------------------------------------------------------
   return an equilbrium bond length
------------------------------------------------------------------------- */

double BondHill::equilibrium_distance(int i)
{
  return 0.0;
}

/* ----------------------------------------------------------------------
   proc 0 writes out coeffs to restart file
------------------------------------------------------------------------- */

void BondHill::write_restart(FILE *fp)
{
  fwrite(&F0[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&V0[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&Ha[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&Hb[1],sizeof(double),atom->nbondtypes,fp);
  fwrite(&ls[1],sizeof(double),atom->nbondtypes,fp);
}

/* ----------------------------------------------------------------------
   proc 0 reads coeffs from restart file, bcasts them
------------------------------------------------------------------------- */

void BondHill::read_restart(FILE *fp)
{
  allocate();

  if (comm->me == 0) {
    fread(&F0[1],sizeof(double),atom->nbondtypes,fp);
    fread(&V0[1],sizeof(double),atom->nbondtypes,fp);
    fread(&Ha[1],sizeof(double),atom->nbondtypes,fp);
    fread(&Hb[1],sizeof(double),atom->nbondtypes,fp);
    fread(&ls[1],sizeof(double),atom->nbondtypes,fp);
  }
  MPI_Bcast(&F0[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&V0[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&Ha[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&Hb[1],atom->nbondtypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&ls[1],atom->nbondtypes,MPI_DOUBLE,0,world);

  for (int i = 1; i <= atom->nbondtypes; i++) setflag[i] = 1;
}

/* ----------------------------------------------------------------------
   proc 0 writes to data file
------------------------------------------------------------------------- */

void BondHill::write_data(FILE *fp)
{
  for (int i = 1; i <= atom->nbondtypes; i++)
    fprintf(fp,"%d %g %g %g %g %g\n",i,F0[i],V0[i],Ha[i],Hb[i],ls[i]);
}

/* ---------------------------------------------------------------------- */

double BondHill::single(int type, double rsq, int i, int j,
                        double &fforce)
{/*
  double r = sqrt(rsq);
  double lm = r0[type]/a0[type];
  double a = r/lm;//ratio
  double a2 = a*a;
  double a3 = a2*a;
  double a4 = (1-a)*(1-a);

  fforce = 0;
  if (r > 0.0) fforce = (-k[type]*(0.25/a4 - 0.25 + a) + kp[type]/rsq)/r;
  return 0.25*k[type]*lm*(3*a2 - 2*a3)/(1 - a) + kp[type]/r;*/
  return 0.0;
}
