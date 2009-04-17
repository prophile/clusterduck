COMPILE_FLAGS=-pipe -O3 -emit-llvm -ISource
LINK_FLAGS=-pipe -O3

.PHONY : all clean

all: Products/cduck-keygen Products/cduck Products/cduckd
	true

clean:
	find Objects -name "*.bc" | xargs rm -f

Products/cduck-keygen: Objects/KeyGen/KeyGen.bc
	clang $(LINK_FLAGS) -o $@ $^

Products/cduck: Objects/Controller/Database.bc Objects/Controller/DatabaseSchema.bc Objects/Controller/Message.bc Objects/Controller/Commands.bc Objects/Controller/Controller.bc Objects/Shared/Encrypt.bc
	clang $(LINK_FLAGS) -lcrypto -lsqlite3 -o $@ $^

Products/cduckd: Objects/Daemon/Daemon.bc Objects/Shared/Encrypt.bc
	clang $(LINK_FLAGS) -lcrypto -o $@ $^

Objects/KeyGen/KeyGen.bc: Source/KeyGen/KeyGen.c
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Database.bc: Source/Controller/Database.c Source/Controller/Database.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/DatabaseSchema.bc: Source/Controller/DatabaseSchema.c
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Message.bc: Source/Controller/Message.c Source/Controller/Message.h Source/Shared/Encrypt.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Commands.bc: Source/Controller/Commands.c Source/Controller/Commands.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Controller.bc: Source/Controller/Controller.c Source/Controller/Commands.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Daemon/Daemon.bc: Source/Daemon/Daemon.c Source/Shared/Encrypt.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Shared/Encrypt.bc: Source/Shared/Encrypt.c Source/Shared/Encrypt.h
	clang $(COMPILE_FLAGS) -c -o $@ $<
