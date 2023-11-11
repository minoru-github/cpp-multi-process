#include <iostream>
#include <vector>
#include <future>

void get_example()
{
	std::cout << "get_example" << std::endl;

	auto some_function = [](const std::vector<int>& values) {
		int sum = 0;
		for (const auto& value : values)
		{
			sum += value;
		}
		return sum;
	};

	std::vector<std::future<int>> futures;
	for (int i = 0; i < 10; ++i)
	{
		std::vector<int> values;
		for (int j = i; j < i+100; ++j)
		{
			values.push_back(j);
		}
		futures.push_back(std::async(std::launch::async, some_function, values));
	}

	// 非同期実行
	for (auto& future : futures)
	{
		std::cout << future.get() << std::endl;
	}

	std::cout << "complete" << std::endl;
	std::cout << std::endl;
}

void wait_example()
{
	std::cout << "wait_example" << std::endl;

	auto some_function = [] {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	};

	// 非同期実行
	std::vector<std::future<void>> futures;
	for (int i = 0; i < 10; ++i)
	{
		futures.push_back(std::async(std::launch::async, some_function));
	}

	// 終了待ち
	for (auto& future : futures)
	{
		future.wait();
	}

	std::cout << "complete" << std::endl;
	std::cout << std::endl;
}

void wait_for_examople()
{
	auto some_function = [] {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	};

	std::vector<std::future<void>> futures;
	for (int i = 0; i < 10; ++i)
	{
		futures.push_back(std::async(std::launch::async, some_function));
	}

	// 終了待ち
	auto now = std::chrono::system_clock::now();
	// この指定方法だと、イテレートされたfuture毎にwait_forされる。
	for (auto& future : futures)
	{
		std::future_status result = future.wait_for(std::chrono::milliseconds(100));
		if (result == std::future_status::ready)
		{
			std::cout << "ready" << std::endl;
		}
		else if (result == std::future_status::timeout)
		{
			std::cout << "timeout" << std::endl;
			auto elapsed = std::chrono::system_clock::now() - now;
			std::cout << "elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << std::endl;
		}
		else if (result == std::future_status::deferred)
		{
			std::cout << "deferred" << std::endl;
		}
	}
}

int main()
{
	get_example();
	wait_example();
	wait_for_examople();

	return 0;
}
