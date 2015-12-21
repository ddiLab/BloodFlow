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

#include "mpi.h"
#include "math.h"
#include "stdlib.h"
#include "angle_rbc.h"
#include "atom.h"
#include "neighbor.h"
#include "domain.h"
#include "comm.h"
#include "force.h"
#include "math_const.h"
#include "memory.h"
#include "error.h"

using namespace LAMMPS_NS;
using namespace MathConst;

#define SMALL 0.001

/* ---------------------------------------------------------------------- */

AngleRbc::AngleRbc(LAMMPS *lmp) : Angle(lmp) {
  At = Vt = NULL;
}

/* ---------------------------------------------------------------------- */

AngleRbc::~AngleRbc()
{
  if (allocated && !copymode) {
    memory->destroy(setflag);
    memory->destroy(Cq);
    memory->destroy(q);
    memory->destroy(ka);
    memory->destroy(A0t);
    memory->destroy(kv);
    memory->destroy(V0t);
    memory->destroy(kd);
    memory->destroy(A0);
  }
  if(nmolecule > 0 && !copymode){
    memory->destroy(At);
    memory->destroy(Vt);
  }
}

/* ---------------------------------------------------------------------- */
void AngleRbc::init_style()
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
  
  memory->create(At,nmolecule+1,"angle:At");
  memory->create(Vt,nmolecule+1,"angle:Vt");
}

/* ---------------------------------------------------------------------- */
void AngleRbc::computeAreaVol(double *At, double *Vt){
  for (int i = 1; i < nmolecule+1; i++) {
    At[i] = 0.0;
    Vt[i] = 0.0;
  }
  int i1,i2,i3,n,type,molId;
  double delx1,dely1,delz1,delx2,dely2,delz2,delx3,dely3,delz3;
  double xi[3],cnt[3],xi2;//
  double at,vt;
  double rsq1,rsq2,r1,r2;

  double **x = atom->x;
  double **f = atom->f;
  tagint *molecule = atom->molecule;
  int **anglelist = neighbor->anglelist;
  int nanglelist = neighbor->nanglelist;
  int nlocal = atom->nlocal;
  int newton_bond = force->newton_bond;
  
  // each processor has nanglelist, see neighbor.cpp
  // calculate area and volume
  for (n = 0; n < nanglelist; n++) {
    i1 = anglelist[n][0];
    i2 = anglelist[n][1];
    i3 = anglelist[n][2];
    type = anglelist[n][3];
    molId = molecule[i2];

    //if (molId < 0 || molId > nmolecule+1)  error->all(FLERR,"Wrong molecule ID");


    // 1st bond
    /*    /\3
     *   /  \
     *  /1___\2
     * dx_ij=x_i - x_j
     * */
    delx1 = x[i1][0] - x[i2][0];
    dely1 = x[i1][1] - x[i2][1];
    delz1 = x[i1][2] - x[i2][2];

    rsq1 = delx1*delx1 + dely1*dely1 + delz1*delz1;
    r1 = sqrt(rsq1);

    // 2nd bond
    delx2 = x[i3][0] - x[i2][0];
    dely2 = x[i3][1] - x[i2][1];
    delz2 = x[i3][2] - x[i2][2];

    rsq2 = delx2*delx2 + dely2*dely2 + delz2*delz2;
    r2 = sqrt(rsq2);

    // 3rd bond
    delx3 = x[i1][0] - x[i3][0];
    dely3 = x[i1][1] - x[i3][1];
    delz3 = x[i1][2] - x[i3][2];
   
    // norm xi=dx_2 x dx_1
    xi[0]=delz1*dely2 - dely1*delz2;
    xi[1]=delx1*delz2 - delz1*delx2;
    xi[2]=dely1*delx2 - delx1*dely2;
    
    xi2=xi[0]*xi[0] + xi[1]*xi[1] + xi[2]*xi[2];
    At[molId] += 0.5*sqrt(xi2);

    cnt[0]=(x[i1][0] + x[i2][0] + x[i3][0])/3.0;
    cnt[1]=(x[i1][1] + x[i2][1] + x[i3][1])/3.0;
    cnt[2]=(x[i1][2] + x[i2][2] + x[i3][2])/3.0;

    Vt[molId] += 1.0/6.0*(xi[0]*cnt[0] + xi[1]*cnt[1] + xi[2]*cnt[2]);
  }
  for (int i = 1; i < nmolecule+1; i++) {
    MPI_Allreduce(&At[i],&at,1,MPI_DOUBLE,MPI_SUM,world);
    MPI_Allreduce(&Vt[i],&vt,1,MPI_DOUBLE,MPI_SUM,world);
    At[i] = at;
    Vt[i] = vt;
  }
}
/* ---------------------------------------------------------------------- */

void AngleRbc::compute(int eflag, int vflag)
{
  int i1,i2,i3,n,type,molId;
  double delx1,dely1,delz1,delx2,dely2,delz2,delx3,dely3,delz3;
  double eangle,f1[3],f2[3],f3[3];
  double dA;
  double xi[3],cnt[3],xi2;//
  double a0,beta_a, beta_ad, beta_area, beta_v;

  eangle = 0.0;
  if (eflag || vflag) ev_setup(eflag,vflag);
  else evflag = 0;

  double **x = atom->x;
  double **f = atom->f;
  int **anglelist = neighbor->anglelist;
  int nanglelist = neighbor->nanglelist;
  tagint *molecule = atom->molecule;
  int nlocal = atom->nlocal;
  int newton_bond = force->newton_bond;
  
  // each processor has nanglelist, see neighbor.cpp
  a0=0.0;
  computeAreaVol(At, Vt);
 /* 
  int me = comm->me;
  if (me == 0){
    for (n=1;n<nmolecule+1;n++)
      printf("Area and vol of cell[%d] = %lg %lg\n",n,At[n],Vt[n] );
  }*/
  for (n = 0; n < nanglelist; n++) {
    i1 = anglelist[n][0];
    i2 = anglelist[n][1];
    i3 = anglelist[n][2];
    type = anglelist[n][3];
    molId = molecule[i2];
    // 1st bond 
    delx1 = x[i1][0] - x[i2][0];
    dely1 = x[i1][1] - x[i2][1];
    delz1 = x[i1][2] - x[i2][2];

    // 2nd bond
    delx2 = x[i3][0] - x[i2][0];
    dely2 = x[i3][1] - x[i2][1];
    delz2 = x[i3][2] - x[i2][2];

    // 3rd bond
    delx3 = x[i1][0] - x[i3][0];
    dely3 = x[i1][1] - x[i3][1];
    delz3 = x[i1][2] - x[i3][2];
   
    // norm xi=dx_2 x dx_1
    xi[0]=delz1*dely2 - dely1*delz2;
    xi[1]=delx1*delz2 - delz1*delx2;
    xi[2]=dely1*delx2 - delx1*dely2;
    
    xi2=xi[0]*xi[0] + xi[1]*xi[1] + xi[2]*xi[2];
    a0 = 0.5*sqrt(xi2);
    
    cnt[0]=(x[i1][0] + x[i2][0] + x[i3][0])/3.0;
    cnt[1]=(x[i1][1] + x[i2][1] + x[i3][1])/3.0;
    cnt[2]=(x[i1][2] + x[i2][2] + x[i3][2])/3.0;

    //global area
    beta_a = -0.25 * ka[type] * (At[molId] - A0t[type]) / (A0t[type] * a0);
    beta_ad = -0.25 * kd[type] * (a0 - A0[type])/(A0[type]*a0);
    beta_area = beta_a + beta_ad;
    // force & energy
    // xi x dx2
    f1[0] = beta_area*(xi[1]*delz2 - xi[2]*dely2);
    f1[1] = beta_area*(xi[2]*delx2 - xi[0]*delz2);
    f1[2] = beta_area*(xi[0]*dely2 - xi[1]*delx2);
    // xi x dx3
    f2[0] = beta_area*(xi[1]*delz3 - xi[2]*dely3);
    f2[1] = beta_area*(xi[2]*delx3 - xi[0]*delz3);
    f2[2] = beta_area*(xi[0]*dely3 - xi[1]*delx3);
    // xi x -dx1 = - (xi x dx1)
    f3[0] = -beta_area*(xi[1]*delz1 - xi[2]*dely1);
    f3[1] = -beta_area*(xi[2]*delx1 - xi[0]*delz1);
    f3[2] = -beta_area*(xi[0]*dely1 - xi[1]*delx1);
   
    beta_v = - 0.166666667 * kv[type] * (Vt[molId] - V0t[type]) / V0t[type];
    f1[0] += beta_v*(xi[0]/3.0 + cnt[1]*delz2 - cnt[2]*dely2);
    f1[1] += beta_v*(xi[1]/3.0 + cnt[2]*delx2 - cnt[0]*delz2);
    f1[2] += beta_v*(xi[2]/3.0 + cnt[0]*dely2 - cnt[1]*delx2);

    f2[0] += beta_v*(xi[0]/3.0 + cnt[1]*delz3 - cnt[2]*dely3);
    f2[1] += beta_v*(xi[1]/3.0 + cnt[2]*delx3 - cnt[0]*delz3);
    f2[2] += beta_v*(xi[2]/3.0 + cnt[0]*dely3 - cnt[1]*delx3);
    
    f3[0] += beta_v*(xi[0]/3.0 - (cnt[1]*delz1 - cnt[2]*dely1));
    f3[1] += beta_v*(xi[1]/3.0 - (cnt[2]*delx1 - cnt[0]*delz1));
    f3[2] += beta_v*(xi[2]/3.0 - (cnt[0]*dely1 - cnt[1]*delx1));

    // only local area conservation energy
    dA = a0 - A0[type];
    if (eflag) eangle = 0.5*kd[type]*dA*dA/A0[type];

    // apply force to each of 3 atoms

    if (newton_bond || i1 < nlocal) {
      f[i1][0] += f1[0];
      f[i1][1] += f1[1];
      f[i1][2] += f1[2];
    }

    if (newton_bond || i2 < nlocal) {
      f[i2][0] += f2[0];
      f[i2][1] += f2[1];
      f[i2][2] += f2[2];
    }

    if (newton_bond || i3 < nlocal) {
      f[i3][0] += f3[0];
      f[i3][1] += f3[1];
      f[i3][2] += f3[2];
    }
    // this may not be correct for angle_rbc style
    if (evflag) ev_tally(i1,i2,i3,nlocal,newton_bond,eangle,f1,f3,
                         delx1,dely1,delz1,delx2,dely2,delz2);
  }
}

/* ---------------------------------------------------------------------- */

void AngleRbc::allocate()
{
  allocated = 1;
  int n = atom->nangletypes;

  memory->create(Cq,n+1,"angle:cq");
  memory->create(q,n+1,"angle:q");
  memory->create(ka,n+1,"angle:ka");
  memory->create(A0t,n+1,"angle:a0t");
  memory->create(kv,n+1,"angle:kv");
  memory->create(V0t,n+1,"angle:v0t");
  memory->create(kd,n+1,"angle:kd");
  memory->create(A0,n+1,"angle:a0");

  memory->create(setflag,n+1,"angle:setflag");
  for (int i = 1; i <= n; i++) setflag[i] = 0;
}

/* ----------------------------------------------------------------------
   set coeffs for one or more types
------------------------------------------------------------------------- */

void AngleRbc::coeff(int narg, char **arg)
{
  if (narg != 9) error->all(FLERR,"Incorrect args for angle coefficients");
  if (!allocated) allocate();

  int ilo,ihi;
  force->bounds(arg[0],atom->nangletypes,ilo,ihi);

  double Cq_one = force->numeric(FLERR,arg[1]);
  double q_one = force->numeric(FLERR,arg[2]);
  double ka_one = force->numeric(FLERR,arg[3]);
  double A0t_one = force->numeric(FLERR,arg[4]);
  double kv_one = force->numeric(FLERR,arg[5]);
  double V0t_one = force->numeric(FLERR,arg[6]);
  double kd_one = force->numeric(FLERR,arg[7]);
  double A0_one = force->numeric(FLERR,arg[8]);

  // convert theta0 from degrees to radians

  int count = 0;
  for (int i = ilo; i <= ihi; i++) {
    Cq[i] = Cq_one;
    q[i] = q_one;
    ka[i] = ka_one;
    A0t[i] = A0t_one;
    kv[i] = kv_one;
    V0t[i] = V0t_one;
    kd[i] = kd_one;
    A0[i] = A0_one;
    setflag[i] = 1;
    count++;
  }

  if (count == 0) error->all(FLERR,"Incorrect args for angle coefficients");
}

/* ---------------------------------------------------------------------- */

double AngleRbc::equilibrium_angle(int i)
{
  return 0.0;// Not needed for Area/volume conservation
}

/* ----------------------------------------------------------------------
   proc 0 writes out coeffs to restart file
------------------------------------------------------------------------- */

void AngleRbc::write_restart(FILE *fp)
{
  fwrite(&Cq[1],sizeof(double),atom->nangletypes,fp);
  fwrite(&q[1],sizeof(double),atom->nangletypes,fp);
  fwrite(&ka[1],sizeof(double),atom->nangletypes,fp);
  fwrite(&A0t[1],sizeof(double),atom->nangletypes,fp);
  fwrite(&kv[1],sizeof(double),atom->nangletypes,fp);
  fwrite(&V0t[1],sizeof(double),atom->nangletypes,fp);
  fwrite(&kd[1],sizeof(double),atom->nangletypes,fp);
  fwrite(&A0[1],sizeof(double),atom->nangletypes,fp);
}

/* ----------------------------------------------------------------------
   proc 0 reads coeffs from restart file, bcasts them
------------------------------------------------------------------------- */

void AngleRbc::read_restart(FILE *fp)
{
  allocate();

  if (comm->me == 0) {
    fread(&Cq[1],sizeof(double),atom->nangletypes,fp);
    fread(&q[1],sizeof(double),atom->nangletypes,fp);
    fread(&ka[1],sizeof(double),atom->nangletypes,fp);
    fread(&A0t[1],sizeof(double),atom->nangletypes,fp);
    fread(&kv[1],sizeof(double),atom->nangletypes,fp);
    fread(&V0t[1],sizeof(double),atom->nangletypes,fp);
    fread(&kv[1],sizeof(double),atom->nangletypes,fp);
    fread(&A0[1],sizeof(double),atom->nangletypes,fp);
  }
  MPI_Bcast(&Cq[1],atom->nangletypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&q[1],atom->nangletypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&ka[1],atom->nangletypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&A0t[1],atom->nangletypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&kv[1],atom->nangletypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&V0t[1],atom->nangletypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&kd[1],atom->nangletypes,MPI_DOUBLE,0,world);
  MPI_Bcast(&A0[1],atom->nangletypes,MPI_DOUBLE,0,world);

  for (int i = 1; i <= atom->nangletypes; i++) setflag[i] = 1;
}

/* ----------------------------------------------------------------------
   proc 0 writes to data file
------------------------------------------------------------------------- */

void AngleRbc::write_data(FILE *fp)
{
  for (int i = 1; i <= atom->nangletypes; i++)
    fprintf(fp,"%d %g %g %g %g %g %g %g %g\n",i,Cq[i],q[i],
            ka[i],A0t[i],kv[i],V0t[i],kd[i],A0[i]);
}

/* ---------------------------------------------------------------------- */

double AngleRbc::single(int type, int i1, int i2, int i3)
{
  double **x = atom->x;
  double a0, xi[3], xi2;
  
  double delx1 = x[i1][0] - x[i2][0];
  double dely1 = x[i1][1] - x[i2][1];
  double delz1 = x[i1][2] - x[i2][2];
  domain->minimum_image(delx1,dely1,delz1);
  double r1 = sqrt(delx1*delx1 + dely1*dely1 + delz1*delz1);

  double delx2 = x[i3][0] - x[i2][0];
  double dely2 = x[i3][1] - x[i2][1];
  double delz2 = x[i3][2] - x[i2][2];
  domain->minimum_image(delx2,dely2,delz2);
  double r2 = sqrt(delx2*delx2 + dely2*dely2 + delz2*delz2);
  //area
  xi[0]=delz1*dely2 - dely1*delz2;
  xi[1]=delx1*delz2 - delz1*delx2;
  xi[2]=dely1*delx2 - delx1*dely2;
    
  xi2=xi[0]*xi[0] + xi[1]*xi[1] + xi[2]*xi[2];
  a0 = 0.5*sqrt(xi2);

  double dtheta = a0 - A0[type];
  double tk = 0.5 * kd[type] * dtheta;
  return tk*dtheta/A0[type]; 
}
