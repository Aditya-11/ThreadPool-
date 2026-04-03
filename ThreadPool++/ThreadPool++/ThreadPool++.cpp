// ThreadPool++.cpp : Defines the entry point for the application.
//

#include "ThreadPool++.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;

	TP_CPU_CLASS * tp = new TP_CPU_CLASS();

	cout << tp->get_implementation_() << endl;

	// task function run
		
	auto f_ = [](tp_task_input_ptr input, tp_task_output_ptr output) { 
		    
		    std::cout << " task thread run " << input << " " << std::endl;
			
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

	TP_Task t1;
	TP_Task t2;
	TP_Task t3;
	TP_Task t4;
	TP_Task t5;

	t1.set_tp_task_cb(f_);
	t2.set_tp_task_cb(f_);
	t3.set_tp_task_cb(f_);
	t4.set_tp_task_cb(f_);

	tp_task_input_ptr t1_input = new int(1000); 
	tp_task_input_ptr t2_input = new int(2000);
	tp_task_input_ptr t3_input = new int(3000);
	tp_task_input_ptr t4_input = new int(4000);

	tp_task_output_ptr t1_output = new int(0);
	tp_task_output_ptr t2_output = new int(0);
	tp_task_output_ptr t3_output = new int(0);
	tp_task_output_ptr t4_output = new int(0);

	int t5_input = 5000;
	int t5_output = 5000;

	t1.set_tp_task_input_ptr(t1_input);
	t2.set_tp_task_input_ptr(t2_input);
	t3.set_tp_task_input_ptr(t3_input);
	t4.set_tp_task_input_ptr(t4_input);
	t5.set_tp_task_input_ptr(&t5_input);

	t1.set_tp_task_output_ptr(t1_output);
	t2.set_tp_task_output_ptr(t2_output);
	t3.set_tp_task_output_ptr(t3_output);
	t4.set_tp_task_output_ptr(t4_output);
	t5.set_tp_task_input_ptr(&t5_output);

	tp->enqueue_task(t1);
	cout << tp->get_task_runtime_status(t1.get_tp_task_id()) << endl;
	tp->enqueue_task(t2);
	cout << tp->get_task_runtime_status(t2.get_tp_task_id()) << endl;
	tp->enqueue_task(t3);
	cout << tp->get_task_runtime_status(t3.get_tp_task_id()) << endl;
	tp->enqueue_task(t4);
	cout << tp->get_task_runtime_status(t4.get_tp_task_id()) << endl;

	std::this_thread::sleep_for(std::chrono::seconds(7));

	tp->check_task_completed(t1);
	tp->check_task_completed(t2);
	tp->check_task_completed(t3);
	tp->check_task_completed(t4);
	tp->check_task_completed(t5);

	std::cout << " Task = " << t1.get_tp_task_id() << " " << t1.get_tp_task_status() << std::endl;
	std::cout << " Task = " << t2.get_tp_task_id() << " " << t2.get_tp_task_status() << std::endl;
	std::cout << " Task = " << t3.get_tp_task_id() << " " << t3.get_tp_task_status() << std::endl;
	std::cout << " Task = " << t4.get_tp_task_id() << " " << t4.get_tp_task_status() << std::endl;

	std::cout << " Task = " << t1.get_tp_task_id() << "  task input =  " 
		                                           << t1_input << "  task output = " << t1_output << std::endl;
	std::cout << " Task = " << t1.get_tp_task_id() << "  task input =  "
													<< t2_input << "  task output = " << t2_output << std::endl;
	std::cout << " Task = " << t1.get_tp_task_id() << "  task input =  "
													<< t3_input << "  task output =  " << t3_output << std::endl;
	std::cout << " Task = " << t1.get_tp_task_id() << "  task input =  "
													<< t4_input << "  task output =  " << t4_output << std::endl;
	std::cout << " Task = " << t5.get_tp_task_id() << "  task input =  "
													<< t5_input << "  task output =  " << t5_output << std::endl;
	
	cout << tp->get_task_runtime_status(t3.get_tp_task_id()) << endl;
	cout << tp->get_task_runtime_status(t2.get_tp_task_id()) << endl;

	// free Heap memory 
	delete tp;

	delete t1_input;
	delete t2_input;
	delete t3_input;
	delete t4_input;

	delete t1_output;
	delete t2_output;
	delete t3_output;
	delete t4_output; 

	return 0;
}
