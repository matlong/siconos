
// SWIG interface for Siconos Mechanics/joints
%module(directors="1", allprotected="1") joints

%include start.i

%{
#include <SiconosKernel.hpp>
#include <KneeJointR.hpp>
#include <PivotJointR.hpp>
#include <PrismaticJointR.hpp>
%}

%include FrontEndConfig.h;

// common declarations

%include handleException.i

%import Kernel.i

%include pyRegister.i

PY_REGISTER(KneeJointR);
PY_REGISTER(PivotJointR);
PY_REGISTER(PrismaticJointR);