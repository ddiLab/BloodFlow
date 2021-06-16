/* This file was created by Connor Murphy

Visualization of a red blood cell flowing in a cuboid domain containing a micropost.

*/
#include "palabos3D.h"
#include "palabos3D.hh"

#include "ibm3D.h" //coupling code that needs to be included
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>

#include "mpi.h"
#include "lammps.h"
#include "input.h"
#include "library.h"
#include "lammpsWrapper.h"

#include "latticeDecomposition.h"
//#include "nearestTwoNeighborLattices3D.h"

using namespace plb;
using namespace std;

typedef double T;
//#define DESCRIPTOR descriptors::ForcedN2D3Q19Descriptor  
#define DESCRIPTOR descriptors::ForcedD3Q19Descriptor  //descriptor for the BGK dynamic model
//#define DYNAMICS BGKdynamics<T, DESCRIPTOR>(parameters.getOmega())
#define DYNAMICS GuoExternalForceBGKdynamics<T, DESCRIPTOR>(parameters.getOmega())//BGK model with ability to implement external forces on the fluid (needed for the blood cells)

#define NMAX 150

const T pi = (T)4.*std::atan((T)1.);

static T poiseuillePressure(IncomprFlowParam<T> const &parameters, plint maxN){
    const T a = parameters.getNx()-1;
    const T b = parameters.getNy()-1;//Ny and Nx give the amount of nodes in the x and y direction. The dimension is one less then these values

    const T nu = parameters.getLatticeNu();
    const T uMax = parameters.getLatticeU();

    T sum = T();
    for (plint iN = 0; iN < maxN; iN += 2){
        T twoNplusOne = (T)2*(T)iN+(T)1;
        sum += ((T)1 / (std::pow(twoNplusOne,(T)3)*std::cosh(twoNplusOne*pi*b/((T)2*a))));
    }
    for (plint iN = 1; iN < maxN; iN += 2){
        T twoNplusOne = (T)2*(T)iN+(T)1;
        sum -= ((T)1 / (std::pow(twoNplusOne,(T)3)*std::cosh(twoNplusOne*pi*b/((T)2*a))));
    }

    T alpha = -(T)8 * uMax * pi * pi * pi / (a*a*(pi*pi*pi-(T)32*sum)); // alpha = -dp/dz / mu
    T deltaP = - (alpha * nu);
    return deltaP;
}

T poiseuilleVelocity(plint iX, plint iY, IncomprFlowParam<T> const& parameters, plint maxN){
    const T a = parameters.getNx()-1;
    const T b = parameters.getNy()-1;

    const T x = (T)iX - a / (T)2;
    const T y = (T)iY - b / (T)2;

    const T alpha = - poiseuillePressure(parameters,maxN) / parameters.getLatticeNu();

    T sum = T();

    for (plint iN = 0; iN < maxN; iN += 2){
        T twoNplusOne = (T)2*(T)iN+(T)1;
        sum += (std::cos(twoNplusOne*pi*x/a)*std::cosh(twoNplusOne*pi*y/a)
             / ( std::pow(twoNplusOne,(T)3)*std::cosh(twoNplusOne*pi*b/((T)2*a)) ));
    }
    for (plint iN = 1; iN < maxN; iN += 2){
        T twoNplusOne = (T)2*(T)iN+(T)1;
        sum -= (std::cos(twoNplusOne*pi*x/a)*std::cosh(twoNplusOne*pi*y/a)
             / ( std::pow(twoNplusOne,(T)3)*std::cosh(twoNplusOne*pi*b/((T)2*a)) ));
    }

    sum *= ((T)4 * alpha * a *a /std::pow(pi,(T)3));
    sum += (alpha / (T)2 * (x * x - a*a / (T)4));
    
    return sum;
}

template <typename T>
class SquarePoiseuilleDensityAndVelocity {
public:
    SquarePoiseuilleDensityAndVelocity(IncomprFlowParam<T> const& parameters_, plint maxN_)
        : parameters(parameters_),
          maxN(maxN_)
    { }
    void operator()(plint iX, plint iY, plint iZ, T &rho, Array<T,3>& u) const {
        rho = (T)1;
        u[0] = T();
        u[1] = T();
        u[2] = poiseuilleVelocity(iX, iY, parameters, maxN);
    }
private:
    IncomprFlowParam<T> parameters;
    plint maxN;
};

template <typename T>
class SquarePoiseuilleVelocity {
public:
    SquarePoiseuilleVelocity(IncomprFlowParam<T> const& parameters_, plint maxN_)
        : parameters(parameters_),
          maxN(maxN_)
    { }
    void operator()(plint iX, plint iY, plint iZ, Array<T,3>& u) const  {
        u[0] = T();
        u[1] = T();
        u[2] = poiseuilleVelocity(iX, iY, parameters, maxN);
    }
private:
    IncomprFlowParam<T> parameters;
    plint maxN;
};

template <typename T>
class ShearTopVelocity {
public:
    ShearTopVelocity(IncomprFlowParam<T> const& parameters_, plint maxN_)
        : parameters(parameters_),
          maxN(maxN_)
    { }
    void operator()(plint iX, plint iY, plint iZ, Array<T,3>& u) const  {
        u[0] = T(); 
        u[1] = T(); 
        u[2] = parameters.getLatticeU();
    }
private:
    IncomprFlowParam<T> parameters;
    plint maxN;
};

template <typename T>
class ShearBottomVelocity {
public:
    ShearBottomVelocity(IncomprFlowParam<T> const& parameters_, plint maxN_)
        : parameters(parameters_),
          maxN(maxN_)
    { }
    void operator()(plint iX, plint iY, plint iZ, Array<T,3>& u) const  {
        u[0] = T();
        u[1] = T();
        u[2] = T();
    }
private:
    IncomprFlowParam<T> parameters;
    plint maxN;
};

template <typename T> //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class BounceBackNodes : public DomainFunctional3D {
public: 
    BounceBackNodes(plb::plint cx_, plb::plint cy_, plb::plint radius)
        : cx(cx_),
          cy(cy_),
          radiusSqr(plb::util::sqr(radius))
    { }
    virtual bool operator() (plb::plint iX, plb::plint iY, plb::plint iZ) const {
        return plb::util::sqr(iX-cx) + plb::util::sqr(iY-cy) >= radiusSqr;
    }
    virtual BounceBackNodes<T>* clone() const {
        return new BounceBackNodes<T>(*this);
    }
private:
    plb::plint cx;
    plb::plint cy;
    plb::plint radiusSqr;
};


void squarePoiseuilleSetup( MultiBlockLattice3D<T,DESCRIPTOR>& lattice,
                            IncomprFlowParam<T> const& parameters,
                            OnLatticeBoundaryCondition3D<T,DESCRIPTOR>& boundaryCondition )
{
    const plint nx = parameters.getNx();
    const plint ny = parameters.getNy();
    const plint nz = parameters.getNz();
    Box3D top    = Box3D(0,    nx-1, ny-1, ny-1, 0, nz-1);
    Box3D bottom = Box3D(0,    nx-1, 0,    0,    0, nz-1);
    
    //Box3D inlet    = Box3D(0,    nx-1, 1,    ny-2, 0,    0);
    //Box3D outlet = Box3D(0,    nx-1, 1,    ny-2, nz-1, nz-1);
    
    Box3D left   = Box3D(0,    0,    1,    ny-2, 1, nz-2);
    Box3D right  = Box3D(nx-1, nx-1, 1,    ny-2, 1, nz-2);
    // shear flow top bottom surface
    /*
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, inlet, boundary::outflow );
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, outlet, boundary::outflow );

    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, top );
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, bottom );
    
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, left, boundary::outflow );
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, right, boundary::outflow );
    
    setBoundaryVelocity(lattice, top, ShearTopVelocity<T>(parameters,NMAX));
    setBoundaryVelocity(lattice, bottom, ShearBottomVelocity<T>(parameters,NMAX));
    
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, inlet, boundary::outflow );
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, outlet, boundary::outflow );
    */
    // channel flow
    //boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, inlet);
    //boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, outlet);
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, top );
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, bottom );
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, left );
    boundaryCondition.setVelocityConditionOnBlockBoundaries ( lattice, right );
    
    //setBoundaryVelocity(lattice, inlet, SquarePoiseuilleVelocity<T>(parameters, NMAX));
    //setBoundaryVelocity(lattice, outlet, SquarePoiseuilleVelocity<T>(parameters, NMAX));
    
    setBoundaryVelocity(lattice, top, Array<T,3>((T)0.0,(T)0.0,(T)0.0));
    setBoundaryVelocity(lattice, bottom, Array<T,3>((T)0.0,(T)0.0,(T)0.0));
    setBoundaryVelocity(lattice, left, Array<T,3>((T)0.0,(T)0.0,(T)0.0));
    setBoundaryVelocity(lattice, right, Array<T,3>((T)0.0,(T)0.0,(T)0.0));
    
    plint cx    = nx/2;
    plint cy    = ny/2; 
                          
    plint radius = nx/2;//parameters.getResolution() / 2;

    //initializeAtEquilibrium(lattice, lattice.getBoundingBox(), SquarePoiseuilleDensityAndVelocity<T>(parameters, NMAX));
   
    initializeAtEquilibrium(lattice, lattice.getBoundingBox(),(T)1.0, Array<T,3>(0.0,0.0,0.0));

    defineDynamics(lattice, lattice.getBoundingBox(),
                   new BounceBackNodes<T>(cx, cy, radius),
                   new BounceBack<T,DESCRIPTOR>);


    lattice.initialize();//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}

T computeRMSerror ( MultiBlockLattice3D<T,DESCRIPTOR>& lattice,
                    IncomprFlowParam<T> const& parameters )
{
    MultiTensorField3D<T,3> analyticalVelocity(lattice);
    setToFunction( analyticalVelocity, analyticalVelocity.getBoundingBox(),
                   SquarePoiseuilleVelocity<T>(parameters, NMAX) );
    MultiTensorField3D<T,3> numericalVelocity(lattice);
    computeVelocity(lattice, numericalVelocity, lattice.getBoundingBox());

           // Divide by lattice velocity to normalize the error
    return 1./parameters.getLatticeU() *
           // Compute RMS difference between analytical and numerical solution
           std::sqrt( computeAverage( *computeNormSqr(
                          *subtract(analyticalVelocity, numericalVelocity)
                     ) ) );
}

void writeVTK(MultiBlockLattice3D<T,DESCRIPTOR>& lattice,
              IncomprFlowParam<T> const& parameters, plint iter)
{
    T dx = parameters.getDeltaX();
    T dt = parameters.getDeltaT();
    VtkImageOutput3D<T> vtkOut(createFileName("vtk", iter, 6), dx);
    vtkOut.writeData<float>(*computeVelocityNorm(lattice), "velocityNorm", dx/dt);
    vtkOut.writeData<3,float>(*computeVelocity(lattice), "velocity", dx/dt);
    vtkOut.writeData<3,float>(*computeVorticity(*computeVelocity(lattice)), "vorticity", 1./dt);
}
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Not all of the classes and functions above were used. It is vital one looks carefully through 
the main() script.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


int main(int argc, char* argv[]) {

    plbInit(&argc, &argv);
    global::directories().setOutputDir("./tmp/");//Need these for every palabos code (I think)
/*
    if (argc != 2) {
        pcout << "Error the parameters are wrong. The structure must be :\n";
        pcout << "1 : N\n";
        exit(1);
    }*/

    //const plint N = atoi(argv[1]);
    const plint N = 1;// atoi(argv[1]);  //With N=1, the number of nodes in any direction is the dimension+1
    const T Re = 5e-3;
    //const plint Nref = 50;
    //const T uMaxRef = 0.01;
    const T uMax = 0.00075;//uMaxRef /(T)N * (T)Nref; // Needed to avoid compressibility errors.

    IncomprFlowParam<T> parameters(  //class found in palabos/src/core/units.h
            uMax,
            Re,
            N,
            30.,        // lx      //These are the dimensions of the simulation in dimensionless units
            30.,        // ly
            40.         // lz
    );
    const T maxT    =100 ;//6.6e4; //(T)0.01;  // maximum time duration
    plint iSave =10;//2000;//10; //This value indicated how often the code will save a vtk file
    plint iCheck = 10*iSave;
    writeLogFile(parameters, "3D square Poiseuille"); //A function defined in palabos/src/core/units.h (Logs the parameters and other values calculated by them)

    LammpsWrapper wrapper(argv,global::mpi().getGlobalCommunicator()); //Uses mpi to communicate with Lammps somehow. This class is found in BloodFlow/ibm/lammpsWrapper.h ??????????
    char * inlmp = argv[1];//??????
    wrapper.execFile(inlmp);//Just a void function that appears to do nothing in lammpsWrapper
   
    //MultiTensorField3D<T,3> vel(parameters.getNx(),parameters.getNy(),parameters.getNz());
    pcout<<"Nx,Ny,Nz "<<parameters.getNx()<<" "<<parameters.getNy()<<" "<<parameters.getNz()<<endl; //This line just outputs how many nodes in each dimension for user reference
    LatticeDecomposition lDec(parameters.getNx(),parameters.getNy(),parameters.getNz(), //Part of the ibm coupling code. What does it do????????????? Located in BloodFlow/ibm/latticeDecomposition.h
                              wrapper.lmp);
    SparseBlockStructure3D blockStructure = lDec.getBlockDistribution(); //Found in palabos/src/multiBlock/sparseBlockStructure3D.h
    ExplicitThreadAttribution* threadAttribution = lDec.getThreadAttribution();
    plint envelopeWidth = 4;

    MultiBlockLattice3D<T, DESCRIPTOR>        //A complex data structure???????????????????
      lattice (MultiBlockManagement3D (blockStructure, threadAttribution, envelopeWidth ),
               defaultMultiBlockPolicy3D().getBlockCommunicator(),
               defaultMultiBlockPolicy3D().getCombinedStatistics(),
               defaultMultiBlockPolicy3D().getMultiCellAccess<T,DESCRIPTOR>(),
               new DYNAMICS );
    
    //Cell<T,DESCRIPTOR> &cell = lattice.get(550,5500,550);
    pcout<<"dx "<<parameters.getDeltaX()<<" dt  "<<parameters.getDeltaT()<<" tau "<<parameters.getTau()<<endl; //outputing values for user reference (Tau is relaxation time)
    //pcout<<"51 works"<<endl;

/*
    MultiBlockLattice3D<T, DESCRIPTOR> lattice (
        parameters.getNx(), parameters.getNy(), parameters.getNz(), 
        new DYNAMICS );*/

    // Use periodic boundary conditions.
    lattice.periodicity().toggle(2,true);

    OnLatticeBoundaryCondition3D<T,DESCRIPTOR>* boundaryCondition
        = createLocalBoundaryCondition3D<T,DESCRIPTOR>();

    squarePoiseuilleSetup(lattice, parameters, *boundaryCondition);//function defined earlier in this script. Sets up the domain of the simulation

    // Loop over main time iteration.
    util::ValueTracer<T> converge(parameters.getLatticeU(),parameters.getResolution(),1.0e-3);
      //coupling between lammps and palabos
    /// Check time-convergence of a scalar.
/** This class is useful, for example to check convergence of
 * the velocity field for the simulation of a stationary flow.
 * Convergence is claimed when the standard deviation of the
 * monitored value is smaller than epsilon times the average.
 * The statistics are taken over a macroscopic time scale of the
 * system.???????????????????????????? this was description for ValueTracer
 */
    Array<T,3> force(0,0.,3e-7);//Force applied on fluid nodes
    setExternalVector(lattice,lattice.getBoundingBox(),DESCRIPTOR<T>::ExternalField::forceBeginsAt,force);//I think this spreads the force of the solid to neighboring fluid nodes??????????????
    
    for (plint iT=0;iT<4e3;iT++){
        lattice.collideAndStream();//collideAndStream is used in the MultiBlockLattice3D class (lattice is an instance of this class)
    }
    T timeduration = T(); //initialization of the timeduration variable
    global::timer("mainloop").start();//a timer that will eventually assign the end time value to timeduration
    for (plint iT=0; iT<maxT; ++iT) {
    //for (plint iT=0; iT<2; ++iT) {
        if (iT%iSave ==0 && iT >0){
            pcout<<"Saving VTK file..."<<endl;
            writeVTK(lattice, parameters, iT); //If statement saves a VTK file every iSave'th iteration
        }
        if (iT%iCheck ==0 && iT >0){
            pcout<<"Timestep "<<iT<<" Saving checkPoint file..."<<endl;
            saveBinaryBlock(lattice,"checkpoint.dat");
        }
        // lammps to calculate force
        wrapper.execCommand("run 1 pre no post no");
        // Clear and spread fluid force 
        
        setExternalVector(lattice,lattice.getBoundingBox(),DESCRIPTOR<T>::ExternalField::forceBeginsAt,force);//I think this spreads the force of the solid to neighboring fluid nodes??????????????
        ////-----classical ibm coupling-------------//
        spreadForce3D(lattice,wrapper);
        ////// Lattice Boltzmann iteration step.
        lattice.collideAndStream();
        ////// Interpolate and update solid position
        interpolateVelocity3D(lattice,wrapper);
        //-----force FSI ibm coupling-------------//
        //forceCoupling3D(lattice,wrapper);//located in BloodFlow/ibm/ibm3D.h and ibm3D.hh
        //lattice.collideAndStream();
    }

    timeduration = global::timer("mainloop").stop();
    pcout<<"total execution time "<<timeduration<<endl;
    delete boundaryCondition;
}
