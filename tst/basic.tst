gap> PARIInitialise();
gap> PARI_GET_VERSION();
[ 2, 11, 0 ]
gap> r := PolynomialRing(Integers, 1);;
gap> p := r.1 ^ 5 + 2;;
gap> PARIGaloisGroup(p);
0
