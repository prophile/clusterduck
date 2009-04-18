# Release options, with LLVM
COMPILE_FLAGS=-pipe -O3 -emit-llvm -ISource
LINK_FLAGS=-native
COMPILER=clang
LINKER=llvm-ld
WRAPUP=strip Products/* 2>/dev/null

# Release options, without LLVM
#COMPILE_FLAGS=-pipe -O3 -ISource
#LINK_FLAGS=
#COMPILER=gcc
#LINKER=gcc
#WRAPUP=strip Products/* 2>/dev/null

# Release options, without LLVM, with ccache
#COMPILE_FLAGS=-pipe -O3 -ISource
#LINK_FLAGS=
#COMPILER=ccache gcc
#LINKER=gcc
#WRAPUP=strip Products/* 2>/dev/null

# Debug options
#COMPILE_FLAGS=-pipe -O0 -gfull -ISource
#LINK_FLAGS=
#COMPILER=gcc
#LINKER=gcc
#WRAPUP=

.PHONY : all clean

all: Products/cduck-keygen Products/cduck Products/cduckd
	$(WRAPUP)

clean:
	find Objects -name "*.o" | xargs rm -f
	rm -f Products/*

Products/cduck-keygen: Objects/KeyGen/KeyGen.o
	$(LINKER) $(LINK_FLAGS) -o $@ $^

Products/cduck: Objects/Controller/Database.o Objects/Controller/DatabaseSchema.o Objects/Controller/Message.o Objects/Controller/Commands.o Objects/Controller/Controller.o Objects/Shared/Encrypt.o Objects/Controller/StatCommands.o Objects/Controller/ShellCommands.o Objects/Controller/GroupCommands.o Objects/Controller/PackageCommands.o
	$(LINKER) $(LINK_FLAGS) -lcrypto -lsqlite3 -o $@ $^

Products/cduckd: Objects/Daemon/Daemon.o Objects/Shared/Encrypt.o Objects/Daemon/Dispatcher.o Objects/Daemon/Dispatchers.o
	$(LINKER) $(LINK_FLAGS) -lcrypto -o $@ $^

Objects/KeyGen/KeyGen.o: Source/KeyGen/KeyGen.c
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Database.o: Source/Controller/Database.c Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/DatabaseSchema.o: Source/Controller/DatabaseSchema.c
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Message.o: Source/Controller/Message.c Source/Controller/Message.h Source/Shared/Encrypt.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Commands.o: Source/Controller/Commands.c Source/Controller/Commands.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Controller.o: Source/Controller/Controller.c Source/Controller/Commands.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/StatCommands.o: Source/Controller/StatCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/ShellCommands.o: Source/Controller/ShellCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/GroupCommands.o: Source/Controller/GroupCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/PackageCommands.o: Source/Controller/PackageCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Daemon/Daemon.o: Source/Daemon/Daemon.c Source/Shared/Encrypt.h Source/Daemon/Dispatcher.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Daemon/Dispatcher.o: Source/Daemon/Dispatcher.c Source/Daemon/Dispatcher.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Daemon/Dispatchers.o: Source/Daemon/Dispatchers.c Source/Daemon/Dispatcher.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Shared/Encrypt.o: Source/Shared/Encrypt.c Source/Shared/Encrypt.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<
