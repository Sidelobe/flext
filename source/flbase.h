/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is all derived from GEM by Mark Danks


#ifndef _FLBASE_H
#define _FLBASE_H

#include <flstdc.h>

class flext_obj;

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    flext_hdr
    
    The obligatory object header

DESCRIPTION
    
    This is in a separate struct to assure that obj is the very first thing.  
    If it were the first thing in flext_obj, then there could be problems with
    the vtable.
     
-----------------------------------------------------------------*/
struct FLEXT_EXT flext_hdr
{
    	//////////
    	// The obligatory object header
    	t_sigobj    	    obj;  // MUST reside at memory offset 0

#ifdef PD
		float defsig;			// float signal holder for pd
#endif

#if defined(MAXMSP) 
		long curinlet;      // current inlet used by proxy objects
#endif

    	//////////
    	// Our data structure
        flext_obj           *data;
};

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    flext_obj
    
    The base class for all externs written in C++

DESCRIPTION
    
    Each extern which is written in C++ needs to use the #defines at the
    end of this header file.  
    
    The define
    
        FLEXT_HEADER(NEW_CLASS, PARENT_CLASS)
    
    should be somewhere in your header file.
    One of the defines like
    
    FLEXT_NEW(NEW_CLASS)
    FLEXT_NEW_2(NEW_CLASS, float, float)
    
    should be the first thing in your implementation file.
    NEW_CLASS is the name of your class and PARENT_CLASS is the 
    parent of your class.
        
-----------------------------------------------------------------*/
class FLEXT_EXT flext_obj
{
    public:

		// --- overloading of new/delete memory allocation methods ----
		// MaxMSP allows only 16K in overdrive mode!

		void *operator new(size_t bytes);
		void operator delete(void *blk);

		#ifndef __MRC__ // doesn't allow new[] overloading?!
		void *operator new[](size_t bytes) { return operator new(bytes); }
		void operator delete[](void *blk) { operator delete(blk); }
		#endif

		// these are aligned 
		static void *NewAligned(size_t bytes,int bitalign = 128);
		static void FreeAligned(void *blk);
		
		// ---------------------

        //////////
        // Constructor
    	flext_obj();

    	//////////
    	// Destructor
    	virtual ~flext_obj() = 0;
    	
        //////////
        // Get the object's canvas
        t_canvas            *getCanvas()        { return(m_canvas); }

    protected:
    	
        //////////
        // The object header
        flext_hdr          *x_obj;        	

    private:

        //////////
        // The canvas (patcher) that the object is in
        t_canvas            *m_canvas;

	public:

    	//////////
    	// Creation callback
    	static void callb_setup(t_class *) {}	

        //////////
        // This is a holder - don't touch it
        static flext_hdr     *m_holder;
        static const char *m_holdname;  // hold object's name during construction

        //////////
        // The object's name in the patcher
		const char *m_name;
		
#ifdef _DEBUG
		static bool check_tilde(const char *objname,const char *setupfun);
#endif

#ifdef MAXMSP
		union lib_arg {
			int i;
			float f;
			t_symbol *s;
		};
		
		static t_class *lib_class;
		static void libfun_add(const char *name,t_method newfun,void (*freefun)(flext_hdr *),int argtp1,...);
		static flext_hdr *libfun_new(t_symbol *s,int argc,t_atom *argv);
		static void libfun_free(flext_hdr *o);
#endif
};

// This has a dummy arg so that NT won't complain
inline void *operator new(size_t, void *location, void *) { return location; }

////////////////////////////////////////
// This should be used in the header
////////////////////////////////////////

#ifdef PD
#define FLEXT_GETCLASS(obj) ((t_class *)((t_object *)(obj))->te_g.g_pd)
#elif defined(MAXMSP)
#define FLEXT_GETCLASS(obj) ((t_class *)(((t_tinyobject *)obj)->t_messlist-1))
#endif


// Header without setup callback

#define FLEXT_HEADER(NEW_CLASS, PARENT_CLASS)    	    	\
public:     	    	    \
typedef NEW_CLASS thisType;  \
static void callb_free(flext_hdr *hdr)    	    	    	\
{ flext_obj *mydata = ((flext_hdr *)hdr)->data; delete mydata; \
  ((flext_hdr *)hdr)->flext_hdr::~flext_hdr(); }   	    	\
static void callb_setup(t_class *classPtr)  	    	\
{ PARENT_CLASS::callb_setup(classPtr); }  	    	    	\
protected:    \
inline t_sigobj *thisHdr() { return &x_obj->obj; } \
inline t_class *thisClass() { return FLEXT_GETCLASS(x_obj); } \
inline const char *thisName() const { return m_name; } \
static NEW_CLASS *thisObject(void *c) { return (NEW_CLASS *)((flext_hdr *)c)->data; }	  


// Header with setup callback

#define FLEXT_HEADER_S(NEW_CLASS, PARENT_CLASS,SETUPFUN)    	    	\
public:     	    	    \
typedef NEW_CLASS thisType;  \
static void callb_free(flext_hdr *hdr)    	    	    	\
{ flext_obj *mydata = ((flext_hdr *)hdr)->data; delete mydata; \
  ((flext_hdr *)hdr)->flext_hdr::~flext_hdr(); }   	    	\
static void callb_setup(t_class *classPtr)  	    	\
{ PARENT_CLASS::callb_setup(classPtr);    	    	\
	NEW_CLASS::SETUPFUN(classPtr); }  	    	    	\
protected:    \
inline t_sigobj *thisHdr() { return &x_obj->obj; } \
inline t_class *thisClass() { return FLEXT_GETCLASS(x_obj); } \
inline const char *thisName() const { return m_name; } \
static NEW_CLASS *thisObject(void *c) { return (NEW_CLASS *)((flext_hdr *)c)->data; }	  


#define REAL_NEW(NAME,NEW_CLASS,SETUP_FUNCTION) REAL_INST(0,NAME,NEW_CLASS,SETUP_FUNCTION)
#define REAL_NEW_G(NAME,NEW_CLASS,SETUP_FUNCTION) REAL_INST_G(0,NAME,NEW_CLASS,SETUP_FUNCTION)
#define REAL_NEW_1(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1) REAL_INST_1(0,NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1)
#define REAL_NEW_2(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2) REAL_INST_2(0,NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2)
#define REAL_NEW_3(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2,TYPE3) REAL_INST_3(0,NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2,TYPE3)

#ifdef PD
#define REAL_EXT(NEW_CLASS,SETUP_FUNCTION)
#define REAL_LIB(NAME,NEW_CLASS,SETUP_FUNCTION) REAL_INST(1,NAME,NEW_CLASS,SETUP_FUNCTION)
#define REAL_LIB_G(NAME,NEW_CLASS,SETUP_FUNCTION) REAL_INST_G(1,NAME,NEW_CLASS,SETUP_FUNCTION)
#define REAL_LIB_1(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1) REAL_INST_1(1,NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1)
#define REAL_LIB_2(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2) REAL_INST_2(1,NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2)
#define REAL_LIB_3(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2,TYPE3) REAL_INST_3(1,NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2,TYPE3)
#else // MAXMSP
#define REAL_EXT(NEW_CLASS,SETUP_FUNCTION) extern "C" FLEXT_EXT int main() { NEW_CLASS##SETUP_FUNCTION(); return 0; }
#define REAL_LIB(NAME,NEW_CLASS,SETUP_FUNCTION) REAL_NEWLIB(NAME,NEW_CLASS,SETUP_FUNCTION)
#define REAL_LIB_G(NAME,NEW_CLASS,SETUP_FUNCTION) REAL_NEWLIB_G(NAME,NEW_CLASS,SETUP_FUNCTION)
#define REAL_LIB_1(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1) REAL_NEWLIB_1(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1)
#define REAL_LIB_2(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2) REAL_NEWLIB_2(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2)
#define REAL_LIB_3(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2,TYPE3) REAL_NEWLIB_3(NAME,NEW_CLASS,SETUP_FUNCTION,TYPE1,TYPE2,TYPE3)
#endif

////////////////////////////////////////
// This should be the first thing in the implementation file
////////////////////////////////////////

//
// NO ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW(NAME,NEW_CLASS)		\
REAL_NEW(NAME,NEW_CLASS, _setup)  \
REAL_EXT(NEW_CLASS, _setup)

#define FLEXT_NEW_TILDE(NAME,NEW_CLASS)	\
REAL_NEW(NAME,NEW_CLASS, _tilde_setup) \
REAL_EXT(NEW_CLASS, _tilde_setup)

#define FLEXT_LIB(NAME,NEW_CLASS)		REAL_LIB(NAME,NEW_CLASS, _setup)
#define FLEXT_LIB_TILDE(NAME,NEW_CLASS)	REAL_LIB(NAME,NEW_CLASS, _tilde_setup)

//
// ONE ARGUMENT
/////////////////////////////////////////////////
#define FLEXT_NEW_1(NAME,NEW_CLASS, TYPE)		\
REAL_NEW_1(NAME,NEW_CLASS, _setup, TYPE) \
REAL_EXT(NEW_CLASS, _setup)

#define FLEXT_NEW_TILDE_1(NAME,NEW_CLASS, TYPE)	\
REAL_NEW_1(NAME,NEW_CLASS, _tilde_setup, TYPE) \
REAL_EXT(NEW_CLASS, _tilde_setup)

#define FLEXT_LIB_1(NAME,NEW_CLASS, TYPE)		REAL_LIB_1(NAME,NEW_CLASS, _setup,TYPE)
#define FLEXT_LIB_TILDE_1(NAME,NEW_CLASS, TYPE)	REAL_LIB_1(NAME,NEW_CLASS, _tilde_setup, TYPE)

//
// GIMME ARGUMENT
/////////////////////////////////////////////////
#define FLEXT_NEW_G(NAME,NEW_CLASS)			\
REAL_NEW_G(NAME,NEW_CLASS, _setup) \
REAL_EXT(NEW_CLASS, _setup)

#define FLEXT_NEW_TILDE_G(NAME,NEW_CLASS)	\
REAL_NEW_G(NAME,NEW_CLASS,_tilde_setup) \
REAL_EXT(NEW_CLASS, _tilde_setup)

#define FLEXT_LIB_G(NAME,NEW_CLASS)			REAL_LIB_G(NAME,NEW_CLASS, _setup) 
#define FLEXT_LIB_TILDE_G(NAME,NEW_CLASS)	REAL_LIB_G(NAME,NEW_CLASS, _tilde_setup) 

//
// TWO ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_2(NAME,NEW_CLASS, TYPE1, TYPE2)			\
REAL_NEW_2(NAME,NEW_CLASS, _setup, TYPE1, TYPE2) \
REAL_EXT(NEW_CLASS, _setup)

#define FLEXT_NEW_TILDE_2(NAME,NEW_CLASS, TYPE1, TYPE2)	\
REAL_NEW_2(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2) \
REAL_EXT(NEW_CLASS, _tilde_setup)

#define FLEXT_LIB_2(NAME,NEW_CLASS, TYPE1, TYPE2)		REAL_LIB_2(NAME,NEW_CLASS, _setup, TYPE1, TYPE2)
#define FLEXT_LIB_TILDE_2(NAME,NEW_CLASS, TYPE1, TYPE2)	REAL_LIB_2(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2)

//
// THREE ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3) \
REAL_NEW_3(NAME,NEW_CLASS, _setup, TYPE1, TYPE2, TYPE3)  \
REAL_EXT(NEW_CLASS, _setup)

#define FLEXT_NEW_TILDE_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	\
REAL_NEW_3(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2, TYPE3) \
REAL_EXT(NEW_CLASS, _tilde_setup)

#define FLEXT_LIB_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)		REAL_LIB_3(NAME,NEW_CLASS, _setup,TYPE1, TYPE2, TYPE3)
#define FLEXT_LIB_TILDE_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	REAL_LIB_3(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2, TYPE3)

/*
// MaxMSP doesn't seem to be able to handle more than 3 creation arguments! -> USE GIMME

//
// FOUR ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_4(NAME,NEW_CLASS, TYPE, TTWO, TTHREE, TFOUR) \
    REAL_NEW_4(NAME,NEW_CLASS, _setup, _class, TYPE, TTWO, TTHREE, TFOUR)

#define FLEXT_NEW_TILDE_4(NAME,NEW_CLASS, TYPE, TTWO, TTHREE, TFOUR) \
    REAL_NEW_4(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, TTWO, TTHREE, TFOUR)
*/


//////////////////////////////////////////////////////
// This is to be called in the library setup function
//////////////////////////////////////////////////////

#define FLEXT_EXP_0 extern "C" FLEXT_EXT
#define FLEXT_EXP_1  
#define FLEXT_EXP(LIB) FLEXT_EXP_##LIB

#define FLEXT_SETUP(cl) \
extern void cl##_setup(); \
cl##_setup()  

#define FLEXT_TILDE_SETUP(cl) \
extern void cl##_tilde_setup(); \
cl##_tilde_setup()  

#ifdef PD
#define FLEXT_LIB_SETUP(NAME,SETUPFUN) extern "C" FLEXT_EXT void NAME##_setup() { SETUPFUN(); }
#else // MAXMSP
#define FLEXT_LIB_SETUP(NAME,SETUPFUN) \
extern "C" FLEXT_EXT int main() { \
SETUPFUN(); \
::setup((t_messlist **)&flext_obj::lib_class,(t_newmethod)&flext_obj::libfun_new,(t_method)flext_obj::libfun_free,sizeof(flext_hdr), 0,A_GIMME,A_NULL); \
return 0; \
}
#endif

////////////////////////////////////////
// These definitions are used below
////////////////////////////////////////

// Shortcuts for PD/Max type arguments
#define FLEXTTYPE_void A_NULL
#define FLEXTTYPE_float A_FLOAT
#define FLEXTTYPE_t_float A_FLOAT
#define FLEXTTYPE_t_flint A_FLINT
#define FLEXTTYPE_t_symptr A_SYMBOL
#define FLEXTTYPE_t_ptrtype A_POINTER

#define FLEXTTP(TP) FLEXTTYPE_ ## TP


#ifdef PD
#define FLEXT_NEWFN ::class_new
#define FLEXT_CLREF(NAME,CLASS) gensym(NAME)
//#define FLEXT_MAIN(MAINNAME) MAINNAME
#define CLNEW_OPTIONS 0  // flags for class creation
//#define LIB_INIT(NAME,NEWMETH,FREEMETH,ARG1,ARG2,ARG3,ARG4) ((void(0))
//#define LIB_INIT(NAME,NEWMETH,FREEMETH,ARGC) ((void(0))
//#define IS_PD 1
//#define IS_MAXMSP 0

#define newobject(CLSS) pd_new(CLSS)

#define ARGMEMBER_t_flint f

#elif defined(MAXMSP)
#define FLEXT_NEWFN NULL; ::setup    // extremely ugly!!! I hope Mark Danks doesn't see that......
#define FLEXT_CLREF(NAME,CLASS) (t_messlist **)&(CLASS)
//#define FLEXT_MAIN_0(MAINNAME) main // main for standalone object
//#define FLEXT_MAIN_1(MAINNAME) MAINNAME // main for library object
//#define FLEXT_MAIN(MAINNAME) main
#define CLNEW_OPTIONS 0  // flags for class creation

//#define LIB_INIT(NAME,NEWMETH,FREEMETH,ARG1,ARG2,ARG3,ARG4) flext_obj::libfun_add(NAME,(t_newmethod)(NEWMETH),FREEMETH,ARG1,ARG2,ARG3,ARG4,A_NULL) 

//#define IS_PD 0
//#define IS_MAXMSP 1

#define ARGMEMBER_t_flint i

#endif

//#define ARGMEMBER_int i
#define ARGMEMBER_float f
#define ARGMEMBER_t_symptr s
#define ARGCAST(arg,tp) arg.ARGMEMBER_##tp

//#define EXTPROTO_0  extern "C" FLEXT_EXT
//#define EXTPROTO_1
//#define EXTPROTO(LIB) EXTPROTO_ ## LIB

#ifdef _DEBUG
#define CHECK_TILDE(OBJNAME,SETUPFUN) flext_obj::check_tilde(OBJNAME,SETUPFUN)
#else
#define CHECK_TILDE(OBJNAME,SETUPFUN) ((void)0)
#endif


///////////////////////////////////////////////////////////////////////////////
// These should never be called or used directly!!!
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// no args
///////////////////////////////////////////////////////////////////////////////
#define REAL_INST(LIB,NAME,NEW_CLASS, SETUP_FUNCTION) \
static t_class * NEW_CLASS ## _class;    	    	    	\
flext_hdr* class_ ## NEW_CLASS () \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## _class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS;                     \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void NEW_CLASS ## SETUP_FUNCTION()   \
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## _class = FLEXT_NEWFN(                       \
     	FLEXT_CLREF(NAME,NEW_CLASS ## _class), 	    	    	    	\
    	(t_newmethod)class_ ## NEW_CLASS,	    	\
    	(t_method)&NEW_CLASS::callb_free,         \
     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## _class); \
} 

#define REAL_NEWLIB(NAME,NEW_CLASS, SETUP_FUNCTION) \
flext_hdr* class_ ## NEW_CLASS ()    \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(flext_obj::lib_class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS;      \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()   	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    flext_obj::libfun_add(NAME,(t_method)(class_ ## NEW_CLASS),&NEW_CLASS::callb_free,A_NULL); \
    NEW_CLASS::callb_setup(flext_obj::lib_class); \
}   


///////////////////////////////////////////////////////////////////////////////
// one arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_INST_1(LIB,NAME,NEW_CLASS, SETUP_FUNCTION, TYPE1) \
static t_class * NEW_CLASS ## _class;    	    	    	\
flext_hdr* class_ ## NEW_CLASS (TYPE1 arg1) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## _class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg1);                     \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void NEW_CLASS ## SETUP_FUNCTION()   \
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## _class = FLEXT_NEWFN(                       \
     	FLEXT_CLREF(NAME,NEW_CLASS ## _class), 	    	    	    	\
    	(t_newmethod)class_ ## NEW_CLASS,	    	\
    	(t_method)&NEW_CLASS::callb_free,         \
     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
     	FLEXTTP(TYPE1),                       \
     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## _class); \
} 

#define REAL_NEWLIB_1(NAME,NEW_CLASS, SETUP_FUNCTION,TYPE1) \
flext_hdr* class_ ## NEW_CLASS (const flext_obj::lib_arg &arg1)    \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(flext_obj::lib_class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(ARGCAST(arg1,TYPE1));      \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()   	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    flext_obj::libfun_add(NAME,(t_method)(class_ ## NEW_CLASS),&NEW_CLASS::callb_free,FLEXTTP(TYPE1),A_NULL); \
    NEW_CLASS::callb_setup(flext_obj::lib_class); \
}   


///////////////////////////////////////////////////////////////////////////////
// gimme arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_INST_G(LIB,NAME,NEW_CLASS, SETUP_FUNCTION) \
static t_class * NEW_CLASS ## _class;    	    	    	\
flext_hdr* class_ ## NEW_CLASS (t_symbol *s,int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## _class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(argc,argv);                     \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void NEW_CLASS ## SETUP_FUNCTION()   \
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## _class = FLEXT_NEWFN(                       \
     	FLEXT_CLREF(NAME,NEW_CLASS ## _class), 	    	    	    	\
    	(t_newmethod)class_ ## NEW_CLASS,	    	\
    	(t_method)&NEW_CLASS::callb_free,         \
     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
     	A_GIMME,                       \
     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## _class); \
} 

#define REAL_NEWLIB_G(NAME,NEW_CLASS, SETUP_FUNCTION) \
flext_hdr* class_ ## NEW_CLASS (t_symbol *s,int argc,t_atom *argv)    \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(flext_obj::lib_class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(argc,argv);      \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()   	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    flext_obj::libfun_add(NAME,(t_method)(class_ ## NEW_CLASS),&NEW_CLASS::callb_free,A_GIMME,A_NULL); \
    NEW_CLASS::callb_setup(flext_obj::lib_class); \
}   


///////////////////////////////////////////////////////////////////////////////
// two args
///////////////////////////////////////////////////////////////////////////////
#define REAL_INST_2(LIB,NAME,NEW_CLASS, SETUP_FUNCTION, TYPE1, TYPE2) \
static t_class * NEW_CLASS ## _class;    	    	    	\
flext_hdr* class_ ## NEW_CLASS (TYPE1 arg1, TYPE2 arg2) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## _class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg1, arg2);                     \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void NEW_CLASS ## SETUP_FUNCTION()   \
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## _class = FLEXT_NEWFN(                       \
     	FLEXT_CLREF(NAME,NEW_CLASS ## _class), 	    	    	    	\
    	(t_newmethod)class_ ## NEW_CLASS,	    	\
    	(t_method)&NEW_CLASS::callb_free,         \
     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
     	FLEXTTP(TYPE1), FLEXTTP(TYPE2),                       \
     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## _class); \
} 

#define REAL_NEWLIB_2(NAME,NEW_CLASS, SETUP_FUNCTION,TYPE1, TYPE2) \
flext_hdr* class_ ## NEW_CLASS (const flext_obj::lib_arg &arg1,const flext_obj::lib_arg &arg2)    \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(flext_obj::lib_class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(ARGCAST(arg1,TYPE1),ARGCAST(arg2,TYPE2));      \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()   	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    flext_obj::libfun_add(NAME,(t_method)(class_ ## NEW_CLASS),&NEW_CLASS::callb_free,FLEXTTP(TYPE1),FLEXTTP(TYPE2),A_NULL); \
    NEW_CLASS::callb_setup(flext_obj::lib_class); \
}   


///////////////////////////////////////////////////////////////////////////////
// three args
///////////////////////////////////////////////////////////////////////////////
#define REAL_INST_3(LIB,NAME,NEW_CLASS, SETUP_FUNCTION, TYPE1,TYPE2,TYPE3) \
static t_class * NEW_CLASS ## _class;    	    	    	\
flext_hdr* class_ ## NEW_CLASS (TYPE1 arg1,TYPE2 arg2,TYPE3 arg3) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## _class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg1,arg2,arg3);                     \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void NEW_CLASS ## SETUP_FUNCTION()   \
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## _class = FLEXT_NEWFN(                       \
     	FLEXT_CLREF(NAME,NEW_CLASS ## _class), 	    	    	    	\
    	(t_newmethod)class_ ## NEW_CLASS,	    	\
    	(t_method)&NEW_CLASS::callb_free,         \
     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
     	FLEXTTP(TYPE1), FLEXTTP(TYPE2),FLEXTTP(TYPE3),                       \
     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## _class); \
} 

#define REAL_NEWLIB_3(NAME,NEW_CLASS, SETUP_FUNCTION,TYPE1,TYPE2,TYPE3) \
flext_hdr* class_ ## NEW_CLASS (const flext_obj::lib_arg &arg1,const flext_obj::lib_arg &arg2,const flext_obj::lib_arg &arg3)    \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(flext_obj::lib_class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(ARGCAST(arg1,TYPE1),ARGCAST(arg2,TYPE2),ARGCAST(arg3,TYPE3));      \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()   	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    flext_obj::libfun_add(NAME,(t_method)(class_ ## NEW_CLASS),&NEW_CLASS::callb_free,FLEXTTP(TYPE1),FLEXTTP(TYPE2),FLEXTTP(TYPE3),A_NULL); \
    NEW_CLASS::callb_setup(flext_obj::lib_class); \
}   

///////////////////////////////////////////////////////////////////////////////
// four args
///////////////////////////////////////////////////////////////////////////////
#define REAL_INST_4(LIB,NAME,NEW_CLASS, SETUP_FUNCTION, TYPE1,TYPE2,TYPE3,TYPE4) \
static t_class * NEW_CLASS ## _class;    	    	    	\
flext_hdr* class_ ## NEW_CLASS (TYPE1 arg1,TYPE2 arg2,TYPE3 arg3,TYPE4 arg4) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## _class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg1,arg2,arg3,arg4);                     \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void NEW_CLASS ## SETUP_FUNCTION()   \
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## _class = FLEXT_NEWFN(                       \
     	FLEXT_CLREF(NAME,NEW_CLASS ## _class), 	    	    	    	\
    	(t_newmethod)class_ ## NEW_CLASS,	    	\
    	(t_method)&NEW_CLASS::callb_free,         \
     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
     	FLEXTTP(TYPE1), FLEXTTP(TYPE2),FLEXTTP(TYPE3),FLEXTTP(TYPE4),                     \
     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## _class); \
} 

#define REAL_NEWLIB_4(NAME,NEW_CLASS, SETUP_FUNCTION,TYPE1,TYPE2,TYPE3,TYPE4) \
flext_hdr* class_ ## NEW_CLASS (const flext_obj::lib_arg &arg1,const flext_obj::lib_arg &arg2,const flext_obj::lib_arg &arg3,const flext_obj::lib_arg &arg4)    \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(flext_obj::lib_class),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(ARGCAST(arg1,TYPE1),ARGCAST(arg2,TYPE2),ARGCAST(arg3,TYPE3),ARGCAST(arg4,TYPE4));      \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()   	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    flext_obj::libfun_add(NAME,(t_method)(class_ ## NEW_CLASS),&NEW_CLASS::callb_free,FLEXTTP(TYPE1),FLEXTTP(TYPE2),FLEXTTP(TYPE3),FLEXTTP(TYPE4),A_NULL); \
    NEW_CLASS::callb_setup(flext_obj::lib_class); \
}   

#endif




