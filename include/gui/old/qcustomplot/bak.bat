@echo off
::set path="C:\Program Files\WinRAR\";%path%

for %%a in (%CD%) do (
Rar.exe a "%%~na_%date:/=_%.rar" -r *.m *.java *.reg *.cs *.h *.c *.cpp *.hpp *.inl *.asm *.dsp makefile *.mak *.mk *.js *.bat *.m *.doc *.pdf *.rc *.sh *.xdc *.xml *.cfg *.ini *.md
)
