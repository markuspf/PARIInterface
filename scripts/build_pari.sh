#!/usr/bin/env bash

set -ex

if [ "$#" -ge 1 ]; then
    GAPDIR=$1
    shift
else
    GAPDIR=../..
fi


TOP=$PWD
cd $GAPDIR
GAPDIR=$PWD
cd $TOP

if [ ! -f "$GAPDIR/sysinfo.gap" ]; then
    echo "ERROR: could not locate GAP in directory $GAPDIR"
    exit 1
fi
. "$GAPDIR/sysinfo.gap"

echo "Found GAP in directory $GAPDIR"
echo "GAParch = $GAParch"

PARI_VERSION=pari-2.11.0
PARI_GIT_URL=https://pari.math.u-bordeaux.fr/git/pari.git
PARI_DOWNLOAD_URL="https://pari.math.u-bordeaux.fr/pub/pari/unix/${PARI_VERSION}.tar.gz"
PARI_BUILDDIR=extern
PARI_PREFIX="${PARI_BUILDDIR}/install"

mkdir -p $PARI_BUILDDIR
cd $PARI_BUILDDIR

if [[ $USE_GIT = yes ]] ; then
    git clone http://pari.math.u-bordeaux.fr/git/pari.git
    git checkout $PARI_VERSION
else
    wget -N $PARI_DOWNLOAD_URL
    tar xvzf ${PARI_VERSION}.tar.gz > /dev/null
    mv ${PARI_VERSION} pari
fi

cd pari
./Configure --prefix=$TOP/$PARI_PREFIX --graphic=none
make install

