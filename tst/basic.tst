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
8916100448256
gap> PARI_FACTOR_INT(24012425);
[ [ 5, 960497 ], [ 2, 1 ] ]
gap> PARI_FACTOR_INT(100);
[ [ 2, 5 ], [ 2, 2 ] ]
gap> PARI_FACTOR_INT(1204102740127840128401821209348);
[ [ 2, 3, 163, 7823, 12097, 27719453, 234670785731 ], [ 2, 1, 1, 1, 1, 1, 1 ] 
 ]
