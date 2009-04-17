COMPILE_FLAGS=-pipe -O3 -emit-llvm -ISource
LINK_FLAGS=-pipe -O3

.PHONY : all clean

all: Products/cduck-keygen Products/cduck-db-test Products/cduckd
	true

clean:
	find Objects -name "*.bc" | xargs rm -f

Products/cduck-keygen: Objects/KeyGen/KeyGen.bc
	clang $(LINK_FLAGS) -o $@ $^

Products/cduck-db-test: Objects/Controller/Database.bc Objects/Controller/DatabaseSchema.bc Objects/Controller/Test.bc
	clang $(LINK_FLAGS) -lsqlite3 -o $@ $^

Products/cduckd: Objects/Daemon/Daemon.bc Objects/Shared/Encrypt.bc
	clang $(LINK_FLAGS) -lcrypto -o $@ $^

Objects/KeyGen/KeyGen.bc: Source/KeyGen/KeyGen.c
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Database.bc: Source/Controller/Database.c Source/Controller/Database.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/DatabaseSchema.bc: Source/Controller/DatabaseSchema.c
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Message.bc: Source/Controller/Message.c Source/Shared/Message.h Source/Shared/Encrypt.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Commands.bc: Source/Controller/Commands.c Source/Controller/Commands.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Controller/Test.bc: Source/Controller/Test.c Source/Controller/Database.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Daemon/Daemon.bc: Source/Daemon/Daemon.c Source/Shared/Encrypt.h
	clang $(COMPILE_FLAGS) -c -o $@ $<

Objects/Shared/Encrypt.bc: Source/Shared/Encrypt.c Source/Shared/Encrypt.h
	clang $(COMPILE_FLAGS) -c -o $@ $<
