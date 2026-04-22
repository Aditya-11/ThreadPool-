// ThreadPool++.h : Include file for standard system include files,
// or project specific include files.

// Thread Pool ++ library
// Developed by Aditya Dubey

/*
Thread Pool ++

	Create Parallel Tasks (TP_Task Objects) which could run parallely on hyperthreads of multicore CPU processor
	enqueue_task: add tasks to ThreadPool
	get_task_runtime_status : check runtime performance and status of the task
 	end_task : end task if task not required
	check_task_completed : checks and waits until the task is completed
	check_task_completed_native : checks and waits until the task completed, returns information related to thread.
	ThreadPool manages tasks and threads resources, ThreadPool maps tasks to threads

	Library :
    C++ STANDARD TEMPLATE LIBRARY (STL) , C++11 standard
	WIN32 Library for windows native implementation
	PTHREAD Library for Macos and linux native implementation

	Code and support of other features for Thread Pool ++ Library :
	Add code and support for GPU hardware, via native API or cross platform framework
	Add code and support for Macos and linux native implementation via Pthread Library
	Add code and support for other Hardware Accelerators

*/

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
#include <ctime>
#include <chrono>
#include <atomic>
#include <cstring>

// Use Native CPU thread Implementation based on Operating System
#ifdef WIN32
	// Windows OS
	#include <Windows.h>
#else
	// Linux or MACOS
	#include <pthread.h>

	#ifdef __linux__
		#include <unistd.h>
	#endif

#endif

namespace TP {

// TP TASK error code
#define TP_NO_THREAD_FOR_TP_TASK -1
#define TP_TASK_INVALID_STATUS -2
#define TP_TASK_CANCELLED -3
#define TP_NO_TASK_ENQUEUED_TO_PROCESS_QUEUE -4

// string utility macro
#define UTIL_STRING_NEWLINE [](char * str_, size_t size_) { \
	       for (int x = 0 ; x < size_ ; x++) { \
		         if (str_[x] == '\n' || str_[x] == '\t') { str_[x] = ' '; } else { continue;} }}

// windows platform Utility functions
#ifdef WIN32

#define TIME_ZONE_TYPE_ARRAY_SIZE 32

// For No Time Zone
#define NO_TZ_STRING true

auto util_time_to_string_windows = [](SYSTEMTIME t_, WCHAR* time_zone_value) {
	std::string str_;
	char str_time_zone_[TIME_ZONE_TYPE_ARRAY_SIZE] ;
	WideCharToMultiByte(CP_UTF8, 0, time_zone_value, -1, &str_time_zone_[0], TIME_ZONE_TYPE_ARRAY_SIZE, NULL, NULL);
	str_.append(std::to_string(t_.wMonth)).append("-").append(std::to_string(t_.wDay)).append("-")
		.append(std::to_string(t_.wYear)).append(" ").append(std::to_string(t_.wHour)).append(":")
		.append(std::to_string(t_.wMinute)).append(":").append(std::to_string(t_.wSecond)).append(".")
		.append(std::to_string(t_.wMilliseconds)).append(" ")
#ifndef NO_TZ_STRING
		.append(str_time_zone_);
#else
		;
#endif
	return str_;
};

#define UTIL_THREAD_PRIORITY_WINDOWS [](int thread_priority) { \
	switch(thread_priority) { \
		case THREAD_PRIORITY_ABOVE_NORMAL: return "Thread Priority Above normal"; \
		case THREAD_PRIORITY_BELOW_NORMAL: return "Thread Priority Below normal"; \
		case THREAD_PRIORITY_HIGHEST: return "Thread Priority Highest"; \
		case THREAD_PRIORITY_IDLE : return "Thread Priority Idle"; \
		case THREAD_PRIORITY_LOWEST : return "Thread Priority Lowest"; \
		case THREAD_PRIORITY_NORMAL : return "Thread Priority Normal"; \
		case THREAD_PRIORITY_TIME_CRITICAL : return "Thread Priority time critical";\
		default : return "Thread Priority None";\
	}\
}

#endif

// linux platform utility functions
#ifdef __linux__ 
#ifdef __cplusplus

#define PTHREAD_JOINED 0

#define util_thread_join_output_to_str_linux [](int output) { \
	switch(output) { \
		case EINVAL: return "pthread_join, invalid argument"; \
		case ESRCH: return "pthread_join, No such process"; \
		case ETIMEDOUT: return "pthread_join, time out thread couldnt be joined"; \
		case EBUSY: return "pthread_join, thread is busy"; \
		case PTHREAD_JOINED : return "pthread_join, thread joined success"; \
		default : return "default" ;\
	} } \ 

#define util_check_thread_is_running [](int thread_status) { \
	if (thread_status == ETIMEDOUT || thread_status == EBUSY) {\
		return true;\
	} \ 
	return false; \
}

#endif
#endif

typedef uint32_t tp_task_id;

// task time
typedef std::chrono::time_point<std::chrono::steady_clock> tp_task_time;

// task duration in milliseconds
typedef std::chrono::milliseconds tp_time_milliseconds;

// task duration in seconds
typedef std::chrono::seconds tp_time_seconds;

// task duration in nano seconds
typedef std::chrono::nanoseconds tp_time_nanoseconds;

// task duration in nano seconds
#define TP_END_TASK_WAIT_TIME 3334

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

// task status string
static const char* tp_task_status_str_arr[] = {
	"TP_TASK_MIN_VALUE",
	"TP_TASK_ENQUEUED_PROCESS_QUEUE",
	"TP_TASK_RUN",
	"TP_TASK_ENQUEUED_COMPLETION_QUEUE",
	"TP_TASK_COMPLETED",
	"TP_TASK_ENDED",
	"TP_TASK_MAX_VALUE"
};

// task_call_back
typedef std::function<void(void *,void *)> tp_task_cb;

// task_output
typedef void* tp_task_output_ptr;

// task_input
typedef void* tp_task_input_ptr;

// task runtime data
typedef struct tp_task_runtime_data{

	time_t               curr_time;
	tp_task_id           task_id;
	tp_task_status       task_status;
	tp_time_milliseconds task_start;
	tp_time_milliseconds task_end;
	tp_time_milliseconds task_duration;
	uint32_t             thread_id;
	uint8_t              thread_run_status;

};

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
		this->task_start_time = std::chrono::steady_clock::time_point(tp_time_milliseconds(0));
		this->task_end_time = std::chrono::steady_clock::time_point(tp_time_milliseconds(0));
		this->task_duration = tp_time_milliseconds(0);
	}

	TP_Task() {
		this->id = -1;
		this->status = TP_TASK_MIN_VALUE;
		this->run_cb = nullptr;
		this->complete_cb = nullptr;
		this->thread_id = -1;
		this->task_start_time = std::chrono::steady_clock::time_point(tp_time_milliseconds(0));
		this->task_end_time = std::chrono::steady_clock::time_point(tp_time_milliseconds(0));
		this->task_duration = tp_time_milliseconds(0);
	}

	TP_Task(tp_task_cb run_cb, tp_task_cb complete_cb) {
		this->id = -1;
		this->status = TP_TASK_MIN_VALUE;
		this->run_cb = run_cb;
		this->complete_cb = complete_cb;
		this->thread_id = -1;
		this->task_start_time = std::chrono::steady_clock::time_point(tp_time_milliseconds(0));
		this->task_end_time = std::chrono::steady_clock::time_point(tp_time_milliseconds(0));
		this->task_duration = tp_time_milliseconds(0);
	}

	tp_task_id get_tp_task_id();
	void set_tp_task_id(tp_task_id task_id);

	tp_task_status get_tp_task_status();
	void set_tp_task_status(tp_task_status task_status);

	tp_task_cb get_tp_task_cb();
	void set_tp_task_cb(tp_task_cb task_cb);

	void set_tp_task_input_ptr(tp_task_input_ptr output_ptr);
	tp_task_input_ptr get_tp_task_input_ptr();

	void set_tp_task_output_ptr(tp_task_output_ptr output_ptr);
	tp_task_output_ptr get_tp_task_output_ptr();

	tp_task_time get_tp_task_start_time();
	void set_tp_task_start_time(tp_task_time t);

	tp_task_time get_tp_task_end_time();
	void set_tp_task_end_time(tp_task_time t);

	tp_time_milliseconds get_tp_task_duration();
	void set_tp_task_duration(tp_time_milliseconds t);

	~TP_Task() {

	}

private :

	tp_task_id id;

	tp_task_status status;

	tp_task_cb run_cb;

	tp_task_cb complete_cb;

	tp_task_time task_start_time;

	tp_task_time task_end_time;

	tp_time_milliseconds task_duration;

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

tp_task_input_ptr TP_Task::get_tp_task_input_ptr() {
	return this->input_ptr;
}

void TP_Task::set_tp_task_input_ptr(tp_task_input_ptr input_ptr) {
	this->input_ptr = input_ptr;
}

void TP_Task::set_tp_task_output_ptr(tp_task_output_ptr output_ptr) {
	this->output_ptr = output_ptr;
}

tp_task_output_ptr TP_Task::get_tp_task_output_ptr() {
	return this->output_ptr;
}

tp_task_time TP_Task::get_tp_task_start_time() {
	return this->task_start_time;
}

void TP_Task::set_tp_task_start_time(tp_task_time t) {
	this->task_start_time = t;
}

tp_task_time TP_Task::get_tp_task_end_time() {
	return this->task_end_time;
}

void TP_Task::set_tp_task_end_time(tp_task_time t) {
	this->task_end_time = t;
}

void TP_Task::set_tp_task_duration(tp_time_milliseconds t) {
	this->task_duration = t;
}

tp_time_milliseconds TP_Task::get_tp_task_duration() {
	return this->task_duration;
}

// Thread Safe Implementation

// Thread Pool Process Queue
std::queue <TP_Task*> process_queue;

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
	virtual uint32_t process_task() = 0;
	std::string get_implementation_() { return this->implementation_; }
	virtual bool check_task_completed(TP_Task task_) = 0;

	~Thread_Pool_() {}

protected:
	std::string implementation_;

};

class TP_Implementation_ : public Thread_Pool_ {

public:
		uint32_t enqueue_task(TP_Task &task_);
		tp_task_status get_task_status(TP_Task task_);
		bool check_task_completed_native(TP_Task &task_, std::string& output);
		bool check_task_completed_native(TP_Task& task_, uint64_t time_in_milliseconds, std::string &output);
		uint32_t process_task() ;
		std::string get_task_runtime_status(tp_task_id task_id, tp_task_runtime_data* tp_perf_stats);
		std::string get_task_runtime_status(tp_task_id task_id);
		bool check_task_completed(TP_Task  task_);
		void tp_task_function_(TP_Task task_);
		void set_time_check_process_q_(int32_t time_);
		int32_t get_time_check_process_q_();
		bool end_task(TP_Task &task_);
		std::string check_thread_status_native(TP_Task &task_);
#ifdef WIN32
		// WINDOWS Specific feature
		std::string check_thread_status_native(HANDLE win_handle, tp_task_id task_id, tp_task_status task_status);
#endif
#ifdef __linux__
		// Linux Specific feature
		// Linux pthread id to kernel id
		std::unordered_map <pthread_t, int> * tid_m;
		int check_tid_thread();
#endif
		std::unordered_map <tp_task_id, std::string> * thread_status;

		TP_Implementation_() {
			this->tp_instance_init_time = std::chrono::steady_clock::now();
			this->cpu_max_hyper_threads = std::thread::hardware_concurrency();
			#ifdef WIN32
			assert(this->cpu_max_hyper_threads > 0, "Failed Thread Pool Initialization, threads == 0");
			#else
			
			#ifdef __linux__
			assert(this->cpu_max_hyper_threads > 0);
			#endif
			
			#endif
			this->thread_vec.resize(this->cpu_max_hyper_threads);
			tp_init_thread_vector();

			#ifdef WIN32
			assert(this->thread_vec.size() == this->cpu_max_hyper_threads, "Thread Vec not initialized ");
			#else
			#ifdef __linux__
			assert(this->thread_vec.size() == this->cpu_max_hyper_threads);
			#endif
			#endif

			this->task_m = new std::unordered_map <tp_task_id, int>();
			this->process_queue_ptr = &process_queue;
			this->completion_queue_ptr = &completion_queue;
			this->implementation_ = "CPU THREAD POOL++ Implementation";
			this->add_to_process_q = &added_to_process_queue;
			this->get_from_process_q = &get_from_process_queue;
			this->current_task = 0;
			this->check_process_q_ = true;
			this->time_check_process_q_ = 800;
			this->thread_check_process_q_ = std::thread([this]() {
				while (this->check_process_q_ && !(this->process_queue_ptr->empty())) {
					this->process_task();
					std::this_thread::sleep_for(tp_time_milliseconds(this->time_check_process_q_));
				}
				});
			this->thread_status = new std::unordered_map <tp_task_id, std::string>();

			#ifdef __linux__
				this->tid_m = new std::unordered_map <pthread_t, int>();
				this->thread_status_vec.resize(this->cpu_max_hyper_threads);
				assert(this->thread_status_vec.size() == this->cpu_max_hyper_threads);
			#endif 
		}

		~TP_Implementation_() {
			this->check_process_q_ = false;
			if (this->thread_check_process_q_.joinable()) this->thread_check_process_q_.join();
			this->check_all_tasks_completed();
			if (task_m != nullptr) delete this->task_m;
			this->process_queue_ptr = nullptr;
			this->completion_queue_ptr = nullptr;
			this->check_process_q_ = false;
			delete this->thread_status;
			#ifdef __linux__
			delete this->tid_m;
			#endif
		}

private :

		tp_task_time tp_instance_init_time;

		std::queue <TP_Task *> * process_queue_ptr;

		std::mutex process_q_mutex;

		std::condition_variable process_q_conditional_var;

		// HasH Table : tp_task_id -> vector thread index
		std::unordered_map <tp_task_id, int> * task_m;

		// Hash Table : thread index -> tp_task
		std::vector <TP_Task> task_vec;

		std::vector<std::thread> thread_vec;

		std::queue <TP_Task>* completion_queue_ptr;

		void tp_init_thread_vector();

		uint32_t cpu_max_hyper_threads;

		std::atomic <bool> * add_to_process_q;

		std::atomic <bool> * get_from_process_q;

		tp_task_id current_task;

		uint32_t tp_find_free_thread_for_task();

		std::atomic <bool> check_process_q_ = false;

		std::thread thread_check_process_q_;

		bool check_all_tasks_completed();

		void check_start_time(TP_Task& t);

		void check_end_time(TP_Task& t);

		int32_t time_check_process_q_;

		#ifdef __linux__
			std::vector<bool> thread_status_vec; 
	    #endif
};

uint32_t TP_Implementation_::enqueue_task(TP_Task &task_) {

	std::unique_lock <std::mutex> lk(process_queue_mutex);

	task_.set_tp_task_id(this->current_task);
	this->current_task = this->current_task + 1;
	task_.set_tp_task_status(TP_TASK_ENQUEUED_PROCESS_QUEUE);
	this->check_start_time(task_);

	this->process_q_conditional_var.wait(lk, [this, &task_]() {
		this->process_queue_ptr->push(&task_);
		*(this->add_to_process_q) = true;
		return *(this->add_to_process_q) && !(*(this->get_from_process_q));
		});

	*(this->add_to_process_q) = false;
	
	lk.unlock();
	
	this->process_q_conditional_var.notify_all();
	
	this->process_task();
	
	return 0;
}

uint32_t TP_Implementation_::process_task() {

	if (this->process_queue_ptr->size() == 0) return TP_NO_TASK_ENQUEUED_TO_PROCESS_QUEUE;

	uint32_t x = this->tp_find_free_thread_for_task();

	if (x != TP_NO_THREAD_FOR_TP_TASK) {

		try {

			std::unique_lock <std::mutex> lk(process_queue_mutex);

			TP_Task * task_;

			this->process_q_conditional_var.wait(lk, [this, &task_]() {
				if (this->process_queue_ptr->size() > 0) {
					task_ = this->process_queue_ptr->front();

					this->process_queue_ptr->pop();
					*(this->get_from_process_q) = true;
					return *(this->get_from_process_q) && !(*(this->add_to_process_q));
				}
				else return true;
				});

			*(this->get_from_process_q) = false;

			lk.unlock();

			this->process_q_conditional_var.notify_all();

			if (task_->get_tp_task_cb() == nullptr) {
				return TP_NO_TASK_ENQUEUED_TO_PROCESS_QUEUE;
			}

			if (task_->get_tp_task_status() == TP_TASK_ENQUEUED_PROCESS_QUEUE) {

				tp_task_cb task_cb = task_->get_tp_task_cb();

				task_->set_tp_task_status(TP_TASK_RUN);

				this->thread_vec[x] = std::thread(&TP_Implementation_::tp_task_function_, this, *task_);

				#ifdef __linux__
				    this->thread_status_vec[x] = true;
					pthread_t t_handle = this->thread_vec[x].native_handle();
					this->tid_m->insert({t_handle, 0});
				#endif

				//this->thread_vec[x] = std::thread(task_cb);

				this->task_m->insert({ task_->get_tp_task_id(), x });

				this->task_vec.push_back(*task_);

				*(this->get_from_process_q) = false;

				return x;
			}
			else {

				*(this->get_from_process_q) = false;

				lk.unlock();

				this->process_q_conditional_var.notify_all();

				return TP_TASK_INVALID_STATUS;
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;

			return TP_NO_THREAD_FOR_TP_TASK;
		}
	}

	return TP_NO_THREAD_FOR_TP_TASK;

}

tp_task_status TP_Implementation_::get_task_status(TP_Task task_) {
	return task_.get_tp_task_status();
}

bool TP_Implementation_::check_task_completed_native(TP_Task& task_,
	                                                               std::string &output) {
	return this->check_task_completed_native(task_, 3000, output);
}

bool TP_Implementation_::check_task_completed_native(TP_Task& task_,
	                                                              uint64_t time_in_milliseconds, std::string &output) {

	tp_task_id task_id = task_.get_tp_task_id();

	tp_task_status task_status_ = task_.get_tp_task_status();

	#if WIN32
	assert((task_status_ <= TP_TASK_MAX_VALUE ||
		                                 task_status_ >= TP_TASK_MIN_VALUE),
										                   " Invalid TP Task Status ");
	#else
	#ifdef __linux__
		assert((task_status_ <= TP_TASK_MAX_VALUE ||
		                                        task_status_ >= TP_TASK_MIN_VALUE));
	#endif
	#endif

	output.append("{ \n");
	output.append(" \"method\" : \" THREAD POOL ++ check_task_completed_native \", \n");
	output.append(" \"task_id\" : ").append(std::to_string(task_.get_tp_task_id())).append(", \n");

	try {

		// if task is at the process_queue

		if (task_.get_tp_task_status() == TP_TASK_ENQUEUED_PROCESS_QUEUE) {
			task_.set_tp_task_status(TP_TASK_ENDED);
			output.append(" \"thread_status\": \"Task enqued to process queue, No thread assigned to the task\",\n");
			output.append(" \"task_status\": \"").append(tp_task_status_str_arr[task_.get_tp_task_status()]).append("\"\n }");
			return false;
		}

		else if (task_.get_tp_task_status() == TP_TASK_ENDED) {
			output.append(" \"thread_status\": \" Task ended, No thread assigned to the task\",\n");
			output.append(" \"task_status\": \"").append(tp_task_status_str_arr[task_.get_tp_task_status()]).append("\"\n }");
			return false;
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

			#ifdef WIN32

			HANDLE win_handle = this->thread_vec.at(thread_index).native_handle();

			DWORD val_ ;

			if (win_handle != NULL && win_handle != INVALID_HANDLE_VALUE) {
				//CloseHandle(win_handle);
				val_ = WaitForSingleObject(win_handle, time_in_milliseconds);

				if (val_ == WAIT_ABANDONED) {
					output.append(" \"thread_status\" : \" WINDOWS thread, thread status wait abandoned \", \n");
				}
				else if (val_ == WAIT_OBJECT_0) {
					output.append(" \"thread_status\" : \" WINDOWS thread, thread status wait object 0 \", \n");
				}
				else if (val_ == WAIT_TIMEOUT) {
					output.append(" \"thread_status\" : \" WINDOWS thread, thread status wait timeout \", \n");
				}
				else if (val_ == WAIT_FAILED) {
					output.append(" \"thread_status\" : \" WINDOWS thread, thread status wait failed\", \n");
				}
				else {
					output.append(" \"thread_status\" : \" WINDOWS thread, thread status wait ")
						.append(std::to_string(val_))
						.append(" \", \n");
				}

			}

			std::string t_output_usage = this->check_thread_status_native(win_handle, task_.get_tp_task_id(), TP_TASK_COMPLETED);

			this->thread_status->insert({task_id , t_output_usage});

			#else

				#ifdef __linux__

					pthread_t handle_ = this->thread_vec.at(thread_index).native_handle();

					timespec ts_ = {0};

					ts_.tv_sec  = (long int) (time_in_milliseconds / 1000);

					ts_.tv_nsec = (time_in_milliseconds % 1000);

					ts_.tv_nsec *= 1000;

					int output_ = pthread_timedjoin_np(handle_, NULL, &ts_);

					std::string output_str = util_thread_join_output_to_str_linux(output_);

					output.append(" \"thread_status\" : \"").append(output_str).append("\", \n");

					if (output_ == PTHREAD_JOINED) {
						task_.set_tp_task_status(TP_TASK_COMPLETED);
						this->task_vec.at(thread_index).set_tp_task_status(TP_TASK_COMPLETED);
						this->check_end_time(task_);
						task_status_ = task_.get_tp_task_status();
				        output.append(" \"task_status\" : \"").append(tp_task_status_str_arr[task_status_]).append("\" \n");
			            output.append("}\n");
				        return true;
					}

				#endif

			#endif

			try {
				if (this->thread_vec.at(thread_index).joinable()) {
					this->thread_vec.at(thread_index).join();
				}
				task_.set_tp_task_status(TP_TASK_COMPLETED);
				this->task_vec.at(thread_index).set_tp_task_status(TP_TASK_COMPLETED);
			}
			catch (std::exception &e){
				std::cout << e.what() << std::endl;
				task_.set_tp_task_status(TP_TASK_ENDED);
				this->task_vec.at(thread_index).set_tp_task_status(TP_TASK_ENDED);
			}

			this->check_end_time(task_);

			task_status_ = task_.get_tp_task_status();

			output.append(" \"task_status\" : \"").append(tp_task_status_str_arr[task_status_]).append("\" \n");

			output.append("}\n");

			//this->task_m->insert({ task_.get_tp_task_id() , TP_TASK_CANCELLED });

			return true;
		}

		else {
			task_status_ = task_.get_tp_task_status();
			std::cout << "task_status = " << task_status_ << std::endl;
			output.append(" \"task_status\" : \"").append(tp_task_status_str_arr[task_status_]).append("\" \n");
			output.append("}\n");
			return false;
		}
	}
	catch (std::exception& e) {

		std::cout << e.what() << std::endl;

		std::cout << "task_status = " << task_status_ << std::endl;
		output.append(" \"task_status\" : \"").append(tp_task_status_str_arr[task_status_]).append("\" \n");
		output.append("}");

		return false;
	}

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

		DWORD exitCode = 0;

		GetExitCodeThread(win_handle, &exitCode);

		bool thread_is_busy = (exitCode == STILL_ACTIVE) ? true : false;

		if (thread_is_busy) continue;
		else {
			if (this->thread_vec[x].joinable()) {
				this->thread_vec[x].join();
				return x;
			}
			else {
				return x;
			}
		}

#else
	#ifdef __linux__
        // pthread implementation for linux/macos OS
	
	try {
		if (this->thread_status_vec[x] == false) {
			if (this->thread_vec[x].joinable()) {
				this->thread_vec[x].join();
			}
			return x;
		}
		else continue;
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;	
	}
	#endif

#endif

	}

	return TP_NO_THREAD_FOR_TP_TASK;
}


std::string TP_Implementation_::get_task_runtime_status(tp_task_id task_id ,
																tp_task_runtime_data* task_runtime_data) {

	std::string str_ = "";

	// Use WIN32 Thread Information API also

	int x = TP_NO_THREAD_FOR_TP_TASK;

	if (this->task_m->find(task_id) != this->task_m->end()) {
		x = this->task_m->at(task_id);
	}
	else {
		str_.append(" error runtime status, Task with task_id = ")
			.append(std::to_string(task_id))
			.append(" not enqueued ");
		return str_;
	}

	#ifdef WIN32
	HANDLE win_handle = this->thread_vec[x].native_handle();

	DWORD exitCode;

	GetExitCodeThread(win_handle, &exitCode);

	bool thread_is_busy = (exitCode == STILL_ACTIVE) ? true : false;

	DWORD thread_id = GetThreadId(win_handle);

	/*
	THREAD_INFORMATION_CLASS thread_info_class = ThreadAbsoluteCpuPriority;

	LPVOID thread_info;

	DWORD thread_info_class_size = sizeof(ThreadAbsoluteCpuPriority);

	// windows thread info
	bool windows_thread_info_ = GetThreadInformation(win_handle,
													thread_info_class,
													&thread_info ,
													thread_info_class_size
													);
	*/
	#else

		bool thread_is_busy = false;
		
		int thread_id = 0;
		
		#ifdef __linux__

			pthread_t handle_ = this->thread_vec[x].native_handle(); 

			thread_is_busy = this->thread_status_vec[task_id];

			if (this->tid_m->find(handle_) != this->tid_m->end()) {
			  thread_id = this->tid_m->at(handle_);
			}
			else {
			  thread_id = 0;
			}

		#endif

	#endif

	TP_Task task_ = this->task_vec.at(x);

	tp_task_status task_status_ = task_.get_tp_task_status();

	std::string str_task_id = std::to_string(task_id);

	std::string str_thread;
	std::string str_thread_is_busy;
	
	str_thread = std::to_string(thread_id);
	str_thread_is_busy = std::to_string(thread_is_busy);

	std::chrono::system_clock::time_point now_clock = std::chrono::system_clock::now();

	std::time_t curr_t = std::chrono::system_clock::to_time_t(now_clock);

	char* curr_t_str = std::ctime(&curr_t);
    
	size_t curr_str_len = strlen(curr_t_str);

	UTIL_STRING_NEWLINE(curr_t_str, curr_str_len);

	#ifdef WIN32
	assert((task_status_ <= TP_TASK_MAX_VALUE ||
						task_status_ >= TP_TASK_MIN_VALUE),
											" Invalid TP Task Status ");
	#else 
	#ifdef __linux__
	assert((task_status_ <= TP_TASK_MAX_VALUE) ||
                                                  task_status_ >= TP_TASK_MIN_VALUE);
	#endif
	#endif
	
	const char* str_task_status = tp_task_status_str_arr[task_status_];

	tp_time_milliseconds t_start = std::chrono::duration_cast <tp_time_milliseconds>
		(task_.get_tp_task_start_time() - this->tp_instance_init_time);

	tp_time_milliseconds t_end = tp_time_milliseconds(0);

	if (task_status_ == TP_TASK_ENDED || task_status_ == TP_TASK_COMPLETED)
	{
		t_end = std::chrono::duration_cast <tp_time_milliseconds>
			(task_.get_tp_task_end_time() - this->tp_instance_init_time);
	}

	tp_time_milliseconds t_duration = task_.get_tp_task_duration();

	std::string t_start_str = std::to_string(t_start.count());

	std::string t_end_str = std::to_string(t_end.count());

	std::string t_duration_str = std::to_string(t_duration.count());

	std::string thread_info_str;

	str_.append("{").append(" \n")
		.append(" \"current_date_time\" : ").append("\" ").append(curr_t_str).append("\", \n")
		.append(" \"method\" : \" Thread Pool ++ task_runtime_status \", \n")
		.append(" \"task_id\" : ").append(str_task_id).append(", \n")
		.append(" \"task status\" : ").append(" \"").append(str_task_status).append("\"").append(", \n")
		.append(" \"task_start_time\" : ").append(t_start_str).append(", \n")
		.append(" \"task_end_time\" : ").append(t_end_str).append(", \n")
		.append(" \"task_duration_time\" : ").append(t_duration_str).append(", \n")
		.append(" \"thread_id\" : ").append(str_thread).append(", \n")
		.append(" \"thread_run_status\" : ").append(str_thread_is_busy).append(" \n")
		//.append(" \"thread_info\" : ").append(thread_info_str).append("\n")
		.append("}").append(" \n");

	if (task_runtime_data != nullptr) {

		task_runtime_data->task_id = task_id;
		task_runtime_data->task_status = task_status_;
		task_runtime_data->task_start = t_start;
		task_runtime_data->task_end = t_end;
		task_runtime_data->task_duration = t_duration;
		task_runtime_data->thread_id = thread_id;
		task_runtime_data->thread_run_status = thread_is_busy;
		task_runtime_data->curr_time = curr_t;

	}

	return str_;
}

std::string TP_Implementation_::get_task_runtime_status(tp_task_id task_id) {

	return this->get_task_runtime_status(task_id, nullptr);

}

void TP_Implementation_::tp_task_function_(TP_Task t_) {
	
	tp_task_id task_id = t_.get_tp_task_id();

	tp_task_cb run_task = std::bind(t_.get_tp_task_cb(),
		std::placeholders::_1,
		std::placeholders::_2);

	try {
		t_.set_tp_task_status(TP_TASK_RUN); 
		
		#ifdef __linux__
			
		    int t_id = this->check_tid_thread(); 
			int thread_id;
			pthread_t p_thread_handle;

			if (this->task_m->find(task_id) != this->task_m->end()) {  
				p_thread_handle =  this->thread_vec[task_id].native_handle();
			}
			
			this->tid_m->insert({p_thread_handle , t_id});
			this->thread_status_vec[task_id] = true;
		#endif

		run_task(t_.get_tp_task_input_ptr() , t_.get_tp_task_output_ptr());
		if (t_.get_tp_task_id() < this->task_vec.size()) {
			this->task_vec.at(t_.get_tp_task_id()).set_tp_task_status(TP_TASK_COMPLETED);
		}
		this->check_end_time(t_);
		t_.set_tp_task_status(TP_TASK_COMPLETED);
		this->task_vec[task_id].set_tp_task_status(TP_TASK_COMPLETED);
		this->check_thread_status_native(t_);
		
		#ifdef __linux__ 
			this->thread_status_vec[task_id] = false;
			//this->tid_m->insert({p_thread_handle, 0});
		#endif

	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		t_.set_tp_task_status(TP_TASK_ENDED);
		this->task_vec[task_id].set_tp_task_status(TP_TASK_ENDED);
		this->check_end_time(t_);
		this->check_thread_status_native(t_);
	}

}

bool TP_Implementation_::check_task_completed(TP_Task task_) {

	if (task_.get_tp_task_status() != TP_TASK_RUN) {
		return true;
	}

	try {

	int x = TP_NO_THREAD_FOR_TP_TASK;

	tp_task_id task_id = task_.get_tp_task_id() ;

	if (this->task_m->find(task_id) != this->task_m->end()) {
		x = this->task_m->at(task_id);
	}
	else return true;

	if (this->thread_vec[x].joinable()) {
		this->thread_vec[x].join();
		task_.set_tp_task_status(TP_TASK_COMPLETED);
	}

	return true;

}
catch (std::exception& e) {
	std::cout << e.what() << std::endl;
	return false;
}

return true;
}

bool TP_Implementation_::check_all_tasks_completed() {

	bool value = false;

	for (TP_Task t : this->task_vec) {
		try {
			value = (value || this->check_task_completed(t));
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			value = false;
		}
	}

	for (std::thread &t : this->thread_vec) {

		if (t.joinable()) {
			try {
				t.join();
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
		}

	}

	return value;
}

void TP_Implementation_::check_start_time(TP_Task& t) {
	tp_task_time t_ = std::chrono::steady_clock::now();
	t.set_tp_task_start_time(t_);
}

bool TP_Implementation_::end_task(TP_Task& task_) {

#ifdef  WIN32
	try {
		tp_task_id task_id = task_.get_tp_task_id();
		int32_t thread_id = TP_NO_THREAD_FOR_TP_TASK;

		if (this->task_m->find(task_id) != this->task_m->end()) {
			thread_id = this->task_m->at(task_id);
		}
		else {
			return false;
		}

		HANDLE win_handle = this->thread_vec[task_id].native_handle();
		DWORD dwExitCode = 0;

		TerminateThread(win_handle, dwExitCode);

		std::this_thread::sleep_for(tp_time_nanoseconds(TP_END_TASK_WAIT_TIME));

		this->task_vec[task_id].set_tp_task_status(TP_TASK_ENDED);
		task_.set_tp_task_status(TP_TASK_ENDED);

		std::string output_ = this->check_thread_status_native(task_);

		this->thread_status->insert({task_id, output_});

		if (this->thread_vec[task_id].joinable()) {
			this->thread_vec[task_id].join();
		}

		this->check_end_time(task_);
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
#else
	// linux and macOS implementation
	return false;
#endif
}

std::string TP_Implementation_::check_thread_status_native(TP_Task& task_) {

	std::string output_;

	tp_task_id task_id = task_.get_tp_task_id();

	if (this->thread_status->find(task_id) != this->thread_status->end()) {
		output_ = this->thread_status->at(task_id);
		return output_;
	}

	tp_task_status task_status = task_.get_tp_task_status();

	uint32_t thread_id = TP_NO_THREAD_FOR_TP_TASK;

	if (this->task_m->find(task_id) != this->task_m->end()) {
		thread_id = this->task_m->at(task_id);
	}
	else {
		return output_;
	}

#ifdef WIN32
	HANDLE win_handle = this->thread_vec.at(thread_id).native_handle();
	output_ = this->check_thread_status_native(win_handle, task_id , task_status);

	if (task_status == TP_TASK_COMPLETED &&
							                task_status == TP_TASK_ENDED) {
		this->thread_status->insert({ task_id, output_ });
	}
#else
	
	#ifdef __linux__

	#endif

#endif

	return output_;
}


#ifdef WIN32
std::string TP_Implementation_::check_thread_status_native(HANDLE win_handle, tp_task_id task_id,
	                                                                    tp_task_status task_status) {

	std::string output_;

	if (this->thread_status->find(task_id) != this->thread_status->end()) {
		output_ = this->thread_status->at(task_id);
		return output_;
	}

	int win_thread_priority = GetThreadPriority(win_handle);

	std::string win_thread_priority_str = UTIL_THREAD_PRIORITY_WINDOWS(win_thread_priority);

	FILETIME thread_creation_time;
	FILETIME thread_exit_time;
	FILETIME thread_kernel_time;
	FILETIME thread_user_time;

	std::string thread_creation_time_str = "";
	std::string thread_end_time_str = "";

	GetThreadTimes(
		win_handle,
		&thread_creation_time,
		&thread_exit_time,
		&thread_kernel_time,
		&thread_user_time
	);

	bool thread_running_status = false;

	if (thread_exit_time.dwHighDateTime == 0 &&
											thread_exit_time.dwLowDateTime == 0) {
		thread_running_status = true;
	}

	SYSTEMTIME thread_creation_system_time;
	SYSTEMTIME thread_exit_system_time;

	SYSTEMTIME thread_creation_system_time_value;
	SYSTEMTIME thread_exit_system_time_value;

	FileTimeToSystemTime(&thread_creation_time, &thread_creation_system_time);

	if (!thread_running_status) FileTimeToSystemTime(&thread_exit_time, &thread_exit_system_time);

	TIME_ZONE_INFORMATION tmz_info;
	GetTimeZoneInformation(&tmz_info);

	SystemTimeToTzSpecificLocalTime(
		&tmz_info,
		&thread_creation_system_time,
		&thread_creation_system_time_value
	);

	if (!thread_running_status) {
		SystemTimeToTzSpecificLocalTime(
		    &tmz_info,
			&thread_exit_system_time,
			&thread_exit_system_time_value
		);
	}

	// Time zone standard name

	thread_creation_time_str = util_time_to_string_windows(thread_creation_system_time_value, &tmz_info.StandardName[0]);

	if (!thread_running_status) {
		thread_end_time_str = util_time_to_string_windows(thread_exit_system_time_value, &tmz_info.StandardName[0]);
	}
	else {
		thread_end_time_str = "none";
	}

	std::string str_;

	output_.append("{\n").append(" \"method\": \"check CPU thread status native\", \n")
		.append(" \"task_id\": \"").append(std::to_string(task_id)).append("\", \n")
		.append(" \"platform\": ").append(" \"WINDOWS\", \n")
		.append(" \"start_time\": \"").append(thread_creation_time_str).append("\",\n")
	    .append(" \"complete_time\": \"").append(thread_end_time_str).append("\",\n")
		.append(" \"os_thread_priority\": \"").append(win_thread_priority_str).append("\" \n")
	    .append("}\n");

	return output_;
}
#endif

void TP_Implementation_::check_end_time(TP_Task& t) {
	tp_task_time t_start_ = t.get_tp_task_start_time();
	tp_task_time t_ = std::chrono::steady_clock::now();
	tp_time_milliseconds t_duration = std::chrono::duration_cast <tp_time_milliseconds> (t_ - t_start_);
	int task_id = t.get_tp_task_id();
	this->task_vec[task_id].set_tp_task_duration(t_duration);
	this->task_vec[task_id].set_tp_task_end_time(t_);
}

void TP_Implementation_::set_time_check_process_q_(int32_t time_) {
	this->time_check_process_q_ = time_;
}

int32_t TP_Implementation_::get_time_check_process_q_() {
	return this->time_check_process_q_;
}

#ifdef __linux__	
int TP_Implementation_::check_tid_thread() {
		return gettid();
	}
#endif

typedef TP_Implementation_ TP_CPU_CLASS;
}