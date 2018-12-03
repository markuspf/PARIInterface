#
# PARIInterface: Interface to PARI
#
# Declarations
#

_PARIINTERFACE_INITIALISED := false;
_PARIINTERFACE_STACK_DEFAULT := 8 * 1024 * 1024;
_PARIINTERFACE_STACKMAX_DEFAULT := 1024 * 1024 *1024;


#! @Chapter PARIInterface
#!
#! A C interface to the PARI number theory library
DeclareCategory( "IsPARIObj",  IsObject );
BindGlobal( "PARI_GEN_Family", NewFamily("PARIGENFamily"));
BindGlobal( "PARI_GEN_Type", NewType(PARI_GEN_Family, IsPARIObj) );

#! @Description
#!   Turn an univariate polynomial into a PARI polynomial
DeclareGlobalFunction( "PARIInitialise" );
DeclareGlobalFunction( "PARIClose" );
DeclareGlobalFunction( "PARIPolynomial" );
DeclareGlobalFunction( "PARIGaloisGroup" );
