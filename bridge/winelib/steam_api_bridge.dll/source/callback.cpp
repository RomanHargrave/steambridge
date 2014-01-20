// callback.cpp - Handles callback API calls

#include <cstdio>

#include <steam_api.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <wine/debug.h>

#include "api.h"
#include "core.h"
#include "logging.h"


WINE_DEFAULT_DEBUG_CHANNEL(steam_bridge);

//class CallbackImpl : protected CCallbackBase
// TODO: It's unclear whether making the base public (over private
//       or protected in the steam headers) has any impact.  At the moment,
//       it doesn't seem like it should from libsteam_api.so's perspective.
//
class CallbackImpl : protected CCallbackBase
{
  public:
    CallbackImpl(steam_bridge_CallbackRunFunc run, 
        steam_bridge_CallbackRunArgsFunc runargs, void *cCallbackBase, int size);
    virtual void Run(void *pvParam);
    virtual void Run(void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall);
    virtual int GetCallbackSizeBytes();

  private:
    // Pointer to the win32 CCallbackBase object
    void *cCallbackBase;
    // Size of the callback structure
    int size;

    // Function pointers to the win32 location to run callbacks
    steam_bridge_CallbackRunFunc run;
    steam_bridge_CallbackRunArgsFunc runargs;

    friend int steam_bridge_SteamAPI_RegisterCallback(
        steam_bridge_CallbackRunFunc, steam_bridge_CallbackRunArgsFunc,
        void *, int, int);
};

CallbackImpl::CallbackImpl(steam_bridge_CallbackRunFunc run,
    steam_bridge_CallbackRunArgsFunc runargs, void *cCallbackBase, int size) 
  : CCallbackBase(), cCallbackBase(cCallbackBase), size(size), run(run), runargs(runargs)
{
  m_iCallback = 0;
  m_nCallbackFlags = 0;
}

// A now for a word or two on the Callback function pointers:
// 
// Real steam_api.dll uses a simple virtual function for the callbacks.
// This doesn't work across the Wine->Linux boundary.  Interestingly enough,
// the Win32 code running in Wine calls the correct Linux virtual function,
// but thiscall is considerably different between GCC and Visual Studio.
// Noticeably, Visual Studio uses callee and LTR (!) arguments, and GCC uses
// C-style caller and RTL arguments.  While the right function executes,
// the arguments are messed up and it's unlikely you'd be able to get the
// function to return correctly.
//
// This necessitates the two different SteamBridge DLLs, as the proxy
// DLL is compiled under Visual Studio and offers the wrapping C++ functions
// compiled in the same form as the real DLL, versus this bridge library that
// compiles classes in the GCC/Linux fashion for the Linux steam_api.so.
// For most functions, a wrapping C interface is fine, if slightly painful
// to write all the glue code.  However, there's isn't a super logical way
// to call functions on the Wine side from the Winelib DLL.  Winelib DLLs
// are effectively entirely Linux code, and cannot link against Win32 DLLs.
// Fortunately, C-style calls are nearly identical on GCC/Linux and VS/Win32.
// At a glance of various Internet documents, it seems like returning
// values would be trouble, but arguments are handled in the same way.
// Note that GCC has a 16-byte stack alignment behavior that Visual Studio
// may or may not have.  Thanks to caller cleanup and the simple nature of
// the callback arguments (pointers, ints, and int64) this shouldn't cause
// any trouble either way.
//
// So in short, this callback method is an ugly, ugly, ugly, ugly method
// that depends heavily on somewhat coincidental behavior between Linux
// and Windows.  It's extremely brittle, and liable to break into a
// million pieces if you so much look at it the wrong way, or really think
// about it.
//
// On a side note, despite the "Let's take a moment and fix a few annoying
// things, such as the lack of general purpose registers" nature of x86-64,
// the default conventions are still different in the magical 64-bit land.
// Microsoft went with a stdcall style LTR argument order, but cdecl style
// caller cleanup.  GCC uses straight cdecl.  Wheee...


void CallbackImpl::Run(void *pvParam)
{
  WINE_TRACE("(this=0x%p,pvParam=0x%p)", this, pvParam);
  (*run)(cCallbackBase, m_nCallbackFlags, pvParam);
}

void CallbackImpl::Run(void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall)
{
  WINE_TRACE("(this=0x%p,pvParam=0x%p,bIOFailure=%i,hSteamAPICall=%llu)", this, pvParam, bIOFailure, hSteamAPICall);
  (*runargs)(cCallbackBase, m_nCallbackFlags, pvParam, bIOFailure, hSteamAPICall);
}

int CallbackImpl::GetCallbackSizeBytes()
{
  // TODO: It's possible this value changes in a custom implementation
  //       of CCallbackBase, though as a wild guess this isn't used in pratice.
  //       The two (technically three) implementations in steam_api.h return
  //       sizeof on a class - which will be constant.
  //
  // TODO: Update, said class is one of the many callback structs that
  //       are used.  It's possible, but unlikely, that if a client
  //       reused a CCallbackBase object this could change.  Said
  //       developers should probably be slapped.  Nonetheless, it's
  //       worth at least catching and aborting in this behavior.
  //
  return size;
}

extern "C"
{

void steam_bridge_SteamAPI_RunCallbacks()
{
  // Note that this appears to be purely single threaded event checking.
  // All Run(...) executions occur while SteamAPI_RunCallbacks()
  // is executing.  It's possible (though unideal) to 'reque' all fired
  // callbacks and return the data in some form from this bridge function.
  // This may be necessary if calling the parent win32 proxy library via
  // function pointers isn't possible (see above notes/implementation about
  // cross-OS C-style function calls).
  WINE_TRACE("()");
  SteamAPI_RunCallbacks();
}

int steam_bridge_SteamAPI_RegisterCallback(steam_bridge_CallbackRunFunc run, 
    steam_bridge_CallbackRunArgsFunc runargs, void *cCallbackBase, int callback, 
    int size)
{
  WINE_TRACE("(0x%p,0x%p,0x%p,%i,%i)", run, runargs, cCallbackBase, callback, size);

  // TODO: If this object has already been Registered, we should catch
  //       and handle this.  If it's not unregistered, probably unregister
  //       it.  Unregistering is the ideal place to delete the wrapper,
  //       so we'll then recreate it here.

  if (!context)
    __ABORT__("No context (Init not called, or failed)!");

  // To make the behavior clear, flags is bits settings that track the
  // state of the callback.  There's only two: 0x1 records whether it's
  // registered, 0x2 records whether it's a game server callback (the exact
  // behavior of this isn't clear yet).  iCallback refers to the callback
  // number, and it's set by SteamAPI_RegisterCallback.  This corresponds to
  // one of the callback structs defined all over the place, which contain
  // a field call k_iCallback (==iCallback).  This is template 'class P',
  // and is passed as the first parameter in the two Run(...) functions.
  // GetCallbackSizeBytes returns the size of this callback struct.
  // Fortunately for us, as it's pure data there shouldn't be any need to
  // do any converting between Linux and Wine.

  CallbackImpl *c = new CallbackImpl(run, runargs, cCallbackBase, size);

  __LOG_ARGS_MSG__("Logging wrapper for callback", 
      "(0x%p,%i,%i)->(0x%p,callback=%i,flags=%i)", cCallbackBase, callback,
      size, c, c->m_iCallback, c->m_nCallbackFlags);

  context->addCallback(c);
  SteamAPI_RegisterCallback(c, callback);

  __LOG_ARGS_MSG__("After SteamAPI_RegisterCallback",
      "(wrapper=0x%p,base=0x%p,callback=%i,flags=%i)", c, cCallbackBase,
      c->m_iCallback, c->m_nCallbackFlags);

  if (c->m_iCallback != callback)
    __ABORT_ARGS__("Callback doesn't match expected after RegisterCallback!",
        "(wrapper=0x%p,base=0x%p,expected=%i,actual=%i)", c, cCallbackBase,
        callback, c->m_iCallback);

  return c->m_nCallbackFlags;
}

} // extern "C"


