# Release options
#COMPILE_FLAGS=-pipe -O3 -emit-llvm -ISource
#LINK_FLAGS=-strip-all -native
#COMPILER=clang
#LINKER=llvm-ld
#WRAPUP=strip Products/* 2>/dev/null

# Debug options
COMPILE_FLAGS=-pipe -O0 -gfull -ISource
LINK_FLAGS=
COMPILER=gcc
LINKER=gcc
WRAPUP=

.PHONY : all clean

all: Products/cduck-keygen Products/cduck Products/cduckd
	$(WRAPUP)

clean:
	find Objects -name "*.bc" | xargs rm -f
	rm -f Products/*

Products/cduck-keygen: Objects/KeyGen/KeyGen.bc
	$(LINKER) $(LINK_FLAGS) -o $@ $^

Products/cduck: Objects/Controller/Database.bc Objects/Controller/DatabaseSchema.bc Objects/Controller/Message.bc Objects/Controller/Commands.bc Objects/Controller/Controller.bc Objects/Shared/Encrypt.bc Objects/Controller/StatCommands.bc Objects/Controller/ShellCommands.bc Objects/Controller/GroupCommands.bc Objects/Controller/PackageCommands.bc
	$(LINKER) $(LINK_FLAGS) -lcrypto -lsqlite3 -o $@ $^

Products/cduckd: Objects/Daemon/Daemon.bc Objects/Shared/Encrypt.bc
	$(LINKER) $(LINK_FLAGS) -lcrypto -o $@ $^

Objects/KeyGen/KeyGen.bc: Source/KeyGen/KeyGen.c
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Database.bc: Source/Controller/Database.c Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/DatabaseSchema.bc: Source/Controller/DatabaseSchema.c
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Message.bc: Source/Controller/Message.c Source/Controller/Message.h Source/Shared/Encrypt.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Commands.bc: Source/Controller/Commands.c Source/Controller/Commands.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Controller.bc: Source/Controller/Controller.c Source/Controller/Commands.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/StatCommands.bc: Source/Controller/StatCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/ShellCommands.bc: Source/Controller/ShellCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/GroupCommands.bc: Source/Controller/GroupCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/PackageCommands.bc: Source/Controller/PackageCommands.c Source/Controller/Message.h Source/Controller/Commands.h Source/Controller/Database.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Daemon/Daemon.bc: Source/Daemon/Daemon.c Source/Shared/Encrypt.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<

Objects/Shared/Encrypt.bc: Source/Shared/Encrypt.c Source/Shared/Encrypt.h
	$(COMPILER) $(COMPILE_FLAGS) -c -o $@ $<
