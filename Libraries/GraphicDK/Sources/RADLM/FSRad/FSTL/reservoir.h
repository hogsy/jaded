// ---------------------------------------------------------------------------------------------------------------------------------
//                                      _
//                                     (_)
//  _ __  ___ ___  ___ _ ____   __ ___  _ _ __
// | '__|/ _ \ __|/ _ \ '__\ \ / // _ \| | '__|
// | |  |  __/__ \  __/ |   \ V /| (_) | | |
// |_|   \___|___/\___|_|    \_/  \___/|_|_|
//
//
//
// Description:
//
//   Memory pool (reservoir) manager
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

#ifndef	_FSTL_RESERVOIR
#define _FSTL_RESERVOIR

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"

FSTL_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T, unsigned int G = 8>
class	reservoir
{
public:
	// Construction/destruction

inline				reservoir()
				{
					setzero();
				}

inline				reservoir(reservoir &rhs)
				{
					setzero();
					*this = rhs;
				}

inline				~reservoir()
				{
					reset();
				}

	// Operators
	
inline		reservoir &	operator =(const reservoir& rhs)
				{
					if (this == &rhs) return *this;
					
					// Wipe myself out (if necessary)
					
					reset();
					
					// Use a temporary object to make things simpler
					
					reservoir	temp;
					
					// Visit each node from 'rhs' and populate 'temp'
					
					node<G> *	ptr = rhs._nodes;
					while(ptr)
					{
						// Copy over the elements from rhs to temp
						
						for (unsigned int i = 0; i < ptr->usedElements; ++i)
						{
							temp.get(ptr->elements[i]);
						}
						ptr = ptr->next;
					}
					
					// Move the data over to 'this'
					
					_size = temp._size;
					_reserved = temp._reserved;
					_currentNode = temp._currentNode;
					_nodes = temp._nodes;
					
					// Zero out 'temp' (this prevents its destructor from doing anything terrible to the
					// memory we just stole from it)
					
					temp.setzero();
					
					// Done

					return *this;
				}

	// Implementation

inline		T &		get(const T & src)
				{
					T *	newElement = getElement();
					
					// Construct the sucker
					
					construct(newElement, src);
					
					return *newElement;
				}

inline		T &		get()
				{
					T *	newElement = getElement();
					
					// Construct the sucker
					
					construct(newElement);
					
					return *newElement;
				}

inline		void		reset()
				{
					// Visit each node
					
					node<G> *	ptr = _nodes;
					while(ptr)
					{
						// Destruct all the elements in this node
						
						for (unsigned int i = 0; i < ptr->usedElements; ++i)
						{
							destruct(&ptr->elements[i]);
						}
						node<G> *	next = ptr->next;
						deallocate(ptr);
						ptr = next;
					}
					
					setzero();
				}

	// Accessors

inline	const	unsigned int	size() const {return _size;}
inline	const	unsigned int	reservred() const {return _reserved;}
inline	const	unsigned int	granularity() const {return G;}

private:
	template <unsigned int G>
	struct node
	{
		T		elements[G];
		unsigned int	usedElements;
		node *		next;
	};
	
	// Implementation (private)
	
				// Generic way to get a new node without any construction
	
inline		T *		getElement()
				{
					// Need to grow the reservoir?
					
					if (!_nodes || _currentNode->usedElements == granularity()) allocNode();
					
					// Grab an element from the current node
					
					T *	el = &_currentNode->elements[_currentNode->usedElements];
					++_currentNode->usedElements;
					
					return el;
				}

inline		void		allocNode()
				{
					// Allocate a new node for the reservoir list
					
					node<G>	*newnode = allocate<node<G> >(1);
					
					// Init the new node
					
					newnode->usedElements = 0;

					// Add it to the node list
					
					newnode->next = _nodes;
					_nodes = newnode;
					_currentNode = newnode;

					// Update the reserved count
					
					_reserved += granularity();
				}

inline		void		setzero()
				{
					_size = 0;
					_reserved = 0;
					_currentNode = static_cast<node<G> *>(0);
					_nodes = static_cast<node<G> *>(0);
				}
	// Data

		unsigned int	_size;
		unsigned int	_reserved;
		node<G> *	_currentNode;
		node<G> *	_nodes;
		
};

FSTL_NAMESPACE_END
#endif // _FSTL_RESERVOIR
// ---------------------------------------------------------------------------------------------------------------------------------
// reservoir - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

