// ---------------------------------------------------------------------------------------------------------------------------------
//  _ _     _
// | (_)   | |
// | |_ ___| |_
// | | / __| __|
// | | \__ \ |_
// |_|_|___/\__|
//
// Description:
//
//   Doubly-linked list
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   04/13/2001 by Paul Nettle: Original creation
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//   This software is 100% free. If you use this software (in part or in whole) you must credit the author. This software may not be
//   re-distributed (in part or in whole) in a modified form without clear documentation on how to obtain a copy of the original
//   work. You may not use this software to directly or indirectly cause harm to others. This software is provided as-is and without
//   warrantee -- Use at your own risk. For more information, visit HTTP://www.FluidStudios.com/
//
// Copyright 2001, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_FSTL_LIST
#define _FSTL_LIST

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"

FSTL_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T, unsigned int G = 2>
class	list
{
public:
	class	node
	{
	friend	list<T,G>;
	public:
		// Construction/Destruction
	
	inline			node() : _next(static_cast<node *>(0)), _prev(static_cast<node *>(0)) {}
	inline			node(const T & data) : _next(static_cast<node *>(0)), _prev(static_cast<node *>(0)), _data(data) {}
	inline			~node() {}
	
		// Implementation
	
	inline		void	remove()
				{
					if (prev()) prev()->_next = next();
					if (next()) next()->_prev = prev();
				}
	
	inline		void	insertBefore(node * n)
				{
					_prev = n->prev();
					_next = n;
					n->_prev = this;
					if (prev()) prev()->_next = this;
				}
	
	inline		void	insertAfter(node * n)
				{
					_prev = n;
					_next = n->next();
					n->_next = this;
					if (next()) next()->_prev = this;
				}
	
		// Accessors
	
	inline		node *	next() const {return _next;}
	inline		node *	prev() const {return _prev;}
	inline	const	T &	data() const {return _data;}
	inline		T &	data()       {return _data;}
	
	private:
		// Data members
	
			node *	_next;
			node *	_prev;
			T	_data;
	
		// Explicitly disallowed calls (they appear here, because if we don't do this, the compiler will generate them for us)
	
				node(const node & rhs);
	inline		node &	operator =(const node & rhs);
	};

	// Construction/Destruction

inline				list()
				{
					setzero();
				}

inline				list(const list &rhs)
				{
					setzero();
					*this = rhs;
				}

inline				~list()
				{
					erase();
					compact();
				}

	// Operators

				// The infamous operator=()

inline		list &		operator =(const list &rhs)
				{
					if (this == &rhs) return *this;

					// Wipe myself out

					erase();
					compact();

					// Allocate enough room for the new list

					reserve(rhs.size());
					
					// Insert the list into myself

					insert(rhs);

					return *this;
				}

				// Concat two lists
				
inline		list		operator +(const list& rhs)
				{
					list	result = *this;
					return result += rhs;
				}

				// Append a list onto the end of the list
				
inline		list &		operator +=(const list& rhs)
				{
					insert(rhs);
					return *this;
				}

				// Add an element to the end
				
inline		list		operator +(const T& rhs)
				{
					list	result = *this;
					return result += rhs;
				}

				// Append an element onto the end of the list
				
inline		list &		operator +=(const T& rhs)
				{
					insert(rhs);
					return *this;
				}

				// Invert the order of all elements in the list
				
inline		void		invert()
				{
					node *	h = head();
					node *	t = tail();
					unsigned int	c = size();
					while(h && t && c > 1)
					{
						swap(h->data(), t->data());
						h = h->next();
						t = t->prev();
						c -= 2;
					}
				}

				// Subset of the list
				
inline		list		operator ()(const unsigned int start, unsigned int count) const
				{
					list		result;
					const node *	n = (*this)[start];
					while(n && count)
					{
						result += n->data();
						--count;
					}
					return result;
				}

				// Index into the list
				
inline		node *		operator [](unsigned int rhs)
				{
					node *	ptr = head();
					while(ptr && rhs)
					{
						ptr = ptr->next();
						--rhs;
					}

					return ptr;
				}

inline	const	node *		operator [](unsigned int rhs) const
				{
					node *	ptr = head();
					while(ptr && rhs)
					{
						ptr = ptr->next();
						--rhs;
					}

					return ptr;
				}

				// Compare if two lists are equal
				
inline		bool		operator ==(const list& rhs) const
				{
					if (size() != rhs.size()) return false;
					if (size())
					{
						const node *	src1 = rhs.head();
						const node *	src2 = head();
						while(src1 && src2)
						{
							if (src1->data() != src2->data()) return false;
							src1 = src1->next();
							src2 = src2->next();
						}
					}

					return true;
				}

				// Compare if two lists are not equal
				
inline		bool		operator !=(const list& rhs) const
				{
					return !(operator==(rhs));
				}

	// Implementation

				// Indexed lookup

inline		node *		at(unsigned int index)
				{
					return (*this)[index];
				}

				// Indexed lookup

inline	const	node *		at(unsigned int index) const
				{
					return (*this)[index];
				}

				// Swap two elements

inline		void		swap(node* lhs, node* rhs)
				{
					if (lhs.next()) lhs->_next->_prev = rhs;
					if (lhs.prev()) lhs->_prev->_next = rhs;
					if (rhs.next()) rhs->_next->_prev = lhs;
					if (rhs.prev()) rhs->_prev->_next = lhs;
					fstl::swap(lhs._next, rhs._next);
					fstl::swap(lhs._prev, rhs._prev);
				}

				// Sort the list -- This just a bubble sort, so use with discretion

inline		void		sort()
				{
					if (!size()) return;
					for (unsigned int i = 0; i < size() - 1; ++i)
					{
						for (unsigned int j = i+1; j < size(); ++j)
						{
							if (at(i) > at(j)) swap(i, j);
						}
					}
				}

				// Sort the list [reverse] -- This just a bubble sort, so use with discretion

inline		void		rsort()
				{
					if (!size()) return;
					for (unsigned int i = 0; i < size() - 1; ++i)
					{
						for (unsigned int j = i+1; j < size(); ++j)
						{
							if (at(i) < at(j)) swap(i, j);
						}
					}
				}

				// Remove neighboring duplicates

inline		void		unique()
				{
					if (!size()) return;
					for (unsigned int i = 0; i < size(); ++i)
					{
						for (unsigned int j = i+1; j < size(); ++j)
						{
							if (at(i) == at(j))
							{
								erase(j, 1);
								j--;
							}
							else
							{
								break;
							}
						}
					}
				}

				// Erase an element or group of elements

inline		void		erase(node* start = static_cast<node *>(0), unsigned int count = 0xffffffff)
				{
					// Clamp the start

					if (!start) start = _head;

					// Erase the elements

					while(start && count && size())
					{
						// Correct for removing the head/tail

						if (start == head()) _head = head()->next();
						if (start == tail()) _tail = tail()->prev();

						// Remove it from the list

						node *	next = start->next();
						start->remove();

						// Destruct this element

						destruct(start);

						// Add it to the free list

						if (freeList) start->_next = freeList;
						freeList = start;

						// Next!

						start = next;
						--count;
						--_size;
					}
				}

				// Insert an element

inline		node *		insert(const T& el, node* start = static_cast<node *>(0))
				{
					// Make room if we need to

					if (extraReserved() < 1) reserve(size() + granularity());

					// Pluck one from the free list

					node *	newNode = freeList;
					freeList = newNode->next();

					// Construct it in place (i.e. 'new placement')

					construct(newNode, el);

					// Append it to the end?

					if (!start)
					{
						if (_tail)
						{
							newNode->insertAfter(_tail);
							_tail = newNode;
						}
						else
						{
							_head = _tail = newNode;
						}
					}
					else
					{
						newNode->insertBefore(start);
						if (head() == start) _head = newNode;
					}

					++_size;
					return newNode;
				}

				// Insert a list

inline		node *		insert(const list& l, node* start = static_cast<node *>(0))
				{
					node *	ptr = l.head();
					node *	first = start;
					while(ptr)
					{
						first = insert(ptr->data(), first)->next();
						ptr = ptr->next();
					}
					return first;
				}

				// Find

inline		node *		find(const T& element, node* start = static_cast<node *>(0)) const
				{
					if (!start) start = head();
					while(start)
					{
						if (start->data() == element) return start;
						start = start->next();
					}
					return static_cast<node *>(0);
				}

				// Reverse find

inline		node *		rfind(const T& element, node* start = static_cast<node *>(0)) const
				{
					if (!start) start = tail();
					while(start)
					{
						if (start->data() == element) return start;
						start = start->prev();
					}
					return static_cast<node *>(0);
				}

				// Fill a region of the list with a fill element

inline		void		fill(const T& filler, node* start = static_cast<node *>(0), unsigned int count = 0xffffffff)
				{
					if (!start) start = head();
					while(start && count)
					{
						start->data() = filler;
						start = start->next();
						--count;
					}
				}

				// Populate a list with a bunch of default elements
				
inline		void		populate(const T& filler, unsigned int count)
				{
					reserve(size() + count);
					for (unsigned int i = 0; i < count; ++i)
					{
						*this += filler;
					}
				}

				// Removes unused (but reserved) elements at the end of the list, including any leftover granularity

inline		void		compact()
				{
					// Don't do this if we don't need to

					if (!reservoirList) return;

					// Create a new, temporary list

					list	temp;
					temp.reserve(size());

					// Insert all of my entries into the new list

					const node *	ptr = head();
					while(ptr)
					{
						temp.insert(ptr->data());
						ptr = ptr->next();
					}

					// Erase everthing from my own list

					cleanup();

					// Copy over the data from the temporary list

					_head = temp._head;
					_tail = temp._tail;
					_size = temp._size;
					_reserved = temp._reserved;
					freeList = temp.freeList;
					reservoirCount = temp.reservoirCount;
					reservoirList = temp.reservoirList;

					// Zero out 'temp' (this prevents its destructor from doing anything terrible to the
					// memory we just stole from it)

					temp.setzero();
				}

				// Grow the reserved memory until it reaches 'len' list elements

inline		void		reserve(const unsigned int len)
				{
					// NOP if they're asking to shrink the list

					if (len <= reserved()) return;

					// Allocate another reservoir for the extra they asked for

					addReservoir(len - reserved());
				}

	// Accessors

inline		node *		head() const {return _head;}
inline		node *		tail() const {return _tail;}
inline	const	unsigned int	size() const {return _size;}
inline	const	unsigned int	reserved() const {return _reserved;}
inline	const	unsigned int	extraReserved() const {return reserved() - size();}
inline	const	unsigned int	granularity() const {return G;}

private:
	// List management data members

		node *		_head;
		node *		_tail;
		unsigned int	_size;
		unsigned int	_reserved;

	// Node reservoir data members

		node *		freeList;
		unsigned int	reservoirCount;
		node * *	reservoirList;

	// Implementation (private)

inline		void		cleanup()
				{
					// Destruct all of the used nodes

					node *	ptr = _head;
					while(ptr)
					{
						node *	next = ptr->next();
						destruct(ptr);
						ptr = next;
					}

					// Deallocate all of the reservoirs

					for (unsigned int i = 0; i < reservoirCount; ++i)
					{
						deallocate(reservoirList[i]);
					}

					// Free the reservoir list

					deallocate(reservoirList);

					// Reset to a known state

					setzero();
				}

inline		void		setzero()
				{
					_head = static_cast<node *>(0);
					_tail = static_cast<node *>(0);
					_size = 0;
					_reserved = 0;
					freeList = static_cast<node *>(0);
					reservoirList = static_cast<node * *>(0);
					reservoirCount = 0;
				}

inline		void		addReservoir(unsigned int gran = 0)
				{
					// How many elements for the reservoir?

					if (!gran) gran = granularity();

					// Allocate a new reservoir

					node *	newReservoir = allocate<node >(gran);

					// Grow the size of the reservoir list

					node * *	newReservoirList = allocate<node *>(reservoirCount+1);

					// Move the list over

					moveElements(newReservoirList, reservoirList, reservoirCount);
					deallocate(reservoirList);

					// Append the new reservoir to the end of the reservoirList

					reservoirList = newReservoirList;
					reservoirList[reservoirCount] = newReservoir;

					// Link the new reservoir into a contiguous list for the free list
					// Note that the free list doesn't require a doubly-linked list, so we'll
					// only track the 'next' pointers, not the 'prev' pointers.

					for (unsigned int i = 0; i < gran - 1; ++i)
					{
						newReservoir[i]._next = &newReservoir[i+1];
					}

					// Add the new reservoir to the free list

					newReservoir[gran - 1]._next = freeList;
					freeList = newReservoir;

					// Increment our counts

					++reservoirCount;
					_reserved += gran;
				}
};

// ---------------------------------------------------------------------------------------------------------------------------------
// These are handy...
// ---------------------------------------------------------------------------------------------------------------------------------

typedef	list<bool>		boolList;
typedef	list<int>		intList;
typedef	list<unsigned int>	uintList;
typedef	list<char>		charList;
typedef	list<unsigned char>	ucharList;
typedef	list<short>		shortList;
typedef	list<unsigned short>	ushortList;
typedef	list<float>		floatList;
typedef	list<double>		doubleList;

FSTL_NAMESPACE_END
#endif // _FSTL_LIST
// ---------------------------------------------------------------------------------------------------------------------------------
// list - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

