#
# PARIInterface: Interface to PARI
#
# This file runs package tests. It is also referenced in the package
# metadata in PackageInfo.g.
#
LoadPackage( "PARIInterface" );

TestDirectory(DirectoriesPackageLibrary( "PARIInterface", "tst" ),
  rec(exitGAP := true));

FORCE_QUIT_GAP(1); # if we ever get here, there was an error
