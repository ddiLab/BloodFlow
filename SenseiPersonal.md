# Building SENSEI on a personnal computer

1. Clone the repo in the home directory: `git clone https://gitlab.kitware.com/sensei/sensei.git src`

2. If you don't have cmake, ccmake, and make, install them.

3. SENSEI has some dependencies on ParaView, so we will need to install the ParaView (specifically v5.9.1).

   * Clone ParaView superbuild repo with: \
   ``

   * configure with cmake: NOTE: I used ccmake to view all configuration options, it might be helpful due to the ParaView repo updating versions (5.10.0 was available to me). \
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



