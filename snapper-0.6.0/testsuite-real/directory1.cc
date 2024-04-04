
#include "common.h"

using namespace std;


int
main()
{
    setup();

    run_command("mkdir already-here");

    first_snapshot();

    run_command("rmdir already-here");

    second_snapshot();

    run_command("mkdir already-here");

    check_undo_statistics(1, 0, 0);

    undo();

    check_undo_errors(0, 0, 0);

    check_first();

    cleanup();

    exit(EXIT_SUCCESS);
}
