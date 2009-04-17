#include "Commands.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void print_usage_and_exit ( const char* invocation )
{
	printf("Usage: %s [command] [arguments]\n", invocation);
	printf("Available commands:\n");
	cduck_command_print_usage();
	printf("\n");
	exit(0);
}

static void print_version_and_exit ()
{
	printf("ClusterDuck controller version 0.0.1\nCopyright (c) 2009 Alistair Lynn\n\n");
	exit(0);
}

int main ( int argc, char** argv )
{
	const char* op;
	const char** arg_array;
	int i, narg;
	int rc;
	if (argc == 1)
	{
		print_usage_and_exit(argv[0]);
	}
	op = argv[1];
	if (!strcmp("op", "-h") ||
	    !strcmp("op", "-help") ||
	    !strcmp("op", "--help"))
	{
		print_usage_and_exit(argv[0]);
	}
	else if (!strcmp("op", "-v") ||
	         !strcmp("op", "-V") ||
	         !strcmp("op", "-version") ||
	         !strcmp("op", "--version"))
	{
		print_version_and_exit();
	}
	else
	{
		narg = argc - 2;
		arg_array = (const char**)malloc(sizeof(const char*) * narg);
		for (i = 0; i < narg; i++)
		{
			arg_array[i] = argv[i + 2];
		}
		rc = cduck_dispatch_command(op, narg, arg_array);
		return rc;
	}
	return 0;
}
