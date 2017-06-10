/*
 *  NAME:
 *      usleep     -- This is the precision timer for Test Set
 *                    Automation. It uses the select(2) system
 *                    call to delay for the desired number of
 *                    micro-seconds. This call returns ZERO
 *                    (which is usually ignored) on successful
 *                    completion, -1 otherwise.
 *
 *  ALGORITHM:
 *      1) We range check the passed in microseconds and log a
 *         warning message if appropriate. We then return without
 *         delay, flagging an error.
 *      2) Load the Seconds and micro-seconds portion of the
 *         interval timer structure.
 *      3) Call select(2) with no file descriptors set, just the
 *         timer, this results in either delaying the proper
 *         ammount of time or being interupted early by a signal.
 *
 *  HISTORY:
 *      Added when the need for a subsecond timer was evident.
 *      Modified for Solaris-specific bits by SAM 24/10/96
 *  AUTHOR:
 *      Michael J. Dyer           Telephone:   AT&T 414.647.4044
 *      General Electric Medical Systems    GE DialComm  8 *767.4044
 *      P.O. Box 414  Mail Stop 12-27     Sect'y   AT&T 414.647.4584
 *      Milwaukee, Wisconsin  USA 53201              8 *767.4584
 *      internet:  mike@sherlock.med.ge.com     GEMS WIZARD e-mail: DYER
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/param.h>
#include <sys/types.h>

int usleep_new(unsigned long microSeconds)
{
	unsigned int Seconds, uSec;
	int nfds;
	struct timeval Timer;

#if (defined(SOLARIS) || (defined(SGI)))
	fd_set readfds, writefds, exceptfds;

	nfds = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
#else
	int readfds, writefds, exceptfds;
	nfds = readfds = writefds = exceptfds = 0;
#endif

	if ((microSeconds == (unsigned long)0)
		|| microSeconds > (unsigned long)4000000) {
		errno = ERANGE;			/* value out of range */
		perror("usleep time out of range ( 0 -> 4000000 ) ");
		return -1;
	}

	Seconds = microSeconds / (unsigned long)1000000;
	uSec = microSeconds % (unsigned long)1000000;

	Timer.tv_sec = Seconds;
	Timer.tv_usec = uSec;

	if (select(nfds, &readfds, &writefds, &exceptfds, &Timer) < 0) {
		perror("usleep (select) failed");
		return -1;
	}

	return 0;
}
