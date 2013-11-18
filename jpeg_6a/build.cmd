@echo off
del *.obj > nul
for %%a in (j*.c) do icc /Tdp /O+ /Ti- /Gm+ /Gd- /G5 /C+ %%a
copy *.obj ..

