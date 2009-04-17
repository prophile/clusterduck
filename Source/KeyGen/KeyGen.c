#include <stdio.h>

int main ( int argc, char** argv )
{
	FILE* keyFP;
	FILE* randFP;
	unsigned char tmpBuf[32 + 16];
	if (keyFP = fopen("/etc/cduck.key", "r"))
	{
		fclose(keyFP);
		fprintf(stderr, "Refusing to overwrite existing /etc/cduck.key\n");
		return 1;
	}
	keyFP = fopen("/etc/cduck.key", "w");
	if (!keyFP)
	{
		fprintf(stderr, "Failed to open /etc/cduck.key for reading\n");
		return 1;
	}
	randFP = fopen("/dev/random", "r");
	if (!randFP)
	{
		fclose(keyFP);
		fprintf(stderr, "Failed to open /dev/random for reading\n");
		return 1;
	}
	fread(tmpBuf, sizeof(tmpBuf), 1, randFP);
	fwrite(tmpBuf, sizeof(tmpBuf), 1, keyFP);
	fclose(keyFP);
	fclose(randFP);
	return 0;
}
