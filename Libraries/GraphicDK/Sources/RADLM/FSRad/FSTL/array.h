// ---------------------------------------------------------------------------------------------------------------------------------
//   __ _ _ __ _ __  __ _ _   _
//  / _` | '__| '__|/ _` | | | |
// | (_| | |  | |  | (_| | |_| |
//  \__,_|_|  |_|   \__,_|\__, |
//                         __/ |
//                        |___/
//
// Description:
//
//   Dynamic array
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

#ifndef	_FSTL_ARRAY
#define _FSTL_ARRAY

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "util.h"

FSTL_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template<class T, unsigned int G = 2>
class	array
{
public:
	// Construction/Destruction

inline				array()
				:_buf(static_cast<T *>(0)), _size(0), _reserved(0)
				{
				}

inline				array(const array &ar)
				:_buf(static_cast<T *>(0)), _size(0), _reserved(0)
				{
					(*this) = ar;
				}

inline				~array()
				{
					erase();
					compact();
				}

	// Operators

				// The infamous operator=()
				
inline		array &		operator =(const array& rhs)
				{
					if (this == &rhs) return *this;

					// Wipe myself out

					erase();
					compact();
					
					// Allocate a whole new list large enough for 'rhs' plus the granularity
					
					_size = rhs.size();
					_reserved = size() + granularity();
					_buf = allocate<T>(reserved());

					// Copy everything over, using the copy ctor

					copyElements(_buf, rhs._buf, size());

					return *this;
				}

				// Concat two arrays
				
inline		array		operator +(const array& rhs)
				{
					array	result = *this;
					return result += rhs;
				}

				// Append an array onto the end of the array
				
inline		array &		operator +=(const array& rhs)
				{
					insert(rhs);
					return *this;
				}

				// Add an element to the end
				
inline		array		operator +(const T& rhs)
				{
					array	result = *this;
					return result += rhs;
				}

				// Append an element onto the end of the array
				
inline		array &		operator +=(const T& rhs)
				{
					insert(rhs);
					return *this;
				}

				// Invert the order of all elements in the array
				
inline		void		invert()
				{
					invertElements(_buf, size());
				}

				// Subset of the array
				
inline		array		operator ()(const unsigned int start, const unsigned int count) const
				{
					array	result;

					// Clamp the count and put it into the new array's size

					result._size = clampCount(start, count);
                                        if (!result._size) return result;
					
					// Allocate the resulting list
					
					result._reserved = result.size() + granularity();
					result._buf = allocate<T>(result.reserved());
					
					// Copy everything over, using the copy ctor
					
					copyElements(result._buf, _buf + start, count);
					
					return result;
				}

				// Index into the array
				
		T &		operator [](const int index)
				{
					return _buf[index];
				}

	const	T &		operator [](const int index) const
				{
					return _buf[index];
				}

				// Compare if two arrays are equal
				
inline		bool		operator ==(const array& rhs) const
				{
					if (!_buf) return _buf == rhs._buf;
					if (size() != rhs.size()) return false;
					for (unsigned int i = 0; i < size(); ++i)
					{
						if (!(_buf[i] == rhs._buf[i])) return false;
					}
					return true;
				}

				// Compare if two arrays are not equal
				
inline		bool		operator !=(const array& rhs) const
				{
					return !(operator==(rhs));
				}

	// Implementation

				// Indexed lookup

inline		T &		at(const unsigned int index)
				{
					return (*this)[index];
				}

inline	const	T &		at(const unsigned int index) const
				{
					return (*this)[index];
				}

				// Swap two elements

inline		void		swap(const unsigned int lhs, const unsigned int rhs)
				{
					fstl::swap(at(lhs), at(rhs));
				}

				// Sort the array -- This just a bubble sort, so use with discretion

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

				// Sort the array [reverse] -- This just a bubble sort, so use with discretion

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
				
inline		void		erase(const unsigned int start = 0, unsigned int count = 0xffffffff)
				{
					// Clamp the count to within range

					count = clampCount(start, count);
					if (!count) return;
					
					// 'Shift' the array down
					
					moveElements(&_buf[start], &_buf[start+count], size() - (start + count));

					// Destruct the dangling elements

					destructElements(&_buf[size()-count], count);
					
					// Adjust our size
					
					_size -= count;
				}

				// Insert an element
				
inline		void		insert(const T& el, unsigned int start = 0xffffffff)
				{
					// Our start should point at the element where the new entry goes. Any existing element
					// (and all elements following that one) will get shifted out of the way
					
					if (start > size()) start = size();

					// Do we have enough room for the new element?
					
					if (reserved() >= size() + 1)
					{
						// This is actually a little trickier than it seems, so read this:
						//
						// Given an array [a, c, d, e, f, g] if we want to insert 'b' so that the
						// resulting array is [a, b, c, d, e, f, g] then we need to move elements 'c' thru
						// 'g' forward to make room for 'b'. Sound simple? Nope.
						//
						// Remember that our array specifically does not construct any of the elements
						// that are not in use. And if an element is removed, we shift the array down
						// and destruct the one that's left dangling on the end. So it's safe to assume
						// that if an array element is unused, it is also not constructed.
						//
						// Given that, what happens when we shift elements 'c' through 'g' forward? Well,
						// this works except that when we move 'g' forward by one, it ends up occupying
						// a non-constructed piece of memory (past the end of the initial array). So in that
						// case, we need to copy-construct 'g' into its new home. However, moving elements
						// 'c' through 'f' forward can be done with the operator=(), because those elements
						// actually exist and have been constructed.
						//
						// That's not all.. just to make things more confusing, if the new element is
						// being inserted past the end, none of this matters, we simply copy-construct
						// the new element into the array, just past the end (into the buffer zone).
						//
						// However, if the new element is being inserted into the array somplace in
						// the middle, we'll have to use the operator=() on it, because it has already
						// been constructed.
						//
						// Hopefully that's not too confusing, because you've reached the end of my
						// explanation. :)

						// Inserting at the end?
						
						if (start == size())
						{
							// Construct the new element into "uncharted territory"
							
							construct(&_buf[start], el);
						}

						// Need to shift stuff around						

						else
						{
							// Copy/construct the last element to be moved
							
							construct(&_buf[size()], _buf[size()-1]);
							
							// Move everything else forward by one
							
							moveElements(&_buf[start+1], &_buf[start], size() - start - 1);
							
							// Move the new element into place
							
							_buf[start] = el;
						}
						
						// Use the copy constructor to put this entry into the list
						
						++_size;
						return;
					}
					
					// We need to grow the array
					
					unsigned int	newSize = size() + 1;
					unsigned int	newReserved = newSize + granularity();
					T*	newBuf = allocate<T>(newReserved);
					copyElements(newBuf, _buf, start);
					construct(&newBuf[start], el);
					copyElements(newBuf+start+1, _buf+start, size() - start);
					
					// Out with the old...
					
					erase();
					compact();
					
					// ...in with the new
					
					_buf = newBuf;
					_size = newSize;
					_reserved = newReserved;
				}

				// Insert an array
				
inline		void		insert(const array& ar, unsigned int start = 0xffffffff)
				{
					// Our start should point at the element where the new entry goes. Any existing element
					// (and all elements following that one) will get shifted out of the way
					
					if (start > size()) start = size();

					// Do we have enough room for the new array?
					
					if (reserved() >= size() + ar.size())
					{
						// Look at the insert() routine (above) for insight as to what might be going
						// on here... then consider that we're inserting a list and also need to deal
						// with the inserted array inserting past the end of the initial array... which
						// confuses things even more... ASCII diagrams below should help...

						// Inserting at the end?
						
						if (start == size())
						{
							// Construct the new element into "uncharted territory"
						
							copyElements(_buf+start, ar._buf, ar.size());	
						}

						// Need to shift stuff around						

						else
						{
							// Case 1:
							//
							//     ABCDEF
							//     V
							//    abcdefghijklm
							//    |___________|...
							//
							// results in:
							//
							//    aABCDEFbcdefghijklm
							//    |___________|...
							//
							// Three steps to do it. They are:
							//
							//    abcdefg      hijklm
							//    |___________|...
							//
							//    a      bcdefghijklm
							//    |___________|...
							//
							//    aABCDEFbcdefghijklm
							//    |___________|...
							
							if (start+ar.size() < size())
							{
								copyElements(_buf+size(), _buf+size()-ar.size(), ar.size());
								moveElements(_buf+start+ar.size(), _buf+start, ar.size());
								moveElements(_buf+start, ar._buf, ar.size());
							}

							// Case 2:
							//
							//             ABCDEF
							//             V
							//    abcdefghijklm
							//    |___________|...
							//
							// results in:
							//
							//    abcdefghiABCDEFjklm
							//    |___________|...
							//
							// Three steps to do it. They are:
							//
							//    abcdefghi      jklm
							//    |___________|...
							//
							//    abcdefghi    EFjklm
							//    |___________|...
							//
							//    abcdefghiABCDEFjklm
							//    |___________|...
							
							else
							{
								unsigned int	 half1 = size() - start;
								unsigned int	 half2 = ar.size() - half1;
								copyElements(_buf+start+ar.size(), _buf+start, half1);
								copyElements(_buf+size(), ar._buf+half1, half2);
								moveElements(_buf+start, ar._buf, half1);
							}
						}
						
						_size += ar.size();
						return;
					}
					
					// We need to grow the array
					
					unsigned int	newSize = size() + ar.size();
					unsigned int	newReserved = newSize + granularity();
					T*	newBuf = allocate<T>(newReserved);
					copyElements(newBuf, _buf, start);
					copyElements(newBuf+start, ar._buf, ar.size());
					copyElements(newBuf+start+ar.size(), _buf+start, size() - start);
					
					// Out with the old...
					
					erase();
					compact();
					
					// ...in with the new
					
					_buf = newBuf;
					_size = newSize;
					_reserved = newReserved;
				}

				// Find
				
inline		int		find(const T& element, const unsigned int start = 0) const
				{
					for (unsigned int i = start; i < size(); ++i)
					{
						if (_buf[i] == element) return i;
					}

					return -1;
				}

				// Reverse find
				
inline		int		rfind(const T& element, unsigned int start = 0xffffffff) const
				{
					if (start >= size()) start = size() - 1;

					for (unsigned int i = start; i >= 0; --i)
					{
						if (_buf[i] == element) return i;
					}

					return -1;
				}

				// Fill a region of the array with a fill element
				
inline		void		fill(const T& filler, unsigned int start = 0, unsigned int count = 0xffffffff)
				{
					if (start >= size()) return;
					fillElements(&_buf[start], filler, clampCount(start, count));
				}

				// Populate an array with a bunch of default elements
				
inline		void		populate(const T& filler, unsigned int count)
				{
					reserve(size() + count);
					for (unsigned int i = 0; i < count; ++i)
					{
						*this += filler;
					}
				}

	// Accessors

				// Removes unused (but reserved) elements at the end of the array, including any leftover granularity

inline		void		compact()
				{
					// Is the array empty?
					
					if (!size())
					{
						deallocate(_buf);
						_buf = static_cast<T*>(0);
						_reserved = 0;
						return;
					}
					
					// Our array has some stuff in it, so reallocate a new array just big enough for size() elements

					T*	newBuf = allocate<T>(size());
					
					// Copy everything over, using the copy ctor

					copyElements(newBuf, _buf, size());
					
					// Destruct the elements in our current array
					
					destructElements(_buf, size());
					deallocate(_buf);
					
					// New buffer
					
					_buf = newBuf;
					_reserved = size();
				}

				// Reallocs the array, adding 'len' entries to the array's allocation pool
				
inline		void		reserve(const unsigned int len)
				{
					// NOP if they're asking to shrink the array
					
					if (len <= reserved()) return;
					
					// Allocate a new array
					
					T*	newBuf = allocate<T>(len);
					
					// Copy everything over, using the copy ctor
					
					copyElements(newBuf, _buf, size());
					
					// Destruct the elements in our current array
					
					destructElements(_buf, size());
					deallocate(_buf);
					_buf = newBuf;
					_reserved = len;
				}

	// Accessors

inline	const	unsigned int	size() const {return _size;}
inline	const	unsigned int	reserved() const {return _reserved;}
inline	const	unsigned int	extraReserved() const {return reserved() - size();}
inline	const	unsigned int	granularity() const {return G;}

private:

inline		unsigned int	clampCount(const int unsigned start, unsigned int count) const
				{
					// If we're starting past the end, count should be 0
					
					if (start >= size()) return 0;
					
					// If count extends past end, clamp it
					
					if (start + count > size()) count = size() - start;
					
					// Return count
					
					return count;
				}

	// Data members

		T *		_buf;
		unsigned int	_size;
		unsigned int	_reserved;
};

// ---------------------------------------------------------------------------------------------------------------------------------
// These are handy...
// ---------------------------------------------------------------------------------------------------------------------------------

typedef	array<bool>		boolArray;
typedef	array<int>		intArray;
typedef	array<unsigned int>	uintArray;
typedef	array<char>		charArray;
typedef	array<unsigned char>	ucharArray;
typedef	array<short>		shortArray;
typedef	array<unsigned short>	ushortArray;
typedef	array<float>		floatArray;
typedef	array<double>		doubleArray;

FSTL_NAMESPACE_END
#endif // _FSTL_ARRAY
// ---------------------------------------------------------------------------------------------------------------------------------
// array - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
