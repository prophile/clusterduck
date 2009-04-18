#ifndef __included_dispatcher_h
#define __included_dispatcher_h

void* cduck_dispatch ( unsigned short msgid, const void* data, unsigned int len, unsigned int* outLen );
typedef void* (*cduck_dispatcher)(unsigned short msgid, const void* data, unsigned int len, unsigned int* outLen);
void cduck_register_dispatcher ( unsigned short msgid, cduck_dispatcher dispatcher );

#define CDUCK_REGISTER_DISPATCHER_STATIC(name, id, handler) void __cduck_dispatcher_register_##name () __attribute__((constructor));\
void __cduck_dispatcher_register_##name () { cduck_register_dispatcher(id, handler); }

#endif
