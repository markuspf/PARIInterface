#
# PARIInterface: Interface to PARI
#
# Implementations
#

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
