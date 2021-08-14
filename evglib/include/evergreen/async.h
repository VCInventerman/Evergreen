#pragma once

namespace evg
{
	class GroupSignal
	{
	public:
		bool ready = false;
		std::mutex m;
		std::condition_variable c;

		void signal() noexcept
		{
			{
				std::unique_lock<std::mutex> lock(m);
				ready = true;
			}
			c.notify_one();
		}

		void watch()
		{
			std::unique_lock<std::mutex> lock(m);
			while (!ready)
			{
				c.wait(lock);
				if (!ready)
				{
					std::cout << "UNLOCK WITHOUT READY\n";
				}
			}
		}

		~GroupSignal()
		{
			std::unique_lock<std::mutex> lock(m);
		}
	};




	template<typename RetT = void>
	struct WaitableTask
	{

		struct VoidResultHolder
		{


			void return_void()
			{
			}
		};

		struct ResultHolder
		{

			RetT result;

			void return_value(RetT val)
			{
				result = val;
			}
		};

		struct promise_type : public std::conditional<std::is_same<RetT, void>::value, VoidResultHolder, ResultHolder>::type
		{
			GroupSignal* signal = nullptr;

			WaitableTask get_return_object() { return WaitableTask(*this); }
			std::suspend_never initial_suspend() { return {}; }
			std::suspend_never final_suspend() noexcept
			{
				signal->signal();
				return {};
			}
			void unhandled_exception() {}
		};


		GroupSignal signal;

		WaitableTask(promise_type& _promise)
		{
			_promise.signal = &signal;
		}
		WaitableTask(const WaitableTask&) = delete;
		WaitableTask& operator= (const WaitableTask& rhs)
		{
			return std::move(rhs);
		}

		promise_type promise;
	};


	struct AsyncSleeper : std::suspend_always
	{
	public:
		using This = AsyncSleeper;

		boost::asio::high_resolution_timer timer;

		AsyncSleeper(const std::chrono::milliseconds _time) : timer(threads.c(), boost::asio::chrono::milliseconds(_time)) {}

		void await_suspend(std::coroutine_handle<> handle)
		{
			//std::coroutine_handle<> mover = std::move(handle);

			timer.async_wait(std::bind(&This::waitComplete, std::move(handle), std::placeholders::_1));
		}

		static void waitComplete(std::coroutine_handle<> handle, const std::error_code error)
		{
			if (error)
			{
				throw std::runtime_error(error.message());
			}

			handle.resume();
		}
	};


	auto AsyncSleep(std::chrono::milliseconds time)
	{
		return AsyncSleeper(time);
	}


	
	struct AsyncTask {};
}



namespace std
{
	template<typename ... Ts>
	struct coroutine_traits<evg::AsyncTask, Ts ...>
	{
		struct promise_type
		{
			evg::AsyncTask get_return_object() const noexcept
			{
				return{};
			}

			void return_void() const noexcept
			{
			}

			suspend_never initial_suspend() const noexcept
			{
				return{};
			}

			suspend_never final_suspend() const noexcept
			{
				return{};
			}

			void unhandled_exception() noexcept
			{
				std::terminate();
			}
		};



	};


}