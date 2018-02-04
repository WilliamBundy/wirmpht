@echo off

ctime -begin wirmpht.ctm

if not exist bin mkdir bin
set disabled=-Wno-macro-redefined ^
	-Wno-unused-function ^
	-Wno-unused-variable ^
	-Wno-format ^
	-Wno-parentheses ^
	-D_CRT_SECURE_NO_WARNINGS

wirmpht -p -s -t -m wirmpht_main.c > wirmpht_generated.h

clang-cl ^
	/TC ^
	/Zi ^
	/MT ^
	/Gd ^
	/EHsc ^
	/W3 ^
	%disabled% ^
	wirmpht_main.c ^
	/Febin\wirmpht.exe ^
/link ^
	/SUBSYSTEM:CONSOLE ^
	/INCREMENTAL:NO 

ctime -end wirmpht.ctm
