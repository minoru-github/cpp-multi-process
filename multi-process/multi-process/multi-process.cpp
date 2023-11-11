#include <iostream>
#include <vector>
#include <future>

int some_function(const std::vector<int>& values)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	int sum = 0;
	for (auto& value : values)
	{
		sum += value;
	}

	return sum;
}

int main()
{
    // 非同期実行
	std::vector<std::future<int>> futures;

	for (int i = 0; i < 8; ++i)
	{
		std::vector<int> values;
		for (int j = i; j < i+100; ++j)
		{
			values.push_back(j);
		}
		futures.push_back(std::async(std::launch::async, some_function, values));
	}

	for (auto& future : futures)
	{
		std::cout << future.get() << std::endl;
	}

	return 0;
}
