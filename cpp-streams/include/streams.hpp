#pragma once

#include <functional>

namespace stream
{

	// ===> STREAM TYPES <===


	/* Base class for all streams

		It provides a full interface for all possible operations with
		streams. Subclasses of this class must implement its pipeline stage
	
		==> T: the type of the elements of this stream
		==> Self: type of this stream object
	*/
	template<typename T, typename Self>
	class Stream;



	/* Stream that supplies elements from a collection of data. Elements
		provided by this type of stream are not modified
	
		==> T: the type of the elements of this stream
		==> Iterator: the iterator type of the data container

	*/
	template<class T, class Iterator>
	class SourceStream;


#define _STREAM_FRIEND_TYPES_ \
	template<class _T, class _Iterator> friend class SourceStream;\



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


	// =======================================================================================



	// ===> IMPLEMENTATION <===

	template<typename T, typename Self>
	class Stream
	{
		_STREAM_FRIEND_TYPES_

	public:

		virtual ~Stream() {}

		

	protected:

		virtual bool hasRemaining() = 0;
		
		virtual T next() = 0;
	};


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

}