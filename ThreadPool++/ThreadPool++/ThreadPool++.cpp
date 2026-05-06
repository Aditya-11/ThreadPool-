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

	auto complete_task_function = [](TP::tp_task_input_ptr input, TP::tp_task_output_ptr output) {
		if (input == nullptr || output == nullptr) return;
		
		int * input_val_ptr = static_cast <int*> (input);
		int * output_val_ptr = static_cast <int*> (output);

		std::cout << " Task Completed " << " input to task = " << *input_val_ptr 
		                                << ", output of task = " << *output_val_ptr << "\n";

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

	t1.set_tp_task_complete_cb(complete_task_function);
	t2.set_tp_task_complete_cb(complete_task_function);
	t3.set_tp_task_complete_cb(complete_task_function);
	t4.set_tp_task_complete_cb(complete_task_function);
	t5.set_tp_task_complete_cb(complete_task_function);

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
	std::this_thread::sleep_for(tp_time_milliseconds(35));
	tp->enqueue_task(t2);
	cout << tp->get_task_runtime_status(t2.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(35));
	tp->enqueue_task(t3);
	cout << tp->get_task_runtime_status(t3.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(35));
	tp->enqueue_task(t4);
	cout << tp->get_task_runtime_status(t4.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(35));
	tp->enqueue_task(t5);
	cout << tp->get_task_runtime_status(t5.get_tp_task_id()) << endl;
	std::this_thread::sleep_for(tp_time_milliseconds(35));

	// CPU thread status 
	std::cout << tp->check_thread_status_native(t1) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(25));
	std::cout << tp->check_thread_status_native(t2) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(35));
	std::cout << tp->check_thread_status_native(t3) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(25));
	std::cout << tp->check_thread_status_native(t4) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(35));
	std::cout << tp->check_thread_status_native(t5) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(25));

	std::string str_1;
	std::string str_2;
	std::string str_3;
	
	tp->set_task_priority(t4, TP_TASK_PRIORITY_LOWEST);
	tp->set_task_priority(t3, TP_TASK_PRIORITY_LOWEST);
	tp->set_task_priority(t5, TP_TASK_PRIORITY_LOWEST);

	tp->check_task_completed_native(t1, 100, str_1);
	std::cout << str_1 << std::endl;
	tp->check_task_completed_native(t2, 200, str_2);
	std::cout << str_2 << std::endl;
	tp->check_task_completed_native(t3, 300, str_3);
	std::cout << str_3 << std::endl;

	tp->end_task(t5);

	std::string str_t5;
	tp->check_task_completed_native(t5, 400, str_t5);

	std::cout << str_t5 << std::endl;

	std::string str_t4;
	tp->check_task_completed_native(t4, 500, str_t4);
	
	std::cout << str_t4 << std::endl;

	int* t1_input_ = static_cast <int*> (t1_input);
	int* t2_input_ = static_cast <int*> (t2_input);
	int* t3_input_ = static_cast <int*> (t3_input);
	int* t4_input_ = static_cast <int*> (t4_input);
	int* t5_input_ = static_cast <int*> (t5_input);

	int* t1_output_ = static_cast <int*> (t1_output);
	int* t2_output_ = static_cast <int*> (t2_output);
	int* t3_output_ = static_cast <int*> (t3_output);
	int* t4_output_ = static_cast <int*> (t4_output);
	int* t5_output_ = static_cast <int*> (t5_output);

	std::cout << " Task = " << t1.get_tp_task_id() << "  task input =  "
		<< *t1_input_ << "  task output = " << *t1_output_ << std::endl;
	std::cout << " Task = " << t2.get_tp_task_id() << "  task input =  "
		<< *t2_input_ << "  task output = " << *t2_output_ << std::endl;
	std::cout << " Task = " << t3.get_tp_task_id() << "  task input =  "
		<< *t3_input_ << "  task output =  " << *t3_output_ << std::endl;
	std::cout << " Task = " << t4.get_tp_task_id() << "  task input =  "
		<< *t4_input_ << "  task output =  " << *t4_output_ << std::endl;
	std::cout << " Task = " << t5.get_tp_task_id() << "  task input =  "
		<< *t5_input_ << "  task output =  " << *t5_output_ << std::endl;

	std::cout << " Task = " << t1.get_tp_task_id() << " " << tp_task_status_str_arr[t1.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t2.get_tp_task_id() << " " << tp_task_status_str_arr[t2.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t3.get_tp_task_id() << " " << tp_task_status_str_arr[t3.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t4.get_tp_task_id() << " " << tp_task_status_str_arr[t4.get_tp_task_status()] << std::endl;
	std::cout << " Task = " << t5.get_tp_task_id() << " " << tp_task_status_str_arr[t5.get_tp_task_status()] << std::endl;

	tp_task_runtime_data t4_runtime_data;

	cout << tp->get_task_runtime_status(t1.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t2.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t3.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t4.get_tp_task_id(), &t4_runtime_data) << endl;
	cout << tp->get_task_runtime_status(t5.get_tp_task_id()) << endl;
	
	cout << t4_runtime_data.task_id << t4_runtime_data.thread_run_status << " " 
	<< t4_runtime_data.task_start.count() << " " << t4_runtime_data.task_end.count() 
	<< " " << t4_runtime_data.thread_id << std::endl;

	// CPU thread status 
	std::cout << tp->check_thread_status_native(t1) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(350));
	std::cout << tp->check_thread_status_native(t2) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(250));
	std::cout << tp->check_thread_status_native(t3) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(350));
	std::cout << tp->check_thread_status_native(t4) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(250));
	std::cout << tp->check_thread_status_native(t5) << std::endl;
	std::this_thread::sleep_for(tp_time_milliseconds(350));

	TP::TP_Task* task = new TP::TP_Task();
	TP::tp_task_input_ptr task_input = new int(0);
	TP::tp_task_output_ptr task_output = new int(0);

	// sample task function
	auto f_task = [](TP::tp_task_input_ptr input, TP::tp_task_output_ptr output) {
		int* input_val = static_cast <int*> (input);
		int* output_val = static_cast <int*> (output);
		*output_val = ++(*input_val);
	};

	task->set_tp_task_input_ptr(task_input);
	task->set_tp_task_output_ptr(task_output);
	task->set_tp_task_cb(f_task);

	tp->enqueue_task(*task);
	tp_task_id task_id = task->get_tp_task_id();

	std::this_thread::sleep_for(tp_time_milliseconds(35));

	// get runtime status of the task in JSON.
	std::cout << tp->get_task_runtime_status(task_id) << std::endl;

	std::this_thread::sleep_for(tp_time_milliseconds(25));

	// check and wait for the task, get task completed output in JSON.
	std::string task_completed_output;
	uint32_t task_timeout = 5000; // value in milliseconds 
	tp->check_task_completed_native(*task, task_timeout, task_completed_output);

	std::cout << task_completed_output << std::endl;

	int* task_output_ptr = static_cast <int*> (task_output);

	std::cout << " task completed output = " << *task_output_ptr << std::endl;

	// free Heap memory 
	delete tp;

	delete task;
    delete task_input;
	delete task_output;

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