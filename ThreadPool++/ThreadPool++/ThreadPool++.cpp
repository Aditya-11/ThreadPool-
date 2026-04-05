// ThreadPool++.cpp : Defines the entry point for the application.
//

#include "ThreadPool++.h"

using namespace std;

using namespace TP;

int main()
{
	cout << "Hello CMake." << endl;

	TP::TP_CPU_CLASS * tp = new TP::TP_CPU_CLASS();

	cout << tp->get_implementation_() << endl;

	// task function run
		
	auto f_ = [](TP::tp_task_input_ptr input, TP::tp_task_output_ptr output) {
			
			if (input == nullptr || output == nullptr) return;

			int x = 0;

			int* input_val = static_cast <int*> (input);
			int* output_val = static_cast <int*> (output);

			while (input_val != nullptr && x < *input_val) {
				std::this_thread::sleep_for(std::chrono::milliseconds(3));
				x++;
			}

			*output_val = x;
		};

	TP::TP_Task t1;
	TP::TP_Task t2;
	TP::TP_Task t3;
	TP::TP_Task t4;
	TP::TP_Task t5;

	t1.set_tp_task_cb(f_);
	t2.set_tp_task_cb(f_);
	t3.set_tp_task_cb(f_);
	t4.set_tp_task_cb(f_);
	t5.set_tp_task_cb(f_);

	tp_task_input_ptr t1_input = new int(100); 
	tp_task_input_ptr t2_input = new int(200);
	tp_task_input_ptr t3_input = new int(300);
	tp_task_input_ptr t4_input = new int(400);

	tp_task_output_ptr t1_output = new int(0);
	tp_task_output_ptr t2_output = new int(0);
	tp_task_output_ptr t3_output = new int(0);
	tp_task_output_ptr t4_output = new int(0);

	tp_task_input_ptr t5_input = new int(500);
	tp_task_output_ptr t5_output = new int(0);

	t1.set_tp_task_input_ptr(t1_input);
	t2.set_tp_task_input_ptr(t2_input);
	t3.set_tp_task_input_ptr(t3_input);
	t4.set_tp_task_input_ptr(t4_input);
	t5.set_tp_task_input_ptr(t5_input);

	t1.set_tp_task_output_ptr(t1_output);
	t2.set_tp_task_output_ptr(t2_output);
	t3.set_tp_task_output_ptr(t3_output);
	t4.set_tp_task_output_ptr(t4_output);
	t5.set_tp_task_output_ptr(t5_output);

	tp->enqueue_task(t1);
	cout << tp->get_task_runtime_status(t1.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(250));
	tp->enqueue_task(t2);
	cout << tp->get_task_runtime_status(t2.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(250));
	tp->enqueue_task(t3);
	cout << tp->get_task_runtime_status(t3.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(250));
	tp->enqueue_task(t4);
	cout << tp->get_task_runtime_status(t4.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(250));
	tp->enqueue_task(t5);
	cout << tp->get_task_runtime_status(t5.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(250));

	//cout << tp->get_task_runtime_status(t5.get_tp_task_id()) << endl;

	//std::this_thread::sleep_for(std::chrono::seconds(7));

	tp->check_task_completed(t1);
	tp->check_task_completed(t2);
	tp->check_task_completed(t3);
	tp->end_task(t5);
	tp->check_task_completed(t4);
	tp->check_task_completed(t5);

	int* t1_input_ = static_cast <int*> (t1_input);
	int* t2_input_ = static_cast <int*> (t2_input);
	int* t3_input_ = static_cast <int*> (t3_input);
	int* t4_input_ = static_cast <int*> (t4_input);

	int* t1_output_ = static_cast <int*> (t1_output);
	int* t2_output_ = static_cast <int*> (t2_output);
	int* t3_output_ = static_cast <int*> (t3_output);
	int* t4_output_ = static_cast <int*> (t4_output);

	std::cout << " Task = " << t1.get_tp_task_id() << "  task input =  "
		<< *t1_input_ << "  task output = " << *t1_output_ << std::endl;
	std::cout << " Task = " << t2.get_tp_task_id() << "  task input =  "
		<< *t2_input_ << "  task output = " << *t2_output_ << std::endl;
	std::cout << " Task = " << t3.get_tp_task_id() << "  task input =  "
		<< *t3_input_ << "  task output =  " << *t3_output_ << std::endl;
	std::cout << " Task = " << t4.get_tp_task_id() << "  task input =  "
		<< *t4_input_ << "  task output =  " << *t4_output_ << std::endl;
	std::cout << " Task = " << t5.get_tp_task_id() << "  task input =  "
		<< t5_input << "  task output =  " << t5_output << std::endl;

	std::cout << " Task = " << t1.get_tp_task_id() << " " << tp_task_status_str_arr[t1.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t2.get_tp_task_id() << " " << tp_task_status_str_arr[t2.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t3.get_tp_task_id() << " " << tp_task_status_str_arr[t3.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t4.get_tp_task_id() << " " << tp_task_status_str_arr[t4.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t5.get_tp_task_id() << " " << tp_task_status_str_arr[t5.get_tp_task_status()] << std::endl;

	cout << tp->get_task_runtime_status(t1.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t2.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t3.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t4.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t5.get_tp_task_id()) << endl;

	// free Heap memory 
	delete tp;

	delete t1_input;
	delete t2_input;
	delete t3_input;
	delete t4_input;
	delete t5_input;

	delete t1_output;
	delete t2_output;
	delete t3_output;
	delete t4_output;
	delete t5_output;

	return 0;
}
