#ifndef __included_encrypt_h
#define __included_encrypt_h

void* cduck_encrypt ( const void* data, unsigned int inLen, unsigned int* outLen );
void* cduck_decrypt ( const void* data, unsigned int inLen, unsigned int* outLen );

#endif
