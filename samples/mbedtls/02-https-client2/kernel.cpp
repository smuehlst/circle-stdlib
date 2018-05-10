//
// kernel.cpp
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
#include <stdio.h>

extern "C" int tlsmain (int argc, char **argv);

CKernel::CKernel (void)
        : CStdlibAppNetwork ("https-client2")
{
	mActLED.Blink (5);	// show we are alive
}

CStdlibApp::TShutdownMode CKernel::Run (void)
{
	char *argv[] = {"https-client2",
#if 1
			"server_name=elinux.org",
			"request_page=/Main_Page",
#else
			"server_name=www.raspberrypi.org",
			"request_page=/forums/app.php/feed/forum/72",
#endif
			"server_port=443",
			"auth_mode=none",
			//"debug_level=1",
			//"nbio=1",
			0};

	int ret = tlsmain (sizeof argv/sizeof argv[0]-1, argv);

	printf ("tlsmain() returned %d\n", ret);

	mScheduler.Sleep (60);		// allow network subsystem to close connections

	return ShutdownHalt;
}
