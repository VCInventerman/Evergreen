#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/dispatch.hpp>

#include "evergreen/common/types.h"

// Thread pool
//todo: create more threads for waiting-heavy tasks based on suppied magnitude of time, custom context

namespace evg
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

		constexpr static Size DEFAULT_WORKING_THREAD_CAP = 4;
		constexpr static Size DEFAULT_BLOCKING_THREAD_CAP = 16;


		boost::asio::io_context context;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type>* work_guard;

		std::vector<std::thread> threads;
		uint32_t cap = DEFAULT_WORKING_THREAD_CAP;
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
				//todo: second pool for blocking tasks that have very little cpu load
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

		operator boost::asio::execution::any_executor<boost::asio::execution::context_as_t<boost::asio::execution_context&>, boost::asio::execution::detail::blocking::never_t<0>, boost::asio::execution::prefer_only<boost::asio::execution::detail::blocking::possibly_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::outstanding_work::tracked_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::outstanding_work::untracked_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::relationship::fork_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::relationship::continuation_t<0>>>()
		{
			return context.get_executor();
		}
	};

	ThreadPool threads; //The thread pool is always global and acsessible to any function inside this process
}