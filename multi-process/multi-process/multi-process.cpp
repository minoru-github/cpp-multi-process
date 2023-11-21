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
	std::cout << "wait_for_example" << std::endl;

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

	std::cout << "complete" << std::endl << std::endl;
}

void wait_for_example2()
{
	std::cout << "wait_for_example2" << std::endl;

	auto some_function = [] {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	};

	std::vector<std::future<void>> futures;
	for (int i = 0; i < 10; ++i)
	{
		futures.push_back(std::async(std::launch::async, some_function));
	}

	// 全処理合計のタイムアウト時間を設定する
	auto now = std::chrono::system_clock::now();
	auto total = std::chrono::milliseconds(0);
	constexpr auto timeout = std::chrono::milliseconds(1000);
	auto remain = timeout - total;
	std::cout << "remain: " << remain.count() << std::endl;

	for (auto& future : futures)
	{
		const auto result = future.wait_for(remain);
		if (result == std::future_status::timeout)
		{
			// タイムアウトした場合は、残りの処理をキャンセルする
			// TODO
			break;
		}

		// 処理時間をstd::chrono::durationに変換し、合計時間に加算する
		const auto elapsed = std::chrono::system_clock::now() - now;
		total += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

		remain = timeout - total;
		if (remain <= std::chrono::milliseconds(0))
		{
			// タイムアウトした場合は、残りの処理をキャンセルする
			// TODO
			break;
		}
		std::cout << "remain: " << remain.count() << std::endl;
	}

	std::cout << "complete" << std::endl << std::endl;
}

void method_example() {
	std::cout << "method_example" << std::endl;

	class Hoge
	{
	public:
		Hoge() {};
		~Hoge() {};
		void func(const int id) {
			std::cout << "id:" << id << std::endl;
		}
	};

	std::vector<std::future<void>> futures;
	const auto hoge = Hoge();
	for (int n = 0; n < 10; n++)
	{
		// クラスのメソッドを非同期化する場合はインスタンスを引数として渡す必要がある
		futures.push_back(std::async(std::launch::async, &Hoge::func, hoge, n));
	}

	for (auto& future : futures)
	{
		future.wait();
	}

	std::cout << "complete" << std::endl << std::endl;
}

int main()
{
	get_example();
	wait_example();
	wait_for_examople();
	wait_for_example2();
	method_example();

	return 0;
}
