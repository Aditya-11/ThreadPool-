Thread Pool ++ 

C++ implmentation of Threadpool pattern, using the threadpool pattern is better for runtime performance of the program than creating a new thread in the program when required. Threadpool manages the thread and task resources. 

Threadpool pattern link : https://en.wikipedia.org/wiki/Thread_pool

<b> Libraries link </b>
* Standard Template Library link : https://en.wikipedia.org/wiki/Standard_Template_Library 
* Win32   link : https://en.wikipedia.org/wiki/Windows_API
* Pthread link : https://en.wikipedia.org/wiki/Pthreads

<b> Implementation and method details </b>
* Add ThreadPool++.h header file to the c++ project.
* thread pool manages tasks and threads resouces, create the tasks which can run parallely, enqueue the task to Thread pool.
* When thread pool memory is freed, thread pool checks and waits for all the task to complete.
* Thread Pool ++ implemented for only windows OS, Pthread implementation is pending for Macos and linux.
* create a new task object via TP_Task class, add function (code) via set_tp_task_cb method to run parallel task.
* Set input of the task via set_tp_task_input_ptr method and set output of the task via set_tp_task_output_ptr. typecast the tp_task_input_ptr and tp_task_output_ptr variables, check ThreadPool++.cpp source code.
* enqueue the task to ThreadPool.
* use the tp_task_runtime_status method to get the runtime information about the task.
* task runtime info are task start time, task end time, thread assigned to task, whether thread is running, task status.
* use tp_check_task_complete method, to check and wait for the task to complete.
* use tp_check_task_complete_native, checks and waits for the task to complete, uses native function to get information about the task.
* end_task implementation pending. Add feature code for other hardware accelerators.

<b> Code example, Check the ThreadPool++.cpp code </b> 

```
  // initialize the parallel task and add function, pointer to input and output data via TP_Task methods.
  // Check the ThreadPool++.cpp code to initialize the TP_Task Object
  TP::TP_Task * task = new TP::TP_Task();

  // Initialize Thread Pool   
  TP::TP_CPU_CLASS * tp = new TP::TP_CPU_CLASS();

  // Enqueue the task to tp 
  tp->enqueue_task(task);

  // get runtime status of the task in JSON.
  std::string task_status_output;
  tp->get_task_runtime_status(task_id , &task_status_output);
  
  // check and wait for the task, get task completed output in JSON.
  std::string task_completed_output;
  uint32_t task_timeout = 5000; // value in milliseconds 
  tp->check_task_completed_native(task_id, task_timeout, &task_completed_output);  

  // free the heap memory for task and thread pool 
  delete task;
  delete tp;

```

<b> Output </b>

```


// task_runtime_status output when task was running
// task_start_time, task_end_time and task_duration_time are in milliseconds

{
 "current_date_time" : " Mon Apr  6 00:54:59 2026 ",
 "method" : " Thread Pool ++ task_runtime_status ",
 "task_id" : 2,
 "task status" :  "TP_TASK_RUN",
 "task_start_time" : 517,
 "task_end_time" : 0,
 "task_duration_time" : 0,
 "thread_id" : 10752,
 "thread_run_status" : 1,
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
 "current_date_time" : " Mon Apr  6 00:55:07 2026 ",
 "method" : " Thread Pool ++ task_runtime_status ",
 "task_id" : 2,
 "task status" :  "TP_TASK_COMPLETED",
 "task_start_time" : 517,
 "task_end_time" : 5289,
 "task_duration_time" : 4771,
 "thread_id" : 0,
 "thread_run_status" : 0,
}


```


