#pragma once

#include <functional>
#include <optional>

namespace stream
{

	template<typename T>
	using Optional = std::optional<T>;

	// ===> OPERATION TYPES <===

	template<typename T, typename R>
	using Function = std::function<R(T)>;

	template<typename T1, typename T2, typename R>
	using BiFunction = std::function<R(T1, T2)>;

	template<typename T>
	using Predicate = Function<T, bool>;

	template<typename T>
	using Consumer = Function<T, void>;

	template<typename T1, typename T2>
	using BiPredicate = BiFunction<T1, T2, bool>;

	template<typename T>
	using Comparator = BiFunction<T, T, signed>;

	template<typename T>
	using Accumulator = BiFunction<T, T, T>;

// ===============================================================================================================================

	// ===> STREAM TYPES <===


	/*
		Base class for all streams

		It provides a full interface for all possible operations with
		streams. Subclasses of this class must implement its pipeline stage
	
		==> T: the type of the elements of this stream
		==> Self: type of this stream object. This will serve as the source for the next stream in the pipeline
	*/
	template<typename T, typename Self>
	class Stream;



	/*
		Stream that supplies elements from a collection of data. Elements
		provided by this type of stream are not modified
	
		==> T: the type of the elements of this stream
		==> Iterator: the iterator type of the data container

	*/
	template<typename T, typename Iterator>
	class SourceStream;

	
	/*
		Stream for filtering operations

		It takes an element from the previous stream and evaluates it.
		If the condition is true, the element will advance through the pipeline
		to the next stream. If the condition is returns false, the element is discarded.
	
	*/
	template<typename T, typename PreviousStream>
	class FilterStream;


#define _STREAM_FRIEND_TYPES_ \
	template<class _T, class _Iterator> friend class SourceStream;\
	template<class _T, class _PreviousStream> friend class FilterStream;\

// ===============================================================================================================================

	// ===> UTILITY FUNCTIONS <===


	/*
		Creates an empty stream
	*/
	template<typename T>
	SourceStream<T, T*> empty()
	{
		return SourceStream<T, T*>(nullptr, nullptr);
	}

	/*
		Creates a stream from the given array and the given size

		==> T* values: the data container
		==> size_t size: the size of the data collection 

	*/
	template<typename T>
	SourceStream<T, T*> of(T* values, size_t size)
	{
		return SourceStream<T, T*>(values, values + size);
	}

	/*
		Creates a stream from the given container.
		
		You may also specify its type and iterator

		==> Container& container: the data container

	*/
	template<
		class Container,
		class T = typename Container::value_type,
		class Iterator = typename Container::iterator>
	SourceStream<T, Iterator> of(Container& container)
	{
		return SourceStream<T, Iterator>(std::begin(container), std::end(container));
	}

	/*
		Creates a stream containing the elements within the given iterators

		==> Iterator begin: the start iterator
		==> Iterator end: the end iterator

	*/
	template<class T, class Iterator>
	SourceStream<T, Iterator> of(Iterator begin, Iterator end)
	{
		return SourceStream<T, Iterator>(std::move(begin), std::move(end));
	}


// ===============================================================================================================================


	// ===> IMPLEMENTATIONS <===

	template<typename T, typename Self>
	class Stream
	{
		_STREAM_FRIEND_TYPES_
	public:

		virtual ~Stream() {}

		// ===> Intermediate operations <===

		FilterStream<T, Self> filter(Predicate<T> condition)
		{
			return FilterStream<T, Self>(static_cast<Self*>(this), condition);
		}

		// ===> Terminal operations <===

		bool allMatch(Predicate<T> condition)
		{
			while(hasRemaining())
			{
				if(!condition(next()))
				{
					return false;
				}
			}
			return true;
		}

		bool anyMatch(Predicate<T> condition)
		{
			while(hasRemaining())
			{
				if(condition(next()))
				{
					return true;
				}
			}
			return false;
		}

		size_t count()
		{
			size_t count;

			for(count = 0;hasRemaining();++count)
			{
				next();
			}

			return count;
		}

		Optional<T> findFirst()
		{
			if(hasRemaining())
			{
				return next();
			}
			return {};
		}

		void forEach(Consumer<T> consumer)
		{
			while(hasRemaining())
			{
				consumer(next());
			}
		}

		Optional<T> max(Comparator<T> comparator)
		{
			return maxMinInternal([&](T next, T old) -> bool {return comparator(next, old) > 0;});
		}

		Optional<T> min(Comparator<T> comparator)
		{
			return maxMinInternal([&](T next, T old) -> bool {return comparator(next, old) < 0;});
		}

		bool noneMatch(Predicate<T> condition)
		{
			while(hasRemaining())
			{
				if(condition(next()))
				{
					return false;
				}
			}
			return true;
		}

		Optional<T> reduce(Accumulator<T> accumulator)
		{
			return reduceInternal({}, accumulator);
		}

		Optional<T> reduce(const T& identity, Accumulator<T> accumulator)
		{
			return reduceInternal(identity, accumulator);
		}

	protected:

		virtual bool hasRemaining() = 0;
		
		virtual T next() = 0;

	private:

		Optional<T> maxMinInternal(BiPredicate<T, T> comparator)
		{
			Optional<T> result;

			while(hasRemaining())
			{
				if(result.has_value())
				{
					T next_element = next();
					if(comparator(next_element, result.value()))
					{
						result = next_element;
					}
				}
				else
				{
					result = next();
				}
			}

			return std::move(result);
		}

		Optional<T> reduceInternal(Optional<T> result, Accumulator<T> accumulator)
		{
			while(hasRemaining())
			{
				result = result.has_value() ? accumulator(result.value(), next()) : next();
			}

			return std::move(result);
		}

	};

// ===============================================================================================================================

	template<typename T, typename Iterator>
	class SourceStream : public Stream<T, SourceStream<T, Iterator>>
	{
		_STREAM_FRIEND_TYPES_
	public:

		explicit SourceStream(Iterator begin, Iterator end)
			: m_Current(begin), m_End(end)
		{

		}

	protected:

		bool hasRemaining() override
		{
			return m_Current != m_End;
		}

		T next() override
		{
			T nextElement = *m_Current;
			++m_Current;
			return nextElement;
		}

	private:
		Iterator m_Current;
		const Iterator m_End;
	};


// ===============================================================================================================================

	template<typename T, typename PreviousStream>
	class FilterStream : public Stream<T, FilterStream<T, PreviousStream>>
	{
		_STREAM_FRIEND_TYPES_
	public:

		FilterStream(PreviousStream* previous, Predicate<T> condition)
			: m_Previous(*previous), m_Condition(condition)
		{

		}

	protected:
		
		bool hasRemaining() override
		{
			m_Next.reset();

			while(m_Previous.hasRemaining())
			{
				m_Next = m_Previous.next();

				if(m_Condition(m_Next.value()))
				{
					return true;
				}
			}

			return false;
		}

		T next() override
		{
			return m_Next.value();
		}

	private:
		PreviousStream m_Previous;
		Predicate<T> m_Condition;
		Optional<T> m_Next;
	};

}