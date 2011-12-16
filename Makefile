all : vimspeak.dll

vimspeak.dll : vimspeak.c
	gcc -shared -o vimspeak.dll vimspeak.c -loleaut32 -lole32 -luuid
