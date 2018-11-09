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

// This is a bag that wraps a PARI value, in case we want to hold on to
// it GAP-side.
// Note that we must not hold on to values on the PARI stack as they might
// disappear due to garbage collection
static Obj NewPARIGEN()
{
    Obj o;

    o = NewBag(T_DATOBJ, PARI_DAT_WORDS);
    PARI_DAT_TYPE(o) = PARI_T_GEN;

    return o;
}

//
// Conversions from PARI GEN to corresponding GAP Obj
//
// Currently supports t_INT, t_VEC, t_VECSMALL, t_STR.
//
static Obj PariGENToObj(GEN v);
static Obj PariVecToList(GEN v);
static Obj PariVecSmallToList(GEN v);
static Obj PariVecToList(GEN v);

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

// Main dispatch
static Obj PariGENToObj(GEN v)
{
    Obj res;
    switch(typ(v)) {
    case t_INT:
        return PariIntToIntObj(v);
        break;
    case t_VEC:
        return PariVecToList(v);
        break;
    case t_VECSMALL:
        return PariVecSmallToList(v);
        break;
    case t_STR:
        return MakeString(GSTR(v));
        break;
    case t_INTMOD:
    case t_FRAC:
    case t_FFELT:
    case t_PADIC:
    case t_POLMOD:
    case t_POL:
    case t_SER:
    case t_RFRAC:
    case t_COL:
    case t_MAT:
    case t_ERROR:
    default:
        // TODO: Find names for the types
        ErrorQuit("not a supported type", 0L, 0L);
        break;
    }
    return res;
}

//
// Converts a GAP Obj to a PARI GEN (if possible)
//
static GEN ListToPariVec(Obj list);

static GEN ListToPariVec(Obj list)
{
    // TODO: If we want this to be strictly correct, we need to use the
    //       traversal code...
    UInt len = LEN_LIST(list);
    GEN v = cgetg(len + 1, t_VEC);
    for(UInt i = 1; i <= len; i++ ) {
        Obj elt = ELM_LIST(list, i);
        if(IS_INTOBJ(elt)) {
            gel(v, i) = stoi(Int8_ObjInt(elt));
        } else if(IS_LIST(elt)) {
            gel(v, i) = ListToPariVec(elt);
        } else {
            ErrorQuit("encountered unhandled object", 0L, 0L);
        }
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
        r[1] = size * sign;
        for (i = 0; i < size; i++)
            *int_W(r, i) = ADDR_INT(o)[i];
    }
    return r;
}

static GEN PariGENUniPoly(Obj poly)
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
    // Replace tnum switch by IS_xxx macros?
    switch(TNUM_OBJ(obj)) {
    case T_INT:
        break;
    case T_FFE:
        break;
    case T_PLIST:
        break;
    default:
        ErrorQuit("not a supported type", 0L, 0L);
        break;
    }
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
    GEN v = PariGENUniPoly(poly);
    output(v);

    return 0;
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
    output(v);
    x = stoi(Int8_ObjInt(p));
    output(x);
    w = FpX_factor(v, x);
    output(w);

    return 0;
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
