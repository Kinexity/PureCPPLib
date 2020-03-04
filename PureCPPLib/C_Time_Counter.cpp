#include "C_Time_Counter.h"

PCL::C_Time_Counter& tc() {
	static PCL::C_Time_Counter _tc_;
	return _tc_;
}