//===-- programs.cpp ------------------------------------------------------===//
//
//                         LDC – the LLVM D compiler
//
// This file is distributed under the BSD-style LDC license. See the LICENSE
// file for details.
//
//===----------------------------------------------------------------------===//

#include "gen/programs.h"
#include "mars.h"       // fatal()
#include "root.h"       // error(char*)
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Program.h"

using namespace llvm;

static cl::opt<std::string> gcc("gcc",
    cl::desc("GCC to use for assembling and linking"),
    cl::Hidden,
    cl::ZeroOrMore);

static cl::opt<std::string> ar("ar",
    cl::desc("Archiver"),
    cl::Hidden,
    cl::ZeroOrMore);

static cl::opt<std::string> mslink("ms-link",
    cl::desc("LINK to use for linking on Windows"),
    cl::Hidden,
    cl::ZeroOrMore);

static cl::opt<std::string> mslib("ms-lib",
    cl::desc("Library Manager to use on Windows"),
    cl::Hidden,
    cl::ZeroOrMore);

#if LDC_LLVM_VER >= 304
typedef std::string RetType;
#else
typedef sys::Path RetType;
#endif

RetType getProgram(const char *name, const cl::opt<std::string> &opt, const char *envVar = 0)
{
    RetType path;
    const char *prog = NULL;

    if (opt.getNumOccurrences() > 0 && opt.length() > 0 && (prog = opt.c_str()))
#if LDC_LLVM_VER >= 304
        path = sys::FindProgramByName(prog);
#else
        path = sys::Program::FindProgramByName(prog);
#endif

    if (path.empty() && envVar && (prog = getenv(envVar)))
#if LDC_LLVM_VER >= 304
        path = sys::FindProgramByName(prog);
#else
        path = sys::Program::FindProgramByName(prog);
#endif

    if (path.empty())
#if LDC_LLVM_VER >= 304
        path = sys::FindProgramByName(name);
#else
        path = sys::Program::FindProgramByName(name);
#endif

    if (path.empty()) {
        error("failed to locate %s", name);
        fatal();
    }

    return path;
}

RetType getGcc()
{
    return getProgram("gcc", gcc, "CC");
}

RetType getArchiver()
{
    return getProgram("ar", ar);
}

RetType getLink()
{
    return getProgram("link.exe", mslink);
}

RetType getLib()
{
    return getProgram("lib.exe", mslib);
}
