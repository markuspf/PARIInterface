/*
 * PARIInterface: Interface to PARI
 */

#include <pari/pari.h>
#include "src/compiled.h"          /* GAP headers */

#include <gmp.h>

#define PARI_T_GEN 0          // Generic PARI object

#define PARI_DAT_WORDS 4
#define PARI_DAT_TYPE(obj)  ADDR_OBJ(obj)[0]


static Obj PARI_GEN_Type; // Imported from GAP

static Obj NewPARIGEN()
{
    Obj gen;

    gen = NewBag(T_DATOBJ, PARI_DAT_WORDS);
    PARI_DAT_TYPE(gen) = PARI_T_GEN;
}

// Functions converting between pari and mpz
//
// These are borrowed from the paritwine library
static void mpz_set_GEN (mpz_ptr z, GEN x)
   /* Sets z to x, which needs to be of type t_INT. */
{
   const long lx = lgefint (x) - 2;
   const long sign = signe (x);
   int i;

   assert (sizeof (long) == sizeof (mp_limb_t));

   if (typ (x) != t_INT)
      pari_err_TYPE ("mpz_set_GEN", x);

   if (sign == 0)
      mpz_set_ui (z, 0);
   else {
      mpz_realloc2 (z, lx * BITS_IN_LONG);
      z->_mp_size = sign * lx;
      for (i = 0; i < lx; i++)
         (z->_mp_d) [i] = *int_W (x, i);
   }
}

static GEN mpz_get_GEN (mpz_srcptr z)
   /* Returns the GEN of type t_INT corresponding to z. */
{
   const long lz = z->_mp_size;
   const long lx = labs (lz);
   const long lx2 = lx + 2;
   int i;
   GEN x = cgeti (lx2);

   assert (sizeof (long) == sizeof (mp_limb_t));

   x [1] = evalsigne ((lz > 0 ? 1 : (lz < 0 ? -1 : 0))) | evallgefint (lx2);
   for (i = 0; i < lx; i++)
      *int_W (x, i) = (z->_mp_d) [i];

   return x;
}

/*
 *
 */
Obj FuncPARIGEN_INT(Obj self, Obj intobj)
{
    if(!IS_INTOBJ(intobj))
        ErrorQuit("<intobj> has to be an integer object", 0L, 0L);

}

static GEN ListToPariVec(Obj list);
static Obj PariVecToList(GEN v);

static GEN ListToPariVec(Obj list)
{
    // TODO: If we want this to be strictly correct, we need to use the
    //       traversal code...
    UInt len = LEN_LIST(list);
    GEN v = cgetg(len + 1, t_VEC);
    for(UInt i = 1; i <= len; i++ ) {
        Obj elt = ELM_LIST(list, i);
        if(IS_INTOBJ(elt)) {
            v[i] = stoi(Int8_ObjInt(elt));
        } else if(IS_LIST(elt)) {
            v[i] = ListToPariVec(elt);
        } else {
            ErrorQuit("encountered unhandled object", 0L, 0L);
        }
    }
    return v;
}

// Converts a PARI GEN to the corresponding GAP
// object
//
// Currently supports t_INT, t_VEC, and t_VECSMALL.

static Obj PariGENToObj(GEN v);
static Obj PariVecToList(GEN v);
static Obj PariVecSmallToList(GEN v);

static Obj PariVecToList(GEN v)
{
    Int len = lg(v);
    Obj res = NEW_PLIST(T_PLIST, len - 1);
    SET_LEN_PLIST(res, len - 1);

    for(Int i = 1; i < len; i++) {
        SET_ELM_PLIST(res, i, PariGENToObj(v[i]));
        CHANGED_BAG(res);
    }
    return res;
}

static Obj PariVecSmallToList(GEN v)
{
    Int len = lg(v);
    Obj res = NEW_PLIST(T_PLIST, len - 1);
    SET_LEN_PLIST(res, len - 1);

    for(Int i = 1; i < len; i++) {
        SET_ELM_PLIST(res, i, INTOBJ_INT(v[i]));
        CHANGED_BAG(res);
    }
    return res;
}

//
// Convert a PARI Gen to a GAP Obj
//
static Obj PariGENToObj(GEN v)
{
    Obj res;
    switch(typ(v)) {
    case t_INT:
        return INTOBJ_INT(itos(v));
        break;
    case t_INTMOD:
        break;
    case t_FRAC:
        break;
    case t_FFELT:
        break;
    case t_PADIC:
        break;
    case t_POLMOD:
        break;
    case t_POL:
        break;
    case t_SER:
        break;
    case t_RFRAC:
        break;
    case t_VEC:
        return PariVecToList(v);
        break;
    case t_COL:
        break;
    case t_MAT:
        break;
    case t_VECSMALL:
        return PariVecSmallToList(v);
        break;
    case t_STR:
        return NEW_CSTRING(GSTR(v));
        break;
    case t_ERROR:
        break;
    default:
        ErrorQuit("not a supported type", 0L, 0L);
        break;
    }
    return res;
}


static GEN ObjToPariGEN(Obj o)
{
}

Obj FuncPARI_VECINT(Obj self, Obj list)
{
    GEN v = ListToPariVec(list);
    return PariGENToObj(v);
}

GEN PariGENUniPoly(Obj poly)
{
    UInt len;
    UInt deg;
    GEN v;

    len = LEN_LIST(poly);
    // Zero polynomial
    if (len == 0) {
        v = cgetg(2, t_POL);
    } else {
        deg = len - 1;
        v = cgetg(3 + deg, t_POL);
        for(UInt i = 2; i < 2 + len; i++) {
            Obj elt = ELM_LIST(poly, i - 1);
            v[i] = stoi(Int8_ObjInt(elt));
        }
    }
    v[1] = evalsigne(0);
    v = normalizepol(v);

    return v;
}

Obj FuncPARI_UNIPOLY(Obj self, Obj poly)
{
    GEN v = PariGENUniPoly(poly);
    output(v);

    return 0;
}

Obj FuncPARI_POL_GALOIS_GROUP(Obj self, Obj poly)
{
    GEN v, w;
    v = PariGENUniPoly(poly);
    output(v);
    w = polgalois(v, DEFAULTPREC);
    output(w);
}

Obj FuncPARI_POL_FACTOR_MOD_P(Obj self, Obj poly, Obj p)
{
    GEN v, w, x;

    v = PariGENUniPoly(poly);
    output(v);
    x = stoi(Int8_ObjInt(p));
    output(x);
    w = FpX_factor(v, x);
    output(w);
}

Obj FuncPARI_INIT(Obj self, Obj stack, Obj primes)
{
    if(!IS_INTOBJ(stack))
        ErrorQuit("<stack> has to be an integer", 0L, 0L);
    if(!IS_INTOBJ(primes))
        ErrorQuit("<primes> has to be an integer", 0L, 0L);

    size_t size = INT_INTOBJ(stack);
    ulong maxprime = INT_INTOBJ(primes);

    pari_init(size, maxprime);
}

typedef Obj (* GVarFunc)(/*arguments*/);

// Table of functions to export
static StructGVarFunc GVarFuncs [] = {
    GVAR_FUNC(PARI_INIT, 2, "stack, primes"),
    GVAR_FUNC(PARI_VECINT, 1, "list"),
    GVAR_FUNC(PARI_UNIPOLY, 1, "poly"),
    GVAR_FUNC(PARI_POL_GALOIS_GROUP, 1, "poly"),
    GVAR_FUNC(PARI_POL_FACTOR_MOD_P, 2, "poly, p"),
	{ 0 } /* Finish with an empty entry */
};

/******************************************************************************
*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel( StructInitInfo *module )
{
    /* init filters and functions                                          */
    InitHdlrFuncsFromTable( GVarFuncs );

    ImportGVarFromLibrary("PARI_GEN_Type", &PARI_GEN_Type);

    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/
static Int InitLibrary( StructInitInfo *module )
{
    /* init filters and functions */
    InitGVarFuncsFromTable( GVarFuncs );

    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitInfopl()  . . . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
    .type = MODULE_DYNAMIC,
    .name = "PARIInterface",
    .initKernel = InitKernel,
    .initLibrary = InitLibrary,
};

StructInitInfo *Init__Dynamic( void )
{
    return &module;
}
