# <b> Thread Pool ++ </b> 

C++ implementation of Threadpool pattern, using the thread pool pattern is better for runtime performance of the program and maintaining the code, than creating a new thread in the program when required. Threadpool manages the thread and task resources. Create a parallel task object by initializing the TP_Task Class, add input, output and function cb to the task and enqueue the object to thread pool. Initially number of hyperthreads at CPU are created and tasks are assigned to the threads by the thread pool.

<br>

Threadpool pattern link : https://en.wikipedia.org/wiki/Thread_pool

<b> Libraries link </b>
* Standard Template Library link : https://en.wikipedia.org/wiki/Standard_Template_Library 
* Win32   link : https://en.wikipedia.org/wiki/Windows_API
* Pthread link : https://en.wikipedia.org/wiki/Pthreads

<b> Implementation and method details </b>

* Add ThreadPool++.h header file to the c++ project.
* thread pool manages tasks and threads resources, create the tasks which can run parallely, enqueue the task to Thread pool.
* When thread pool memory is freed, thread pool checks and waits for all the task to complete.
* Thread Pool ++ implemented for only windows OS, Pthread implementation is pending for Macos and linux.
* create a new task object via TP_Task class, add function (code) via set_tp_task_cb method to run parallel task.
* Set input of the task via set_tp_task_input_ptr method and set output of the task via set_tp_task_output_ptr. typecast the tp_task_input_ptr and tp_task_output_ptr variables, check ThreadPool++.cpp source code. Set the task function callback via set_tp_task_cb method.
* task process queue is checked at some time interval, if process queue is not empty then task is dequed from process queue and processed by the threadpool.
* enqueue the task to ThreadPool.
* use the tp_task_runtime_status method to get the runtime information about the task.
* task runtime info are task start time, task end time, thread assigned to task, whether thread is running, task status.
* use tp_check_task_complete method, to check and wait for the task to complete.
* create variable of type tp_task_runtime_data and add it to the input parameter of the function get_task_runtime_status to get runtime status of the thread.
* use tp_check_task_complete_native, checks and waits for the task to complete, uses native function to get information about the task.
* end_task implementation pending. Add feature code for other hardware accelerators.

<b> Code example, Check the ThreadPool++.cpp code </b> 

```
   
  // initialize the parallel task and add function, add input, output and cb function to the task.
  // Check the ThreadPool++.cpp code to initialize the TP_Task Object, by initializing the task_input, task_output and task_function_cb
  TP::TP_Task * task = new TP::TP_Task();
  
  // Initialize Thread Pool   
  TP::TP_CPU_CLASS * tp = new TP::TP_CPU_CLASS();
  
  // Enqueue the task to tp 
  tp->enqueue_task(task);
  
  // get runtime status of the task in JSON.
  std::cout << tp->get_task_runtime_status(task_id) << std::endl;
   
  // check and wait for the task, get task completed output in JSON.
  std::string task_completed_output;
  uint32_t task_timeout = 5000; // value in milliseconds 
  tp->check_task_completed_native(task_id, task_timeout, task_completed_output);  
  
  std::cout << task_completed_output << std::endl;
  
  // free the heap memory for task and thread pool 
  delete task;
  delete tp;
  
```

<b> Output </b>

```

// task_runtime_status output when task was running
// task_start_time, task_end_time and task_duration_time are in milliseconds
{
 "current_date_time" : " Mon Apr  6 20:52:46 2026 ",
 "method" : " Thread Pool ++ task_runtime_status ",
 "task_id" : 3,
 "task status" :  "TP_TASK_RUN",
 "task_start_time" : 332,
 "task_end_time" : 0,
 "task_duration_time" : 0,
 "thread_id" : 51756,
 "thread_run_status" : 1
}

// check_task_completed_native output
{
 "method" : " THREAD POOL ++ check_task_completed_native ",
 "task_id" : 3,
 "thread_status" : " WINDOWS thread, thread status wait object 0 ",
 "task_status" : "TP_TASK_COMPLETED"
}

// task_runtime_status output after task completed
{
 "current_date_time" : " Mon Apr  6 20:52:54 2026 ",
 "method" : " Thread Pool ++ task_runtime_status ",
 "task_id" : 3,
 "task status" :  "TP_TASK_COMPLETED",
 "task_start_time" : 332,
 "task_end_time" : 8313,
 "task_duration_time" : 7981,
 "thread_id" : 0,
 "thread_run_status" : 0
}

```


