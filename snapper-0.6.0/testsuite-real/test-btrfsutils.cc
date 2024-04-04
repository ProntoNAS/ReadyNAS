

/*
 * Utility program to test functions in BtrfsUtils. You have to adapt the
 * parameters, e.g. mount point and qgroup ids, and enable specific tests
 * before using this program.
 */


#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "snapper/BtrfsUtils.h"


using namespace std;
using namespace snapper;
using namespace BtrfsUtils;


int
main()
{
    int fd = open("/btrfs", O_NOATIME);
    if (fd < 0)
    {
	cerr << "open failed (" << strerror(errno) << ")" << endl;
	return EXIT_FAILURE;
    }


    if (false)
    {
	quota_enable(fd);
    }


    if (false)
    {
	quota_disable(fd);
    }


    if (false)
    {
	quota_rescan(fd);
    }


    if (false)
    {
	qgroup_create(fd, parse_qgroup("1/0"));
    }


    if (false)
    {
	qgroup_destroy(fd, parse_qgroup("1/0"));
    }


    if (false)
    {
	qgroup_assign(fd, parse_qgroup("0/123"), parse_qgroup("1/0"));
    }


    if (false)
    {
	qgroup_remove(fd, parse_qgroup("0/123"), parse_qgroup("1/0"));
    }


    if (false)
    {
	cout << "qgroup_query_usage" << endl;

	qgroup_t qgroup = parse_qgroup("1/0");
	QGroupUsage qgroup_usage = qgroup_query_usage(fd, qgroup);
	cout << "referenced:" << qgroup_usage.referenced << endl;
	cout << "referenced_compressed:" << qgroup_usage.referenced_compressed << endl;
	cout << "exclusive:" << qgroup_usage.exclusive << endl;
	cout << "exclusive_compressed:" << qgroup_usage.exclusive_compressed << endl;
    }


    if (false)
    {
	cout << "qgroup_find_free" << endl;

	qgroup_t qgroup = qgroup_find_free(fd, 1);
	cout << format_qgroup(qgroup) << endl;
    }


    if (false)
    {
	cout << "qgroup_query_children" << endl;

	vector<qgroup_t> children = qgroup_query_children(fd, parse_qgroup("1/0"));
	for (qgroup_t child : children)
	    cout << format_qgroup(child) << " ";
	cout << endl;
    }

    close(fd);
}
