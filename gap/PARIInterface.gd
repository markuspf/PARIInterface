#
# PARIInterface: Interface to PARI
#
# Declarations
#

#! @Chapter PARIInterface
#!
#! A C interface to the PARI number theory library
DeclareCategory( "IsPARIObj",  IsObject );
BindGlobal( "PARI_GEN_Family", NewFamily("PARIGENFamily"));
BindGlobal( "PARI_GEN_Type", NewType(PARI_GEN_Family, IsPARIObj) );

#! @Description
#!   Turn an univariate polynomial into a PARI polynomial
DeclareGlobalFunction( "PARIPolynomial" );
DeclareGlobalFunction( "PARIGaloisGroup" );
