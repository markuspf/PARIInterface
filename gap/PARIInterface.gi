#
# PARIInterface: Interface to PARI
#
# Implementations
#

InstallGlobalFunction( PARIPolynomial,
function(p)
    local coeffs;
    coeffs := CoefficientsOfUnivariatePolynomial(p);
    PARIGEN_UNIPOLY(coeffs);
end );

InstallGlobalFunction( PARIGaloisGroup,
function(p)
    local coeffs;
    coeffs := CoefficientsOfUnivariatePolynomial(p);
    PARIGEN_POLGALOIS(coeffs);
end );
