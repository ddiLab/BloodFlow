FROM ubuntu:18.04

SHELL ["/bin/bash", "-c"]

# directory structure for the build
ENV BASEDIR=/embolism
ENV SRCDIR=$BASEDIR/src
ENV INSTALLDIR=$BASEDIR/install
ENV BUILDDIR=$BASEDIR/build

RUN mkdir $BASEDIR
RUN mkdir $SRCDIR
RUN mkdir $INSTALLDIR
RUN mkdir $BUILDDIR

# ubuntu packages
RUN apt-get update 
RUN apt-get install -y \
    git \
    build-essential \
    libmpich-dev \
    wget

# install cmake binary from Kitware
WORKDIR ${INSTALLDIR}
RUN wget https://github.com/Kitware/CMake/releases/download/v3.21.3/cmake-3.21.3-linux-x86_64.tar.gz && \
    tar xvzf cmake-3.21.3-linux-x86_64.tar.gz && \
    mv cmake-3.21.3-linux-x86_64 cmake && \
    rm cmake-3.21.3-linux-x86_64.tar.gz
ENV PATH=${INSTALLDIR}/cmake/bin:$PATH


###################################
###         Embolism            ###
###################################

#RUN git clone https://github.com/ddiLab/BloodFlow.git






###################################
###          Palabos            ###
###################################

WORKDIR ${SRCDIR}
RUN git clone https://gitlab.com/unigespc/palabos.git && \
    cd palabos && \
    git checkout e498e8ad7f24fd7ff87313670db7873703c1fd3f



###################################
###          LAMMPS             ###
###################################

WORKDIR ${SRCDIR}
RUN git clone https://github.com/lammps/lammps.git && \
    cd lammps && \
    git checkout e960674cea38515ae3749218c314a9e1a3c6c140




