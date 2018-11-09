gap> PARIInitialise();;
gap> r := PolynomialRing(Integers, 1);;
gap> p := r.1 ^ 5 + 2;;
gap> PARIGaloisGroup(p);
0
gap> PARIInitialise(100000,2^24);;

