// ThreadPool++.h : Include file for standard system include files,
// or project specific include files.

// Thread Pool ++ library 
// Developed by Aditya Dubey

#pragma once

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <vector>
#include <functional>
#include <condition_variable>
#include <unordered_map>
#include <cassert>
#include <exception>
#include <string>

// Use Native CPU thread Implementation based on Operating System
#ifdef WIN32
	// Windows OS
	#include <Windows.h>
#else
	// Linux or MACOS 
	#include <pthread.h>
#endif

// TP TASK error code
#define TP_NO_THREAD_FOR_TP_TASK -1
#define TP_TASK_INVALID_STATUS -2
#define TP_TASK_CANCELLED -3
#define TP_NO_TASK_ENQUEUED_TO_PROCESS_QUEUE -4

typedef uint32_t tp_task_id;

// task time in mili seconds
typedef uint64_t tp_task_time;

// task status 
typedef enum {
	TP_TASK_MIN_VALUE = 0x00,
	TP_TASK_ENQUEUED_PROCESS_QUEUE = 0x01,
	TP_TASK_RUN = 0x02,
	TP_TASK_ENQUEUED_COMPLETION_QUEUE = 0x03,
	TP_TASK_COMPLETED = 0x04,
	TP_TASK_ENDED = 0x05,
	TP_TASK_MAX_VALUE = 0x06
} tp_task_status;

// task_call_back
typedef std::function<void()> tp_task_cb;

// task_output
typedef void* tp_task_output_ptr;

// task_input
typedef void* tp_task_input_ptr;

// Thread Pool Task Implementaion 
class TP_Task {

public:

	// thread_id associated with the TP_Task 
	uint32_t thread_id;
	
	TP_Task(tp_task_id id, tp_task_status status, tp_task_cb run_cb, tp_task_cb complete_cb)
	{
		this->id = id;
		this->status = status;
		this->run_cb = run_cb;
		this->complete_cb = complete_cb;
		this->thread_id = -1;
	}

	TP_Task() {
		this->id = -1;
		this->status = TP_TASK_MIN_VALUE;
		this->run_cb = nullptr;
		this->complete_cb = nullptr;
		this->thread_id = -1;
	}

	TP_Task(tp_task_cb run_cb, tp_task_cb complete_cb) {
		this->id = -1;
		this->status = TP_TASK_MIN_VALUE;
		this->run_cb = run_cb;
		this->complete_cb = complete_cb;
		this->thread_id = -1;
	}

	tp_task_id get_tp_task_id();
	void set_tp_task_id(tp_task_id task_id);

	tp_task_status get_tp_task_status();
	void set_tp_task_status(tp_task_status task_status);

	tp_task_cb get_tp_task_cb();
	void set_tp_task_cb(tp_task_cb task_cb);

	void set_tp_task_input_ptr(tp_task_input_ptr output_ptr);
	tp_task_input_ptr get_ip_task_input_ptr();

	void set_tp_task_output_ptr(tp_task_output_ptr output_ptr);
	tp_task_output_ptr get_ip_task_output_ptr();

	~TP_Task() {

	}
 
private :

	tp_task_id id;

	tp_task_status status;

	tp_task_cb run_cb;

	tp_task_cb complete_cb;

	tp_task_time task_start_time;

	tp_task_time task_end_time;

	tp_task_output_ptr output_ptr;

	tp_task_input_ptr input_ptr;

};

tp_task_id TP_Task::get_tp_task_id() {
	return this->id;
}

void TP_Task::set_tp_task_id(tp_task_id task_id) {
	this->id = task_id;
}

tp_task_status TP_Task::get_tp_task_status() {
	return this->status;
}

void TP_Task::set_tp_task_status(tp_task_status task_status) {
	this->status = task_status;
}

tp_task_cb TP_Task::get_tp_task_cb() {
	return this->run_cb;
}

void TP_Task::set_tp_task_cb(tp_task_cb task_cb) {
	this->run_cb = task_cb;
}

tp_task_input_ptr TP_Task::get_ip_task_input_ptr() {
	return this->input_ptr;
}

void TP_Task::set_tp_task_input_ptr (tp_task_input_ptr output_ptr) {
	this->input_ptr = input_ptr;
}

void TP_Task::set_tp_task_output_ptr(tp_task_output_ptr output_ptr) {
	this->output_ptr = output_ptr;
}

tp_task_output_ptr TP_Task::get_ip_task_output_ptr() {
	return this->output_ptr;
}

//tp_task_output_ptr TP_Task::g


// Thread Safe Implementation 

// Thread Pool Process Queue 
std::queue <TP_Task> process_queue;

// boolean to track added to queue
std::atomic<bool> added_to_process_queue = false;

// boolean to track get from queue 
std::atomic<bool> get_from_process_queue = false;

// Mutex 
std::mutex process_queue_mutex;

// conditional_variable process_queue 

std::condition_variable process_q_conditonal_var ;

// Thread Pool Completion Queue  
std::queue <TP_Task> completion_queue;

// boolean to track added to queue
std::atomic <bool> added_to_completion_queue = false;

// boolean to track get from queue
std::atomic <bool> get_from_completion_queue = false;

// Mutex
std::mutex completion_queue_mutex;

// conditional_variable completion queue

std::condition_variable completion_q_conditional_var;

// Abstract Thread_Pool Class

class Thread_Pool_ {

public :

	Thread_Pool_() {}

	virtual tp_task_id enqueue_task(TP_Task &task_) = 0;
	virtual tp_task_status get_task_status(TP_Task task_) = 0;
	virtual bool end_task(TP_Task &task_) = 0;
	virtual uint32_t process_task() = 0;
	std::string get_implementation_() { return this->implementation_; }

	~Thread_Pool_() {}

protected:
	std::string implementation_;

};

// thread_vector -> vector of threads 

class TP_Implementation_ : public Thread_Pool_{

	public :
		uint32_t enqueue_task(TP_Task &task_);
		tp_task_status get_task_status(TP_Task task_);
		bool end_task(TP_Task &task_);
		uint32_t process_task() ;
		std::string get_task_runtime_status(tp_task_id task_id);

		TP_Implementation_() {
			this->cpu_max_hyper_threads = std::thread::hardware_concurrency();
			assert(this->cpu_max_hyper_threads > 0, "Failed Thread Pool Initialization, threads == 0");
			this->thread_vec.resize(this->cpu_max_hyper_threads);
			tp_init_thread_vector();
			this->task_vec.resize(this->cpu_max_hyper_threads);

			assert(this->thread_vec.size() == this->cpu_max_hyper_threads, "Thread Vec not initialized ");
			this->task_m = new std::unordered_map <tp_task_id,int>();
			this->process_queue_ptr = &process_queue;
			this->completion_queue_ptr = &completion_queue;
			this->implementation_ = "CPU THREAD POOL++ Implementation";
			this->add_to_process_q = &added_to_process_queue;
			this->get_from_process_q = &get_from_process_queue;
			this->current_task = 0;
			this->check_process_q_ = true;
			this->t_check_process_q_ = std::thread([this]() {
					while (this->check_process_q_) {
						this->process_task();
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					}
				});
		}

		~TP_Implementation_() {
			if (task_m != nullptr) delete this->task_m;
			this->process_queue_ptr = nullptr;
			this->completion_queue_ptr = nullptr;
			this->check_process_q_ = false;
			if (this->t_check_process_q_.joinable()) this->t_check_process_q_.join();
		}

	private :
		
		std::queue <TP_Task> * process_queue_ptr; 
		
		std::mutex process_q_mutex; 
		
		std::condition_variable process_q_conditional_var; 

		// HasH Table : tp_task_id -> vector thread index  
		std::unordered_map <tp_task_id, int> * task_m; 

		// Hash Table : thread index -> tp_task  
		std::vector <TP_Task *> task_vec;

		std::vector<std::thread> thread_vec;  

		std::queue <TP_Task>* completion_queue_ptr; 
		
		void tp_init_thread_vector();

		//void tp_init_task_vector();
		
		uint32_t cpu_max_hyper_threads;
		
		std::atomic <bool> * add_to_process_q;
		
		std::atomic <bool> * get_from_process_q;
		
		tp_task_id current_task;

		uint32_t tp_find_free_thread_for_task();

		std::atomic <bool> check_process_q_ = false;

		std::thread t_check_process_q_;
};

uint32_t TP_Implementation_::enqueue_task(TP_Task &task_) {

	std::unique_lock <std::mutex> lk(process_queue_mutex);

	task_.set_tp_task_id(this->current_task);
	this->current_task = this->current_task + 1;
	task_.set_tp_task_status(TP_TASK_ENQUEUED_PROCESS_QUEUE);

	this->process_q_conditional_var.wait(lk, [this, &task_]() {
		this->process_queue_ptr->push(task_);
		*(this->add_to_process_q) = true;
		return *(this->add_to_process_q) & !(*(this->get_from_process_q));
	});

	*(this->add_to_process_q) = false;
	
	lk.unlock();

	this->process_q_conditional_var.notify_all();
	
	this->process_task();

	return 0;
}

uint32_t TP_Implementation_::process_task() {

	uint32_t x = this->tp_find_free_thread_for_task();

	if (this->process_queue_ptr->size() == 0) return TP_NO_TASK_ENQUEUED_TO_PROCESS_QUEUE;

	if (x != TP_NO_THREAD_FOR_TP_TASK) {
#ifdef WIN32

		// windows implementation via WIN32 API

		try {

			std::unique_lock <std::mutex> lk(process_queue_mutex);

			TP_Task task_; 

			this->process_q_conditional_var.wait(lk, [this, &task_]() {
				  if (this->process_queue_ptr->size() > 0) {
					  task_ = this->process_queue_ptr->front();
					  this->process_queue_ptr->pop();
					  *(this->get_from_process_q) = true;
				  }
				  return *(this->get_from_process_q) & !(*(this->add_to_process_q)) ;
				});

			*(this->get_from_process_q) = false;

			lk.unlock();

			this->process_q_conditional_var.notify_all();

			if (task_.get_tp_task_status() == TP_TASK_ENQUEUED_PROCESS_QUEUE) {
				assert(this->thread_vec.size() > 0, "thread vector not initialized");
				assert(this->thread_vec.size() == this->cpu_max_hyper_threads, "Invalid Number of threads");

				HANDLE win_handle = this->thread_vec[x].native_handle();

				if (win_handle != NULL && win_handle != INVALID_HANDLE_VALUE) {
					CloseHandle(win_handle);
				}

				tp_task_cb task_cb = task_.get_tp_task_cb();

				if (this->task_vec[x] != nullptr) this->task_vec[x]->set_tp_task_status(TP_TASK_COMPLETED);

				this->thread_vec[x] = std::thread(std::bind(task_cb));

				this->task_m->insert({ task_.get_tp_task_id(), x });

				task_.set_tp_task_status(TP_TASK_RUN);

				this->task_vec[x] = &task_;

				return x;
			}
			else {
				return TP_TASK_INVALID_STATUS;
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;

			return TP_NO_THREAD_FOR_TP_TASK;
		}
#else
		// linux / Macos implementation 

#endif
	}

	return TP_NO_THREAD_FOR_TP_TASK;

}

tp_task_status TP_Implementation_::get_task_status(TP_Task task_) {
	return task_.get_tp_task_status();
}

bool TP_Implementation_::end_task(TP_Task &task_) {
	
   #ifdef WIN32

	try {

		// if task is at the process_queue

		if (task_.get_tp_task_status() == TP_TASK_ENQUEUED_PROCESS_QUEUE) {
			task_.set_tp_task_status(TP_TASK_ENDED);
			return true;
		}

		// if task has started running

		else if (task_.get_tp_task_status() == TP_TASK_RUN) {

			int32_t thread_index = -1;

			if (this->task_m->find(task_.get_tp_task_id()) != this->task_m->end()) {
				thread_index = (*this->task_m)[task_.get_tp_task_id()];
				if (thread_index == TP_TASK_CANCELLED) return false;
			}
			else {
				return false;
			}

			HANDLE win_handle = this->thread_vec.at(thread_index).native_handle();

			if (win_handle != NULL && win_handle != INVALID_HANDLE_VALUE) {
				CloseHandle(win_handle);
			}

			task_.set_tp_task_status(TP_TASK_ENDED);

			this->task_m->insert({thread_index , TP_TASK_CANCELLED});

			return true;

		}

		else {

			std::cout << "task_status = "<< task_.get_tp_task_id() << std::endl;
			return false;
		
		}
	}
	catch (std::exception& e) {

		std::cout << e.what() << std::endl;

		return false;
	}

   #else 
	// linux / MACOS implementation 
		
   #endif

}

void TP_Implementation_::tp_init_thread_vector() {
	
	for (int x = 0; x < this->cpu_max_hyper_threads ; x++) {
		this->thread_vec[x] = std::thread(); 
	}

}

uint32_t TP_Implementation_::tp_find_free_thread_for_task() {

	for (int x = 0; x < this->cpu_max_hyper_threads; x++) {
		// implementation for Windows 
		#ifdef WIN32
		    // windows native thread implementation
		HANDLE win_handle = this->thread_vec[x].native_handle();

		DWORD exitCode;

		bool thread_is_busy = (GetExitCodeThread(win_handle, &exitCode) == STILL_ACTIVE) ? true : false;

		if (thread_is_busy) continue;
		else {
			if (this->task_vec[x] != nullptr) {
				this->end_task(*task_vec[x]);
			}
			return x;
		}
		#else
			// pthread implementation for linux/macos OS	
		#endif

	}

	return TP_NO_THREAD_FOR_TP_TASK;
}

std::string TP_Implementation_::get_task_runtime_status(tp_task_id task_id) {

	std::string str_ = "";

#ifdef WIN32

	// Use WIN32 Thread Information API also  

	int x = TP_NO_THREAD_FOR_TP_TASK;
	
	if (this->task_m->find(task_id) != this->task_m->end()) {
		x = this->task_m->at(task_id);
	}
	else {
		str_.append("error runtime status");
		return str_;
	}

	HANDLE win_handle = this->thread_vec[x].native_handle();

	DWORD exitCode;

	bool thread_is_busy = (GetExitCodeThread(win_handle, &exitCode) == STILL_ACTIVE) ? true : false;

	DWORD thread_id = GetThreadId(win_handle);

	std::string str_task_id = std::to_string(task_id);
	std::string str_thread = std::to_string(thread_id);
	std::string str_thread_is_busy = std::to_string(thread_is_busy);

	str_.append("{")
		.append("task_id : ").append(str_task_id).append(", ")
		.append("thread : ").append(str_thread).append(", ")
		.append("thread_run_status : ").append(str_thread_is_busy).append(" ")
		.append("}");

#else

#endif

	return str_;
}

typedef TP_Implementation_ TP_CPU_CLASS;


// Thread Pool 



// Thread Pool Implementation 

/*
class TP_Implementation {



}; */

/* 	
	Queue <Task> ;

	TP_Task {

		task_id;
		task_status;
		task_cb;
		task_completion_cb;
		task_start_time;
		task_end_time;

	}


	Task -> process queue -> ThreadPool -> completion queue -> run the Callback after task is completed / log the task


	ThreadPool () {
		
	}

	~ThreadPool () {
		
	}

	task_ID enqueue_task(std::function <T> f_) {
		
	}

	task_status get_task_status() {
		
	}	

	boolean end_task() {

	}

*/


// TODO: Reference additional headers your program requires here.





