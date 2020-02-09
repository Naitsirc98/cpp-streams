#include "streams.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <string>
#include <list>

template<typename T>
std::vector<T> createVector(T initial, T limit, T step = 1, size_t repeatCount = 1)
{
	std::vector<T> vec;

	for(T value = initial;value <= limit;value += step)
	{
		for(size_t i = 0;i < repeatCount;++i)
		{
			vec.push_back(value);
		}
	}

	vec.shrink_to_fit();

	return std::move(vec);
}

bool greaterThanZero(int i)
{
	return i > 0;
}

bool isEvenNumber(int i)
{
	return i % 2 == 0;
}

int sum(int a, int b)
{
	return a + b;
}

std::string toString(int i)
{
	return "Number = " + std::to_string(i);
}

float toFloat(int i)
{
	return 0.0f;
}

template<typename T, typename Container = std::unordered_map<size_t, T>>
struct MapCollector
{
	using ContainerType = Container;

	Container m_Container;

	void insert(const T& element)
	{
		static size_t index = 0;
		m_Container[index++] = element;
	}

	ContainerType operator*()
	{
		return std::move(m_Container);
	}

};

template<typename Container, typename T = typename Container::value_type>
void print(const Container& container)
{
	for(const T& element : container)
	{
		std::cout << element << '\n';
	}
}

template<
	typename Container,
	typename K = typename Container::key_type,
	typename V = typename Container::value_type>
void printMap(const Container& container)
{
	for(auto element : container)
	{
		std::cout << element.first << " => " << element.second << '\n';
	}
}

int main()
{

	std::vector<int> vector = createVector<int>(1, 50, 1, 5);

	using StrVector = std::vector<std::string>;

	auto result = stream::of(vector)
		.filter(isEvenNumber)
		.distinct()
		.map<std::string>(toString)
		.collect<StrVector>();

	print(result);

	std::cin.get();

	return 0;
}