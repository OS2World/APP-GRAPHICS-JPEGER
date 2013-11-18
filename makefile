# makefile
# Created by IBM WorkFrame/2 MakeMake at 11:23:16 on 7 Sept 1998
#
# The actions included in this make file are:
#  Compile::Resource Compiler
#  Compile::C++ Compiler (OCL -- Static)
#  Link::Linker (OCL/UICL)
#  Bind::Resource Bind

.SUFFIXES:

.SUFFIXES: \
    .cpp .obj .rc .res

.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r -x2 -DIC_PM %s %|dpfF.RES

{.}.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r -x2 -DIC_PM %s %|dpfF.RES

.cpp.obj:
    @echo " Compile::C++ Compiler (OCL -- Static) "
    icc.exe /Q /O+ /Gm /G5 /C /I.\jpeg_6a %s

{.}.cpp.obj:
    @echo " Compile::C++ Compiler (OCL -- Static) "
    icc.exe /Q /O+ /Gm /G5 /C /I.\jpeg_6a %s

.\jpeger.exe: \
    .\JUTILS.obj \
    .\jpeger_resize.obj \
    .\jpeger_banner.obj \
    .\jpeger_colourcube.obj \
    .\jpeger_compression.obj \
    .\jpeger_decompression.obj \
    .\jpeger_main.obj \
    .\JCAPIMIN.obj \
    .\JCAPISTD.obj \
    .\JCCOEFCT.obj \
    .\JCCOLOR.obj \
    .\JCDCTMGR.obj \
    .\JCHUFF.obj \
    .\JCINIT.obj \
    .\JCMAINCT.obj \
    .\JCMARKER.obj \
    .\JCMASTER.obj \
    .\JCOMAPI.obj \
    .\JCPARAM.obj \
    .\JCPHUFF.obj \
    .\JCPREPCT.obj \
    .\JCSAMPLE.obj \
    .\JCTRANS.obj \
    .\JDAPIMIN.obj \
    .\JDAPISTD.obj \
    .\JDATADST.obj \
    .\JDATASRC.obj \
    .\JDCOEFCT.obj \
    .\JDCOLOR.obj \
    .\JDDCTMGR.obj \
    .\JDHUFF.obj \
    .\JDINPUT.obj \
    .\JDMAINCT.obj \
    .\JDMARKER.obj \
    .\JDMASTER.obj \
    .\JDMERGE.obj \
    .\JDPHUFF.obj \
    .\JDPOSTCT.obj \
    .\JDSAMPLE.obj \
    .\JDTRANS.obj \
    .\JERROR.obj \
    .\JFDCTFLT.obj \
    .\JFDCTFST.obj \
    .\JFDCTINT.obj \
    .\JIDCTFLT.obj \
    .\JIDCTFST.obj \
    .\JIDCTINT.obj \
    .\JIDCTRED.obj \
    .\JMEMMGR.obj \
    .\JMEMNOBS.obj \
    .\JQUANT1.obj \
    .\JQUANT2.obj \
    .\jpeger.res
    @echo " Link::Linker (OCL/UICL) "
    @echo " Bind::Resource Bind "
    icc.exe @<<
    /Tdp /Q
     /O+ /Gm /G5
     /Q /B" /de /exepack:2 /pmtype:pm /packd /optfunc     "
     /Fejpeger.exe
     .\JUTILS.obj
     .\jpeger_resize.obj
     .\jpeger_banner.obj
     .\jpeger_colourcube.obj
     .\jpeger_compression.obj
     .\jpeger_decompression.obj
     .\jpeger_main.obj
     .\JCAPIMIN.obj
     .\JCAPISTD.obj
     .\JCCOEFCT.obj
     .\JCCOLOR.obj
     .\JCDCTMGR.obj
     .\JCHUFF.obj
     .\JCINIT.obj
     .\JCMAINCT.obj
     .\JCMARKER.obj
     .\JCMASTER.obj
     .\JCOMAPI.obj
     .\JCPARAM.obj
     .\JCPHUFF.obj
     .\JCPREPCT.obj
     .\JCSAMPLE.obj
     .\JCTRANS.obj
     .\JDAPIMIN.obj
     .\JDAPISTD.obj
     .\JDATADST.obj
     .\JDATASRC.obj
     .\JDCOEFCT.obj
     .\JDCOLOR.obj
     .\JDDCTMGR.obj
     .\JDHUFF.obj
     .\JDINPUT.obj
     .\JDMAINCT.obj
     .\JDMARKER.obj
     .\JDMASTER.obj
     .\JDMERGE.obj
     .\JDPHUFF.obj
     .\JDPOSTCT.obj
     .\JDSAMPLE.obj
     .\JDTRANS.obj
     .\JERROR.obj
     .\JFDCTFLT.obj
     .\JFDCTFST.obj
     .\JFDCTINT.obj
     .\JIDCTFLT.obj
     .\JIDCTFST.obj
     .\JIDCTINT.obj
     .\JIDCTRED.obj
     .\JMEMMGR.obj
     .\JMEMNOBS.obj
     .\JQUANT1.obj
     .\JQUANT2.obj
<<
    rc.exe .\jpeger.res jpeger.exe

!include "makefile.DEP"
