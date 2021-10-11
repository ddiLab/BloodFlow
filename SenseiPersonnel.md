# Building SENSEI on a personnal computer

1. Clone the repo in the home directory: `git clone https://gitlab.kitware.com/sensei/sensei.git src`

2. If you don't have cmake, ccmake, and make, install them.

3. SENSEI has some dependencies on ParaView, so we will need to install the ParaView superbuild.

   * Clone ParaView superbuild repo with: \
   `git clone --recursive https://gitlab.kitware.com/paraview/paraview-superbuild.git src`

   * configure with cmake: NOTE: I used ccmake to view all configuration options, it might be helpful due to the ParaView repo updating versions (5.10.0 was available to me). \
 
`cmake -B ./build -S ./src \
  -DCMAKE_BUILD_TYPE_paraview=Release \
  -DCMAKE_INSTALL_PREFIX="/home/mectro/pvtest/" \
  -DENABLE_cxx11=ON \
  -Dparaview_SOURCE_SELECTION:STRING="5.10.0" \
  -DENABLE_zfp:BOOL=OFF \
  -DCMAKE_BUILD_TYPE:STRING=Release \
  -DENABLE_netcdf:BOOL=OFF \
  -DENABLE_visitbridge:BOOL=ON \
  -DENABLE_ffmpeg:BOOL=OFF \
  -DENABLE_qt5:BOOL=OFF \
  -DENABLE_mpi:BOOL=ON \
  -DUSE_SYSTEM_mpi:BOOL=ON \
  -DENABLE_paraview:BOOL=ON \
  -DENABLE_paraviewsdk:BOOL=ON \
  -DENABLE_xdmf3:BOOL=OFF \
  -DBUILD_SHARED_LIBS:BOOL=ON \
  -DENABLE_vtkm:BOOL=ON \
  -DENABLE_python3:BOOL=ON \
  -DUSE_SYSTEM_python3:BOOL=OFF \
  -DENABLE_pybind11:BOOL=OFF \
  -DENABLE_numpy:BOOL=ON \
  -DUSE_SYSTEM_numpy:BOOL=OFF \
  -DENABLE_scipy:BOOL=ON \
  -DENABLE_matplotlib:BOOL=ON \
  -DENABLE_vrpn:BOOL=OFF \
  -DENABLE_cosmotools:BOOL=OFF \
  -DENABLE_osmesa:BOOL=ON \
  -Dmesa_USE_SWR:BOOL=ON \
  -DENABLE_tbb:BOOL=ON \
  -DENABLE_silo:BOOL=ON \
  -DENABLE_boost:BOOL=OFF \
  -DENABLE_vortexfinder2:BOOL=OFF \
  -DENABLE_las:BOOL=OFF \
  -DBUILD_TESTING:BOOL=OFF \
  -DENABLE_adios2:BOOL=ON \
  -DENABLE_ospray:BOOL=ON \
  -DENABLE_ospraymodulempi:BOOL=ON \
  -DENABLE_fontconfig:BOOL=ON \
  -DENABLE_bzip2:BOOL=ON \
  -DPARAVIEW_BUILD_EDITION:STRING=CANONICAL \
  -DSUPERBUILD_DEFAULT_INSTALL:STRING=paraviewsdk/TGZ `

   * A directory named build should now exist: \
   `cd build` \
   `make -j12 download-all`

   * Build:\
   `make -j12`

   * Install: NOTE: I had to install LEX and YACK (Flex and Bison) before installing: \
   `sudo apt-get install bison flex` \
   `make install`

4. configure Sensei using cmake: *Here my command:
`cmake -B ./buildsensei -S ./SENSEI -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_INSTALL_PREFIX=./install -DENABLE_SENSEI=ON -DENABLE_VTK_IO=ON -DENABLE_CATALYST=ON -DParaView_DIR="/home/mectro/build/install/lib/cmake/paraview-5.10/"`

5. Build Sensei: \
`cd buildsensei`
`make -j12`



