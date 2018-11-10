/*
 * PARIInterface: Interface to PARI
 */

#include <pari/pari.h>
#include "src/compiled.h"          /* GAP headers */

#include <gmp.h>

#define PARI_T_GEN 0          // Generic PARI object

#define PARI_DAT_WORDS (sizeof(Obj) * 4)
#define PARI_DAT_TYPE(obj)         ((Int)(CONST_ADDR_OBJ(obj)[1]))
#define SET_PARI_DAT_TYPE(obj, t)  (ADDR_OBJ(obj)[1] = (Obj)(t))
#define PARI_DAT_GEN(obj)          ((GEN)(CONST_ADDR_OBJ(obj)[2]))
#define SET_PARI_DAT_GEN(obj, g)   (ADDR_OBJ(obj)[2] = (Obj)(g))


static Obj PARI_GEN_Type;    // Imported from GAP
static Obj PARI_GEN_REFLIST; // list of references to PARI Objects
                             // that are still used by GAP


// Conversions from PARI GEN to corresponding GAP Obj
static Obj PariGENToObj(GEN v);
static Obj PariVecToList(GEN v);
static Obj PariVecSmallToList(GEN v);
static Obj PariVecToList(GEN v);

// Conversions from GAP Object to PARI GEN
static GEN ObjToPariGEN(Obj obj);
static GEN ListToPariVec(Obj list);
static GEN IntToPariGEN(Obj o);

// This is a bag that wraps a PARI value, in case we want to hold on to
// it GAP-side.
// we should create reference counting for pari objects
// because we have full control over garbage collection
// We should never get unreachable loops in the dependency graph because
// never stick GAP objects into PARI objects
static Obj NewPARIGEN(GEN data)
{
    Obj o;

    o = NewBag(T_DATOBJ, PARI_DAT_WORDS);
    SetTypeDatObj(o, PARI_GEN_Type);
    SET_PARI_DAT_TYPE(o, PARI_T_GEN);
    SET_PARI_DAT_GEN(o, data);
    return o;
}

static Obj FuncPARI_GEN_GET_TYPE(Obj self, Obj obj)
{
    if((TNUM_OBJ(obj) != T_DATOBJ) || (PARI_DAT_TYPE(obj) != PARI_T_GEN))
        ErrorQuit("obj has to be a DATOBJ of type PARI_T_GEN", 0L, 0L);
    return INTOBJ_INT(typ(PARI_DAT_GEN(obj)));
}

static Obj FuncPARI_GEN_GET_DATA(Obj self, Obj obj)
{
    if((TNUM_OBJ(obj) != T_DATOBJ) || (PARI_DAT_TYPE(obj) != PARI_T_GEN))
        ErrorQuit("obj has to be a DATOBJ of type PARI_T_GEN", 0L, 0L);
    return PariGENToObj(PARI_DAT_GEN(obj));
}

static Obj FuncINT_TO_PARI_GEN(Obj self, Obj obj)
{
    return NewPARIGEN(IntToPariGEN(obj));
}

//
// Conversions from PARI GEN to corresponding GAP Obj
//
static Obj PariVecToList(GEN v)
{
    Int len = lg(v);
    Obj res = NEW_PLIST(T_PLIST, len - 1);
    SET_LEN_PLIST(res, len - 1);

    for(Int i = 1; i < len; i++) {
        SET_ELM_PLIST(res, i, PariGENToObj(gel(v, i)));
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
        SET_ELM_PLIST(res, i, INTOBJ_INT(itos(gel(v,i))));
        CHANGED_BAG(res);
    }
    return res;
}

/*
 * According to Bill Allombert PARI has support for
 * both MSW and LSW storage of ints,
 *  We only support LSW. If you want MSW, submit a patch
 */
static Obj PariIntToIntObj(GEN v)
{
    long size;

    if (typ (v) != t_INT)
        ErrorQuit("v has to be a PARI t_INT", 0L, 0L);

    size = signe(v) * (lgefint (v) - 2);
    return MakeObjInt(int_LSW(v), size);
}

static Obj PariFracToRatObj(GEN v)
{
    Obj res;

    res = NewBag(T_RAT, 2 * sizeof(Obj));
    SET_NUM_RAT(res, PariGENToObj(gel(v, 1)));
    SET_DEN_RAT(res, PariGENToObj(gel(v, 2)));

    return res;
}

static Obj PariPolToList(GEN v)
{
    Int len = lg(v);
    Obj res = NEW_PLIST(T_PLIST, len - 3);
    SET_LEN_PLIST(res, len - 2);
    for(Int i = 2; i<len; i++) {
        SET_ELM_PLIST(res, i-1, PariGENToObj(gel(v,i)));
        CHANGED_BAG(res);
    }
    return res;
}

// Main dispatch
// When there's no immediately obvious object to convert to
// We currently convert to a list of subobjects
// Some of this should probably be done on the GAP Level
static Obj PariGENToObj(GEN v)
{
    Obj res;
    switch (typ(v)) {
    case t_INT:    // Integer
        return PariIntToIntObj(v);
    case t_COL:    // Column Vector
    case t_VEC:    // Row Vector
        return PariVecToList(v);
    case t_VECSMALL:    // Vector of small integers
        return PariVecSmallToList(v);
    case t_STR:    // String
        return MakeString(GSTR(v));
    case t_INTMOD:    // Int mod Modulus
        return PariVecToList(v);
    case t_FRAC:    // Fraction
        return PariFracToRatObj(v);
    case t_POLMOD:    // Polynomial mod modulus
        return PariVecToList(v);
    case t_POL:       // Polynomial
        return PariPolToList(v);
    case t_MAT:       // Matrix
        return PariVecToList(v);
    case t_FFELT:     // Finite field element
    case t_SER:       // Power series
    case t_RFRAC:     // Rational function
    case t_PADIC:    // p-adic numbers
    case t_QUAD:     // quadratic numbers
    default:
        // TODO: Find names for the types
        ErrorQuit("PariGENToObj: not a supported type %i", typ(v), 0L);
        break;
    }
    return res;
}

//
// Converts a GAP Obj to a PARI GEN (if possible)
//
static GEN ObjToPariGEN(Obj obj);
static GEN ListToPariVec(Obj list);

static GEN ListToPariVec(Obj list)
{
    UInt len = LEN_LIST(list);
    GEN v = cgetg(len + 1, t_VEC);
    for(UInt i = 1; i <= len; i++ ) {
        Obj elt = ELM_LIST(list, i);
        gel(v, i) = ObjToPariGEN(elt);
    }
    return v;
}

static GEN IntToPariGEN(Obj o)
{
    Int i, size, sign;
    GEN r;

    if (IS_INTOBJ(o)) {
        // Immediate integers can be converted using
        // stoi
        r = stoi(UInt8_ObjInt(o));
    } else { // Large integer
        size = SIZE_INT(o);
        sign = IS_POS_INT(o) ? 1 : -1;

        r = cgeti(size + 2);
        // TODO: This looks a bit mysterious
        r[1] = evalsigne(sign) | evallgefint(size + 2);
        memcpy(int_W(r,0), ADDR_INT(o), size * sizeof(mp_limb_t));
    }
    return r;
}

static GEN CoeffListToPariGEN(Obj poly)
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
            gel(v, i) = stoi(Int8_ObjInt(elt));
        }
    }
    v[1] = evalsigne(0);
    v = normalizepol(v);

    return v;
}

static GEN ObjToPariGEN(Obj obj)
{
    if (IS_INT(obj))
        return IntToPariGEN(obj);
    else
        ErrorQuit("ObjToPariGEN: not a supported type: %s", TNAM_OBJ(obj), 0L);
}


//
// GAP Facing functions
//
Obj FuncPARI_VECINT(Obj self, Obj list)
{
    GEN v = ListToPariVec(list);
    return PariGENToObj(v);
}

Obj FuncPARI_UNIPOLY(Obj self, Obj poly)
{
    GEN v = CoeffListToPariGEN(poly);

    return NewPARIGEN(v);
}

Obj FuncPARI_POL_GALOIS_GROUP(Obj self, Obj poly)
{
    GEN v, w;

    v = PariGENUniPoly(poly);
    w = polgalois(v, DEFAULTPREC);

    return PariGENToObj(w);
}

Obj FuncPARI_POL_FACTOR_MOD_P(Obj self, Obj poly, Obj p)
{
    GEN v, w, x;

    v = PariGENUniPoly(poly);
    x = stoi(Int8_ObjInt(p));
    w = FpX_factor(v, x);

    return PariGENToObj(w);
}

Obj FuncPARI_GEN_ROUNDTRIP(Obj self, Obj x)
{
    return PariGENToObj(ObjToPariGEN(x));
}

Obj FuncPARI_MULT(Obj self, Obj x, Obj y)
{
    GEN a,b;

    a = ObjToPariGEN(x);
    b = ObjToPariGEN(y);

    return PariGENToObj(mpmul(a,b));
}

Obj FuncPARI_FACTOR_INT(Obj self, Obj x)
{
    GEN y, f;
    Obj r;

    y = ObjToPariGEN(x);
    f = factorint(y, 0);
    r = PariGENToObj(f);

    return r;
}

Obj FuncPARI_GET_VERSION(Obj self)
{
    return PariGENToObj(pari_version());
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
    return PariGENToObj(pari_version());
}

Obj FuncPARI_CLOSE(Obj self)
{
    pari_close();
}

typedef Obj (* GVarFunc)(/*arguments*/);

// Table of functions to export
static StructGVarFunc GVarFuncs [] = {
    GVAR_FUNC(PARI_INIT, 2, "stack, primes"),
    GVAR_FUNC(PARI_CLOSE, 0, ""),
    GVAR_FUNC(PARI_GET_VERSION, 0, ""),
    GVAR_FUNC(PARI_GEN_ROUNDTRIP, 1, "x"),
    GVAR_FUNC(PARI_MULT, 2, "a, b"),
    GVAR_FUNC(PARI_VECINT, 1, "list"),
    GVAR_FUNC(PARI_UNIPOLY, 1, "poly"),
    GVAR_FUNC(PARI_POL_GALOIS_GROUP, 1, "poly"),
    GVAR_FUNC(PARI_POL_FACTOR_MOD_P, 2, "poly, p"),
    GVAR_FUNC(PARI_FACTOR_INT, 1, "x"),
    GVAR_FUNC(PARI_GEN_GET_TYPE, 1, "o"),
    GVAR_FUNC(PARI_GEN_GET_DATA, 1, "o"),
    GVAR_FUNC(INT_TO_PARI_GEN, 1, "i"),
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
    ImportGVarFromLibrary("PARI_GEN_REFLIST", &PARI_GEN_REFLIST);

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
