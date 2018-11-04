#
# PARIInterface: Interface to PARI
#
# Implementations
#

InstallGlobalFunction( PARIInitialise,
function(args...)
    local stack, primes;

    stack := _PARIINTERFACE_STACK_DEFAULT;
    primes := _PARIINTERFACE_PRIMES_DEFAULT;

    if IsBound(args[1]) and IsPosInt(args[1]) then
        stack := args[1];
    fi;
    if IsBound(args[2]) and IsPosInt(args[2]) then
        primes := args[2];
    fi;
    PARI_INIT(stack, primes);
end);

InstallGlobalFunction( PARIPolynomial,
function(p)
    local coeffs;
    coeffs := CoefficientsOfUnivariatePolynomial(p);
    return PARI_UNIPOLY(coeffs);
end );

InstallGlobalFunction( PARIGaloisGroup,
function(p)
    local coeffs;
    coeffs := CoefficientsOfUnivariatePolynomial(p);
    return PARI_POL_GALOIS_GROUP(coeffs);
end );
