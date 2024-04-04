
#include "common.h"

using namespace std;


int
main()
{
    setup();

    run_command("mkdir directory");
    run_command("chown nobody directory");

    run_command("echo test > file");
    run_command("chown nobody file");

    run_command("ln --symbolic test link");
    run_command("chown --no-dereference nobody link");

    first_snapshot();

    run_command("rmdir directory");
    run_command("rm file");
    run_command("rm link");

    second_snapshot();

    check_undo_statistics(3, 0, 0);

    undo();

    check_undo_errors(0, 0, 0);

    check_first();

    cleanup();

    exit(EXIT_SUCCESS);
}
