#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/dispatch.hpp>

#include "../common/types.h"

// Thread pool
//todo: create more threads for waiting-heavy tasks based on suppied magnitude of time, custom context

namespace bgl
{

	void runWorkerThread(boost::asio::io_context* io_context)
	{
		for (;;) // Infinite so the thread will resume after an exception
		{
			try
			{
				io_context->run(); // Work for IO Context
				break; // Break loop if it exits normally
			}
			catch (std::exception& e)
			{
				std::cerr << "Error in worker thread: " << std::string(e.what()) << "\n";
			}
		}
	}

	class ThreadPool
	{
	public:
		boost::asio::io_context context;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type>* work_guard;

		std::vector<std::thread> threads;
		uint32_t cap = 0;
		bool active = false;

		void init(uint16_t _cap = 0)
		{
			if (_cap != 0)
			{
				cap = _cap;
			}
			else
			{
				// Allocate more threads than cores to reduce the penalty of waiting on mutexes
				//todo: find a better multipler
				cap = uint32_t((double)std::thread::hardware_concurrency() * 1.0) - 1;
			}

			work_guard = new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(context.get_executor());

			for (uint16_t i = 0; i < cap; i++)
			{
				threads.push_back(std::thread(runWorkerThread, &context));

				// Give worker threads a lower priority
#ifdef _WIN32
				SetThreadPriority(threads.back().native_handle(), -1);
#else
				sched_param sch;
				int policy;
				pthread_getschedparam(t1.native_handle(), &policy, &sch);
				sch.sched_priority--;
				pthread_setschedparam(threads.back().native_handle(), policy, &sch)
#endif
				
			}

			if (threads.size() > 0) { active = true; }
			else { std::cerr << "Unable to create worker thread!\n"; }
		}

		void stop()
		{
			work_guard->reset();
			delete work_guard;
			context.stop();
			for (auto& t : threads)
			{
				t.join();
			}
		}

		void spin(uint16_t num = 0)
		{
			if (num != 0)
			{
				for (uint16_t i = 0; i < num; i++)
				{
					context.post([]() {for (;;) {}});
				}
			}
			else
			{
				for (uint16_t i = 0; i < cap; i++)
				{
					context.post([]() {for (;;) {}});
				}
			}
		}
	};

	ThreadPool gThreads; //The thread pool is always global and acsessible to any function inside this process
}