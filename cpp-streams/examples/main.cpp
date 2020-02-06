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

bool greater_than_zero(int i)
{
	return i > 0;
}

int main()
{
	std::vector<int> vector = createVector<int>(1, 100);

	auto first = stream::of(vector)
		.filter([&](int i) -> bool {return i % 2 == 0;})
		.max([&](int a, int b) -> int {return a > b ? 1 : a == b ? 0 : -1;}).value();
		// .forEach([&](int i) -> void {std::cout << i << '\n';});

	std::cout << first << '\n';

	std::cin.get();

	return 0;
}