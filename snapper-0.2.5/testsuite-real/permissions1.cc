
#include "common.h"

using namespace std;


int
main()
{
    setup();

    run_command("mkdir --mode a=rwx directory1");
    run_command("mkdir --mode a=--- directory2");

    run_command("echo test > file1");
    run_command("chmod a=rwx file1");
    run_command("echo test > file2");
    run_command("chmod a=--- file2");

    first_snapshot();

    run_command("rmdir directory1");
    run_command("rmdir directory2");

    run_command("rm file1");
    run_command("rm file2");

    second_snapshot();

    check_undo_statistics(4, 0, 0);

    undo();

    check_undo_errors(0, 0, 0);

    check_first();

    cleanup();

    exit(EXIT_SUCCESS);
}
