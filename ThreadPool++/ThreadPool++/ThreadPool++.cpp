// ThreadPool++.cpp : Defines the entry point for the application.
//

#include "ThreadPool++.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;

	TP_CPU_CLASS * tp = new TP_CPU_CLASS();

	cout << tp->get_implementation_() << endl;
	
	auto f_ = []() { std::cout << " task thread run " << std::endl; };

	TP_Task t1;
	TP_Task t2;
	TP_Task t3;

	t1.set_tp_task_cb(f_);
	t2.set_tp_task_cb(f_);
	t3.set_tp_task_cb(f_);

	tp->enqueue_task(t1);
	cout << tp->get_task_runtime_status(t1.get_tp_task_id()) << endl;
	tp->enqueue_task(t2);
	cout << tp->get_task_runtime_status(t1.get_tp_task_id()) << endl;
	tp->enqueue_task(t3);
	cout << tp->get_task_runtime_status(t1.get_tp_task_id()) << endl;

	delete tp;

	return 0;
}
