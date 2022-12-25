// ---------------------------------------------------------------------------------------------------------------------------------
//  _               _
// | |             | |
// | |__   __ _ ___| |__
// | '_ \ / _` / __| '_ \
// | | | | (_| \__ \ | | |
// |_| |_|\__,_|___/_| |_|
//
// Description:
//
//   Generic hash table
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

#ifndef	_FSTL_HASH
#define _FSTL_HASH

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "list.h"
#include "array.h"

FSTL_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template<class T, unsigned int S = 1024, unsigned int G = 2>
class	hash
{
private:
	class	node
	{
	public:
	inline				node() : _id(0) {}
	inline				~node() {}
	inline	const	unsigned int	id() const {return _id;}
	inline  	unsigned int &	id() {return _id;}
	inline  const	T &		data() const {return _data;}
	inline  	T &		data() {return _data;}
	
	private:
			unsigned int	_id;
			T		_data;
	};
public:
	// Construction/Destruction

inline	        		hash()
				: _size(0)
				{
					erase();
				}

inline	        		hash(hash & rhs)
				: _size(0)
				{
					*this = rhs;
				}

virtual				~hash() {}

	// Operators

inline		hash &		operator =(const hash & h)
				{
					if (this == &h) return *this;
					
					// This is simple...
					
					_table = h._table;
					_size = h._size;

					return *this;
				}

inline		T &		operator [](const unsigned int id)
				{
					return get(id);
				}

	// Implementation

inline		void		erase()
				{
					// Erase anything in the table

					_table.erase();

					// Set the reserve

					_table.reserve(S);

					// Populate the table with "blanks"

					list<node,G>	temp;
					for (unsigned int i = 0; i < hashSize(); ++i)
					{
						_table.insert(temp);
					}

					// Size is now zero

					_size = 0;
				}

inline		T &		get(const unsigned int id)
				{
					// Go find this node in the hash
					
					list<node,G> &		l = locateList(id);
					list<node,G>::node *	n = locateNode(l, id);
					
					// Found?

					if (n) return n->data().data();
					
					// Not found, insert one

					++_size;
					node	temp;
					temp.id() = id;
					return l.insert(temp)->data().data();
				}
				
inline		bool		remove(const unsigned int id)
				{
					// Go find this node in the hash
					
					list<node,G> &		l = locateList(id);
					list<node,G>::node *	n = locateNode(l, id);

					// Not found?

					if (n) return false;
					
					// Remove it

					--_size;
					l.erase(n);
					return true;
				}

inline		bool		exist(const unsigned int id)
				{
					list<node,G> &		l = locateList(id);
					list<node,G>::node *	n = locateNode(l, id);
					return n ? true:false;
				}

	// Accessors

inline		unsigned int	size() const {return _size;}
inline		unsigned int	hashSize() const {return S;}

private:
	// Utilitarian (private)

inline		unsigned int	hashIndex(const unsigned int id) const
				{
					return id % hashSize();
				}

inline		list<node,G> &	locateList(const unsigned int id)
				{
					// Using the index, lookup the hash table entry list

					return _table[hashIndex(id)];
				}
				
inline		list<node,G> * locateNode(list<node,G> & l, const unsigned int id)
				{
					// Scan the given list for a node with the given id

					list<node,G>::node * ptr = l.head();
					while(ptr)
					{
						if (ptr->data().id() == id) return ptr;
						ptr = ptr->next();
					}

					// Not found, return null

					return static_cast<list<node, G>::node *>(0);
				}
	// The hash table

		unsigned int		_size;
		array<list<node,G> >	_table;
};

FSTL_NAMESPACE_END
#endif // _FSTL_HASH
// ---------------------------------------------------------------------------------------------------------------------------------
// hash - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
