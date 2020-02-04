
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libcfs/util/parser.h>
#include <linux/lnet/lnetctl.h>
#include "obdctl.h"
#include <linux/lustre/lustre_ver.h>
#include <lustre/lustreapi.h>
#include <wordexp.h>

// test list_param
int main(int argc, char* argv[])
{

	int rc = 0;
	char* data_buf;
	size_t sizeloc;
	FILE* ostream = open_memstream(&data_buf, &sizeloc);

	if (argv[1][0] == 'l') {
		rc = llapi_param_simple_fetch(argv[2], NULL, LIST_PARAM, ostream);
	}
	if (argv[1][0] == 'g') {
		rc = llapi_param_simple_fetch(argv[2], NULL, GET_PARAM, ostream);
	}
	if (argv[1][0] == 's') {
		printf("TEST: setting a value of 101\n");
		rc = llapi_param_simple_fetch(argv[2], "101", SET_PARAM, ostream);
	}


	fclose(ostream);
	printf("%s", data_buf);
	free(data_buf);

	return rc;
}
