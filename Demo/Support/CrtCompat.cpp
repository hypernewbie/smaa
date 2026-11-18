//--------------------------------------------------------------------------------------
// CrtCompat.cpp
//
// Compatibility shim for building this legacy code base with a modern toolchain.
//
// The DirectX SDK (June 2010) ships dxerr.lib, which was built with Visual Studio 2008
// and references the CRT internal _vsnwprintf. That function still exists in the UCRT,
// but only as a header inline, so nothing emits it and the linker fails with:
//
//   dxerr.lib(dxerrw.obj) : error LNK2001: unresolved external symbol __vsnwprintf
//
// Taking the address of the inline below forces the compiler to emit an out-of-line
// copy into this translation unit, which resolves the reference. The UCRT's inline
// has the same legacy semantics as the VS2008 one (null terminated output, -1 on
// truncation), so behaviour is unchanged.
//
// The name is '_vsnwprintf' (one leading underscore) because that is what the VS2008
// CRT used: x86 decorates it to '__vsnwprintf', the symbol dxerr.lib needs, while
// x64 emits '_vsnwprintf', matching dxerr.lib's x64 build.
//
// This is the only symbol the June 2010 dxerr.lib needs that modern MSVC no longer
// provides; everything else it uses (_vsnprintf, __chkstk, __security_cookie) is
// still available.
//--------------------------------------------------------------------------------------

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#pragma warning(push)
// The UCRT marks _vsnwprintf as deprecated, but it is exactly the legacy
// implementation dxerr.lib was compiled against.
#pragma warning(disable : 4996)
extern "C" int (*const p_vsnwprintf)(wchar_t *, size_t, const wchar_t *, va_list) = &_vsnwprintf;
#pragma warning(pop)
