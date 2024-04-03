#include 	<stdio.h>
#include 	<string.h>
#include 	<stdlib.h>
#include	<unistd.h>

#include "wizd.h"

int main(int argc, char *argv[])
{
	global_param_init();
	config_file_read();
	if (global_param.wizd_chdir[0]) {
		chdir(global_param.wizd_chdir);
	}

	if (global_param.flag_debug_log_output == TRUE) {
		fprintf(stderr, "debug log output start..\n");
		debug_log_initialize(global_param.debug_log_filename);
		debug_log_output("\n%s boot up.", SERVER_NAME );
		debug_log_output("debug log output start..\n");
	} else {
		fprintf(stderr, "couldnt start debug log output..\n");
	}
	config_sanity_check();
	server_http_process(STDIN_FILENO);
	return 0;
}
