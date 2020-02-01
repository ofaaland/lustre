
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


// a hack to get around a circular dependency
// jt_lcfg_listparam() is in luster_cfg.c
// luster_cfg.c calls functions from obd.c
// obd.c gets cmdlist from lctl.c
// lctl.c can't be included in the source files because it also has a main()
// so here is a dummy cmdlist for obd.o to link to
command_t cmdlist[] = {{"===== metacommands =======", NULL, 0, "metacommands"}};


// test list_param
int main(int argc, char* argv[])
{

	int rc = 0;
	char* data_buf;
	size_t sizeloc;
	FILE* ostream = open_memstream(&data_buf, &sizeloc);

	if (argv[1][0] == 'l') {
		rc = llapi_param_simple_fetch(argv[2], LIST_PARAM, ostream);
	}
	if (argv[1][0] == 'g') {
		rc = llapi_param_simple_fetch(argv[2], GET_PARAM, ostream);
	}

	fclose(ostream);
	printf("%s", data_buf);
	free(data_buf);

	return rc;
}
