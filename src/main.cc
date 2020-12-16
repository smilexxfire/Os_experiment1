#include <os/shell.h>
#include <os/manager.h>
using namespace os;

int main(int argc, char *argv[])
{
	Shell shell;
	shell.fromFile();
	shell.repeat();
	return 0;

}
