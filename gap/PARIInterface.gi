#
# PARIInterface: Interface to PARI
#
# Implementations
#
InstallGlobalFunction( PARIInitialise,
function(args...)
    local stack, primes;

    if _PARIINTERFACE_INITIALISED then
        PARI_CLOSE();
    fi;

    stack := _PARIINTERFACE_STACK_DEFAULT;
    primes := _PARIINTERFACE_PRIMES_DEFAULT;

    if IsBound(args[1]) and IsPosInt(args[1]) then
        stack := args[1];
    fi;
    if IsBound(args[2]) and IsPosInt(args[2]) then
        primes := args[2];
    fi;
    PARI_INIT(stack, primes);
    _PARIINTERFACE_INITIALISED := true;
    return PARI_GET_VERSION();
end);

InstallGlobalFunction( PARIClose,
function()
    if _PARIINTERFACE_INITIALISED then
        PARI_CLOSE();
        _PARIINTERFACE_INITIALISED := false;
    fi;
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

InstallMethod( ViewObj, "for a PARI DatObj",
        [ IsPARIObj ],
function(o)
    Print("<PARI GEN>");
end);
