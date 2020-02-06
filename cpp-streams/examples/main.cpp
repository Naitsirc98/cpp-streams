#include "streams.hpp"
#include <iostream>
#include <vector>

template<typename T>
std::vector<T> createVector(T initial, T limit, T step = 1)
{
	std::vector<T> vec;

	for(T value = initial;value <= limit;value += step)
	{
		vec.push_back(value);
	}

	vec.shrink_to_fit();

	return std::move(vec);
}

int main()
{
	std::vector<int> vector = createVector<int>(1, 100);

	auto count = stream::of(vector)
		.filter([&](int i) -> bool {return i % 2 == 0;})
		.count();
		// .forEach([&](int i) -> void {std::cout << i << '\n';});

	std::cout << count << '\n';

	std::cin.get();

	return 0;
}