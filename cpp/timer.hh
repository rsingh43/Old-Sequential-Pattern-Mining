#ifndef _TIMER_HH_
#define _TIMER_HH_

#include <sys/time.h>

#include <cstddef>

template <typename T=double>
class timer {
	public:
		timer() {
			tic();
		}
		
		void tic() {
			gettimeofday(&start_tv, NULL);
		}
		
		T toc() {
			struct timeval end_tv;
			gettimeofday(&end_tv, NULL);
			return ((end_tv.tv_sec - start_tv.tv_sec)*((T)1000000) + (end_tv.tv_usec - start_tv.tv_usec))/((T)1000);
		}
	
	protected:
		struct timeval start_tv;
};

#endif
