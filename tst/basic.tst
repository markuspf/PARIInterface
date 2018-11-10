gap> PARIInitialise();;
gap> r := PolynomialRing(Integers, 1);;
gap> p := r.1 ^ 5 + 2;;
gap> PARIGaloisGroup(p);
[ 20, -1, 1, "F(5) = 5:4" ]
gap> PARIInitialise(100000,2^24);;
gap> PARI_MULT(0,2);
0
gap> PARI_MULT(2,0);
0
gap> PARI_MULT(2,3);
6
gap> PARI_MULT(2^24,3^12);
