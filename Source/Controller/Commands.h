#ifndef __included_commands_h
#define __included_commands_h

typedef struct _cduck_command cduck_command;

typedef void (*cduck_command_handler)(const cduck_command* command, int argc, const char** argv);

struct _cduck_command
{
	void* udata;
	const char* name;
	unsigned short id;
	cduck_command_handler handler;
	// internal use
	cduck_command* next;
	unsigned int hash;
};

void cduck_register_command ( const cduck_command* command );
void cduck_dispatch_command ( const char* name, int argc, const char** argv );

#define CDUCK_REGISTER_COMMAND_STATIC(name, id, handler, udata) void __cduck_command_register_##name () __attribute__((constructor)); \
void __cduck_command_register_##name () { struct _cduck_command cmd = { udata, #name, id, handler, 0, 0 }; cduck_register_command(&cmd); }

#endif
