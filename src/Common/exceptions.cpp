
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		A file that includes the exceptions header file.                            *
 * 																					*
 ************************************************************************************/

#include "exceptions.hh"

// Delete me later
void check_timeout(const Automaton &aut, timespec timeout_start, unsigned int timeout)
{
    unsigned int timeout_temp = timeout==0 ? TIMEOUT : timeout;

    timespec nowTime;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &nowTime);

    //long int time = timeout_start.tv_sec;

    if (nowTime.tv_sec - timeout_start.tv_sec > timeout_temp)
    {
        // outputText(std::to_string(nowTime.tv_sec - timeout_start.tv_sec) + " have passed -> TIMEOUT. "); Uncomment to help fine tune how often to check the timeout.
        throw timeout_(aut);
    }
}

void check_timeout(const Automaton &aut, Time& timeout_start, seconds timeout)
{
    if (timerNotStarted(timeout_start))
        return; /* Nothing to do here. */

    seconds timeout_value = (timeout<=0) ? TIMEOUT : timeout;
    //seconds time_now = secsSinceEpoch();
    //chr_time time_now = startTimer();
    //if (time_now - timeout_start >= timeout_value)
      //  throw timeout_(aut);
    if (elapsedSec(timeout_start) >= timeout_value)
        throw timeout_(aut);
}
