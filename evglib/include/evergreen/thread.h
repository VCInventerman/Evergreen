#pragma once

#include <evergreen/ContiguousBuf.h>




// Thread pool
//todo: create more threads for waiting-heavy tasks based on suppied magnitude of time, custom context

namespace evg
{
	enum class ThreadStatus
	{
		Stopped = 0,
		Light, // Work that doesn't use too much CPU power - the default for most tasks, involves a lot of waiting on mutexes
		Heavy, // Continuous heavy work - shouldn't have more than the number of logical threads - ex. lightmap assembly, compilation
	};

	thread_local std::atomic<ThreadStatus> threadStatus = ThreadStatus::Stopped;

	void runWorkerThread(boost::asio::io_context* io_context)
	{
#if defined(EVG_PLATFORM_WIN)
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

		for (;;) // Infinite so the thread will resume after an exception
		{
			threadStatus = ThreadStatus::Light;

			try
			{
				io_context->run(); // Work for IO Context
				break; // Break loop if it exits normally
			}
			catch (std::exception& e)
			{
				std::cout << "Error in worker thread: " << e.what() << "\n";
			}
			catch (...)
			{
				std::cout << "Error in worker thread: Unknown\n";
			}
		}
	}



	/*struct IntAccessor
	{
		Int value;
	};

	template<>
	IntAccessor boost::asio::io_context::dispatch<IntAccessor>(IntAccessor&& ioc)
	{
		nop();

	}*/








	class ThreadPool
	{
	public:


		constexpr static Int DEFAULT_WORKING_THREAD_CAP = 4;
		constexpr static Int DEFAULT_BLOCKING_THREAD_CAP = 16;


	//protected:
		boost::asio::io_context context;


		union
		{
			char workGuardMem[sizeof(boost::asio::executor_work_guard<boost::asio::io_context::executor_type>)];
			boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard;
		};

		std::vector<std::thread> working;
		std::vector<std::thread> blocking;

		Int maxBlockingThreads;


	public:
		ThreadPool() {} // Should only be used for static construction where initialization must be ordered
		ThreadPool(const Int _workingThreads, const Int _maxBlockingThreads) { init(_workingThreads, _maxBlockingThreads); }

		~ThreadPool()
		{
			stop();
		}

		void init(const Int _workingThreads, const Int _maxBlockingThreads)
		{
			maxBlockingThreads = _maxBlockingThreads;
			new (workGuardMem) boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(context.get_executor());


			for (auto i : Range(_workingThreads))
			{
				working.push_back(std::thread(runWorkerThread, &context));
			}

			for (auto i : Range(DEFAULT_BLOCKING_THREAD_CAP))
			{
				blocking.push_back(std::thread(runWorkerThread, &context));

#ifdef _WIN32
				SetThreadPriority(blocking.back().native_handle(), -1);
#else
				sched_param sch;
				int policy;
				pthread_getschedparam(blocking.back().native_handle(), &policy, &sch);
				sch.sched_priority--;
				pthread_setschedparam(blocking.back().native_handle(), policy, &sch);
#endif
			}


		}

		void init()
		{
			init(Int((double)std::thread::hardware_concurrency() * 1.0) - 1, 128);
		}

		void stop()
		{
			workGuard.reset();
			context.stop();
			for (auto& t : working)
			{
				t.join();
			}
			for (auto& t : blocking)
			{
				t.join();
			}
		}

		void spin(Int num = 0)
		{
			if (num != 0)
			{
				for (auto i : Range(num))
				{
					context.post([]() {for (;;) {}});
				}
			}
			else
			{
				for (auto i : Range((Int)working.size()))
				{
					context.post([]() {for (;;) {}});
				}
			}
		}


		/*operator boost::asio::any_io_executor()
		{
			return context.get_executor();
		}*/

		operator boost::asio::io_context& ()
		{
			return context;
		}

		boost::asio::io_context& c() noexcept
		{
			return context;
		}

		boost::asio::any_io_executor ex() noexcept
		{
			return context.get_executor();
		}

		/*operator boost::asio::execution::any_executor<boost::asio::execution::context_as_t<boost::asio::execution_context&>, boost::asio::execution::detail::blocking::never_t<0>, boost::asio::execution::prefer_only<boost::asio::execution::detail::blocking::possibly_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::outstanding_work::tracked_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::outstanding_work::untracked_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::relationship::fork_t<0>>, boost::asio::execution::prefer_only<boost::asio::execution::detail::relationship::continuation_t<0>>>()
		{
			return context.get_executor();
		}*/

		template<typename T>
		auto post(T&& f, ThreadStatus hint = ThreadStatus::Light)
		{
			return boost::asio::post(context, f);
		}

#if defined(EVG_PLATFORM_WIN)
		HANDLE native()
		{
			return context.impl_.iocp_.handle;
		}
#endif
	};

	ThreadPool threads; //The thread pool is always global and acsessible to any function inside this process
}