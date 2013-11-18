@echo off
echo.
echo Creating .ZIP file with JPEGER source
echo.
if exist jpeger_source.zip del jpeger_source.zip > nul
zip -9 jpeger_source.zip *.?pp *.h *.dlg *.rc *.txt *.cmd makefile makefile.dep
zip -9 -g jpeger_source.zip images\jpeger_startup_240x180x16_beta.bmp images\jpeger.ico
zip -9 -g jpeger_source.zip jpeg_6a\* -x *.obj

