// ---------------------------------------------------------------------------------------------------------------------------------
//      _        _
//     | |      (_)
//  ___| |_ _ __ _ _ __   __ _
// / __| __| '__| | '_ \ / _` |
// \__ \ |_| |  | | | | | (_| |
// |___/\__|_|  |_|_| |_|\__, |
//                        __/ |
//                       |___/
//
// Description:
//
//   Character string class
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

#ifndef	_FSTL_STRING
#define _FSTL_STRING

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "common.h"
#include "util.h"
#include "array.h"
#include "list.h"

FSTL_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T = char>
class	basic_string
{
public:
	// Construction/destruction

inline				basic_string()
				: _length(0), _buffer(static_cast<T *>(0))
				{
				}

inline				basic_string(const basic_string & str)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					*this = str;
				}

inline				basic_string(const char * str)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					resize(static_cast<const unsigned int>(strlen(str)));

					if (_buffer)
					{
						_buffer[length()] = 0;
						fstl::memcpy(_buffer, str, length());
					}
				}

inline				basic_string(const wchar_t * str)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					int	len = WideCharToMultiByte(CP_ACP, 0, str, -1, 0, 0, NULL, NULL);
					resize(len);

					if (_buffer)
					{
						_buffer[length()] = 0;
						WideCharToMultiByte(CP_ACP, 0, str, -1, (LPSTR)_buffer, length(), NULL, NULL);
					}
				}

inline	explicit		basic_string(const T value)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					resize(1);
					buffer()[0] = value;
				}

inline	explicit		basic_string(const int value)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					char	s[50];
					sprintf(s, "%d", value);
					*this = basic_string(s);
				}

inline	explicit		basic_string(const unsigned int value)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					char	s[50];
					sprintf(s, "%u", value);
					*this = basic_string(s);
				}

inline	explicit		basic_string(const long value)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					char	s[50];
					sprintf(s, "%d", value);
					*this = basic_string(s);
				}

inline	explicit		basic_string(const unsigned long value)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					char	s[50];
					sprintf(s, "%u", value);
					*this = basic_string(s);
				}

inline	explicit		basic_string(const float value)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					char	s[50];
					sprintf(s, "%f", value);
					*this = basic_string(s);
				}

inline	explicit		basic_string(const double value)
				: _length(0), _buffer(static_cast<T *>(0))
				{
					char	s[50];
					sprintf(s, "%f", value);
					*this = basic_string(s);
				}

inline				~basic_string()
				{
					erase();
				}

	// Casting & conversion

inline	const	T *		asArray() const		{return buffer();}
inline		char		asChar() const		{return static_cast<char>(asInt());}
inline		unsigned char	asUChar() const		{return static_cast<unsigned char>(asUInt());}
inline		short		asShort() const		{return static_cast<short>(asInt());}
inline		unsigned short	asUShort() const	{return static_cast<unsigned short>(asUInt());}
inline		unsigned int	asUInt() const		{return static_cast<unsigned int>(asInt());}
inline		unsigned long	asULong() const		{return static_cast<unsigned long>(asLong());}
inline		float		asFloat() const		{return static_cast<float>(asDouble());}
inline		int		asInt() const		{return fstl::atoi(buffer());}
inline		long		asLong() const		{return fstl::atol(buffer());}
inline		double		asDouble() const	{return fstl::atof(buffer());}

	// Operators

inline		basic_string &	operator  =(const basic_string & rhs)
				{
					if (this == &rhs) return *this;
					resize(rhs.length());
					if (!rhs.length()) return *this;
					fstl::memcpy(_buffer, rhs._buffer, length());
					_buffer[length()] = 0;
					return *this;
				}

inline		void		operator +=(const basic_string & rhs)
				{
					if (!rhs.length()) return;
					unsigned int	oldLength = length();
					unsigned int	sLength = rhs.length();
					resize(oldLength + sLength);
					fstl::memcpy(&_buffer[oldLength], rhs._buffer, sLength);
					_buffer[length()] = 0;
				}

inline		basic_string	operator  +(const basic_string & rhs) const
				{
					basic_string	result(*this);
					result += rhs;
					return result;
				}

inline		basic_string	operator - ()
				{
					if (!_buffer) return *this;

					T	*p0 = _buffer;
					T	*p1 = &_buffer[length()-1];

					while(p0 < p1)
					{
						T	t = *p0;
						*p0 = *p1;
						*p1 = t;
						p0++;
						p1--;
					}

					return *this;
				}

inline				operator *=(const int value)
				{
					*this = *this * value;
				}

inline		basic_string	operator  *(const int value) const
				{
					// Neg numbers are bad

					if (value < 0) return *this;

					// Something to work with...

					basic_string	result;

					// Make sure we're about to DO something

					if (!length() || !value) return result;

					result.resize(length() * value);

					T	*ptr = result._buffer;

					for (int i = 0; i < value; i++, ptr += length())
					{
						fstl::memcpy(ptr, _buffer, length());
					}

					result._buffer[result.length()] = 0;
					return result;
				}

inline		void		operator >>=(const int value)
				{
					if (value <= 0) return;
					resize(length() + value);
					fstl::memmove(&_buffer[value], _buffer, (length()-value));
					fstl::memset(_buffer, space_char<T>(), value);
					_buffer[length()] = 0;
				}

inline		void		operator <<=(const int value)
				{
					if (value <= 0) return;
					if (!_buffer) return;
					int	count = length() - value;
					if (count > 0) fstl::memmove(_buffer, &_buffer[value], count);
					resize(count);
				}

inline		basic_string	operator >>(const int value) const
				{
					basic_string	result(*this);
					if (value <= 0) return result;
					result >>= value;
					return result;
				}

inline		basic_string	operator <<(const int value) const
				{
					basic_string	result(*this);
					if (value <= 0) return result;
					result <<= value;
					return result;
				}

inline		bool		operator ==(const basic_string & str) const
				{
					if (!_buffer || !str._buffer) return _buffer == str._buffer;
					return fstl::strcmp(_buffer, str._buffer) == 0;
				}

inline		bool		operator !=(const basic_string & str) const
				{
					if (!_buffer || !str._buffer) return _buffer != str._buffer;
					return fstl::strcmp(_buffer, str._buffer) != 0;
				}

inline		bool		operator <=(const basic_string & str) const
				{
					if (!_buffer || !str._buffer) return _buffer == str._buffer;
					return fstl::strcmp(_buffer, str._buffer) <= 0;
				}

inline		bool		operator >=(const basic_string & str) const
				{
					if (!_buffer || !str._buffer) return _buffer == str._buffer;
					return fstl::strcmp(_buffer, str._buffer) >= 0;
				}

inline		bool		operator  <(const basic_string & str) const
				{
					if (!_buffer || !str._buffer) return _buffer == str._buffer;
					return fstl::strcmp(_buffer, str._buffer) < 0;
				}

inline		bool		operator  >(const basic_string & str) const
				{
					if (!_buffer || !str._buffer) return _buffer == str._buffer;
					return fstl::strcmp(_buffer, str._buffer) > 0;
				}

inline		T &		operator [](const int index) const
				{
					return _buffer[index];
				}

	// Utilitarian (public)

inline		basic_string	substring(int start, int count = 0x7fffffff) const
				{
					basic_string	result;
					if (start < 0)
					{
						count += start;
						start = 0;
					}
					else if (static_cast<unsigned int>(start) > length()) start = length();

					if (!_buffer || !length() || static_cast<unsigned int>(start) >= length()) return result;
					if (static_cast<unsigned int>(start + count) > length()+1) count = length() - start;

					result.set(&_buffer[start], count);
					return result;
				}

inline		void		erase(int start = 0, int count = 0x7fffffff)
				{
					if (static_cast<unsigned int>(start) >= length()) return;
					if (static_cast<unsigned int>(start + count) > length()) count = length() - start;

					fstl::memmove(&_buffer[start], &_buffer[start+count], (length() - count - start));
					resize(length() - count);
				}

inline		int		find(const basic_string & str, int start = 0) const
				{
					return find(str._buffer, start);
				}

inline		int		find(const T * str, int start = 0) const
				{
					if (!_buffer || !str) return -1;
					if (start < 0) start = 0;
					else if (static_cast<unsigned int>(start) > length()) return -1;

					T	*ptr = NULL;
					ptr = fstl::strstr(&_buffer[start], str);

					if (!ptr) return -1;
					return static_cast<int>(ptr - _buffer);
				}

inline		int		rfind(const basic_string & str, int start = 0x7fffffff) const
				{
					if (!_buffer || !str._buffer) return -1;
					if (static_cast<unsigned int>(start) > length()) start = length();
					// We'll back up by the length of the string since we can't find
					// a string in reverse order wihtout at least str's length at the end
					// of it...
					start -= str.length();

					while(start > 0)
					{
						if (!fstl::strncmp(&_buffer[start], str._buffer, str.length())) return start;
						start--;
					}

					return -1;
				}

inline		int		ncfind(const basic_string & str, int start = 0) const
				{
					if (!_buffer || !str._buffer) return -1;
					if (start < 0) start = 0;
					int	len = length() - str.length();
					if (len < 0) return -1;
					if (start > len) return -1;

					while(start <= len)
					{
						if (!fstl::strnicmp(&_buffer[start], str._buffer, str.length())) return start;
						start++;
					}

					return -1;
				}

inline		int		ncrfind(const basic_string & str, int start = 0x7fffffff) const
				{
					if (!_buffer || !str._buffer) return -1;
					if (static_cast<unsigned int>(start) > length()) start = length();
					// We'll back up by the length of the string since we can't find
					// a string in reverse order wihtout at least str's length at the end
					// of it...
					start -= str.length();

					while(start > 0)
					{
						if (!fstl::strnicmp(&_buffer[start], str._buffer, str.length())) return start;
						start--;
					}

					return -1;
				}

inline		int		ncCompare(const basic_string & str) const
				{
					if (!_buffer || !str._buffer) return _buffer == str._buffer ? 0:-1;
					return fstl::stricmp(_buffer, str._buffer);
				}

inline		int		ncCompare(const basic_string & str, unsigned int len) const
				{
					if (!_buffer || !str._buffer) return _buffer == str._buffer ? 0:-1;
					if (len > length()) len = length();
					return fstl::strnicmp(_buffer, str._buffer, len);
				}

inline		int		findFirstOf(const T *set, int start = 0) const
				{
					if (!_buffer || !set) return -1;
					if (start < 0) start = 0;
					else if (static_cast<unsigned int>(start) > length()) return -1;

					int	idx = static_cast<int>(fstl::strcspn(&_buffer[start], set));

					if (idx == length()) return -1;
					return start + idx;
				}

inline		int		findFirstNotOf(const T *set, int start = 0) const
				{
					if (!_buffer || !set) return -1;
					if (start < 0) start = 0;
					else if (static_cast<unsigned int>(start) > length()) return -1;

					int	idx = static_cast<int>(fstl::strspn(&_buffer[start], set));

					if (idx == length()) return -1;
					return start + idx;
				}

inline		int		findLastOf(const T *set, int start = 0x7fffffff) const
				{
					if (!_buffer || !set) return -1;
					if (start < 0) return -1;
					else if (static_cast<unsigned int>(start) >= length()) start = length() - 1;

					size_t	setLength = fstl::strlen(set);

					const	T *	ptr = &_buffer[start];
					while(ptr >= _buffer)
					{
						for (size_t i = 0; i < setLength; ++i)
						{
							if (*ptr == set[i])
							{
								return static_cast<int>(ptr - _buffer);
							}
						}

						--ptr;
					}

					// None mached, retrun the beginning of the string

					return 0;
				}

inline		int		findLastNotOf(const T *set, int start = 0x7fffffff) const
				{
					if (!_buffer || !set) return -1;
					if (start < 0) return -1;
					else if (static_cast<unsigned int>(start) >= length()) start = length() - 1;

					size_t	setLength = fstl::strlen(set);

					const	T *	ptr = &_buffer[start];
					while(ptr >= _buffer)
					{
						bool	found = false;
						for (size_t i = 0; i < setLength; ++i)
						{
							if (*ptr == set[i])
							{
								found = true;
								break;
							}
						}

						if (!found) return static_cast<int>(ptr - _buffer);
						--ptr;
					}

					// None mached, retrun the beginning of the string

					return 0;
				}

inline		basic_string	findWord(const int wordIndex, const T token = '|') const
				{
					// Searching backwards?

					if (wordIndex < 0) return rfindWord(-wordIndex, token);

					// Our result will go here

					basic_string	result;

					// No buffer, bail

					if (!_buffer || !length()) return result;

					// Find the start of the word

					int	index = wordIndex+1;
					T *	start = _buffer;
					while(--index && start)
					{
						start = fstl::strchr(start, token);
						if (start) start++;
					}

					// If the word wasn't in the string, then bail

					if (index || !start) return result;

					// Where does the word end?

					T *	end = fstl::strchr(start, token);
					if (!end) end = start + fstl::strlen(start);

					// Copy it into the new string

					result.set(start, static_cast<unsigned int>(end-start));

					// Return the result

					return result;
				}
				
inline		basic_string	rfindWord(const int wordIndex, const T token = '|') const
				{
					// Searching forwards?

					if (wordIndex < 0) return findWord(-wordIndex, token);

					// Our result will go here

					basic_string	result;

					// No buffer, bail

					if (!_buffer || !length()) return result;

					// Find the end of the word

					int	index = wordIndex;
					T *	end = &_buffer[length() - 1];
					while(end >= _buffer)
					{
						if (*end == token && !--index) break;
						end--;
					}

					// If the word wasn't in the string, then bail

					if (index) return result;

					// Where does the word start?

					T *	start = end - 1;
					while(start >= _buffer)
					{
						if (*start == token) break;
						start--;
					}
					if (start < _buffer) start = _buffer;

					// Copy it into the new string

					result.set(start, static_cast<unsigned int>(end-start));

					// Return the result

					return result;
				}

inline		void		fill(const T c = ' ')
				{
					if (!_buffer) return;
					fstl::memset(_buffer, c, length());
				}

inline		void		trimBeginning(const T * charList)
				{
					// We need SOME input

					if (!charList || !*charList || !_buffer) return;

					// How many characters to trim?

					unsigned int	count = static_cast<unsigned int>(fstl::strspn(_buffer, charList));

					// Trim none?

					if (count == 0) return;

					// Trim all?

					if (count == length())
					{
						erase();
						return;
					}

					// Trim some

					fstl::memmove(_buffer, &_buffer[count], (length() - count));
					resize(length() - count);
				}

inline		void		trimEnd(const T * charList)
				{
					// We need SOME input

					if (!charList || !*charList || !_buffer || !length()) return;

					// Find the last character in the string that is not of set charList

					T	*end = &_buffer[length()-1];
					while(end > _buffer && fstl::strchr(charList, *end)) end--;

					// Set the new string length

					resize(static_cast<unsigned int>(end-_buffer+1));
				}

inline		void		trim(const T * charList)
				{
					trimBeginning(charList);
					trimEnd(charList);
				}

inline		void		toupper()
				{
					if (!_buffer) return;
					T	*ptr = _buffer;
					for (unsigned int i = 0; i < length(); i++, ptr++) *ptr = static_cast<T>(fstl::toupper(*ptr));
				}

inline		void		tolower()
				{
					if (!_buffer) return;
					T	*ptr = _buffer;
					for (unsigned int i = 0; i < length(); i++, ptr++) *ptr = static_cast<T>(fstl::tolower(*ptr));
				}

	// Accessors

inline		unsigned int &	length()	{return _length;}
inline	const	unsigned int	length() const	{return _length;}

private:
	// Utilitarian (private)

inline		void		set(const T *str, const unsigned int len)
				{
					resize(len);
					if (!len) return;
					fstl::strncpy(_buffer, str, len);
					_buffer[length()] = 0;
				}

inline		void		resize(const unsigned int len)
				{
					// Do we need to resize?

					if (len == length()) return;

					// Are they clearing the buffer?

					if (!len)
					{
						deallocate(_buffer);
						_buffer = static_cast<T *>(0);
						_length = 0;
						return;
					}

					// Resize the buffer

					T	*temp = allocate<T>(len + 1);

					// Move the data over

					if (len > length())
					{
						fstl::memcpy(temp, _buffer, length());
						temp[length()] = 0;
					}
					else
					{
						fstl::memcpy(temp, _buffer, len);
						temp[len] = 0;
					}

					// Commit the changes

					deallocate(_buffer);
					_buffer = temp;
					length() = len;
				}

inline	const	T *		buffer() const
				{
					if (_buffer) return _buffer;
					return empty_string<T>();
				}

		// The string

		unsigned int	_length;
		T *		_buffer;
};

// ---------------------------------------------------------------------------------------------------------------------------------
// Convenience types - Most common uses
// ---------------------------------------------------------------------------------------------------------------------------------

typedef	basic_string<char>	string;
typedef	array<string>		StringArray;
typedef	list<string>		StringList;

typedef	basic_string<wchar_t>	wstring;
typedef	array<wstring>		WStringArray;
typedef	list<wstring>		WStringList;

// ---------------------------------------------------------------------------------------------------------------------------------
// Mixed-mode global overrides
// ---------------------------------------------------------------------------------------------------------------------------------

inline	string	operator +(const char * lhs, const string & rhs) {return fstl::string(lhs) + rhs;}
inline	wstring	operator +(const wchar_t * lhs, const wstring & rhs) {return fstl::wstring(lhs) + rhs;}

// ---------------------------------------------------------------------------------------------------------------------------------
// Specializations for wide char conversion
// ---------------------------------------------------------------------------------------------------------------------------------

/*template<>
inline	basic_string<wchar_t>::basic_string(const wchar_t * str)
	: _length(0), _buffer(static_cast<T *>(0))
{
	resize(static_cast<const unsigned int>(wcslen(str)));

	if (_buffer)
	{
		_buffer[length()] = 0;
		wcscpy(_buffer, str);
	}
}*/

#ifdef	_UNICODE
inline	basic_string<wchar_t>::basic_string(const char * str)
	: _length(0), _buffer(static_cast<T *>(0))
{
	int	len = MultiByteToWideChar(CP_ACP, 0, str, -1, 0, 0);
	if (len) --len;
	resize(len);

	if (_buffer)
	{
		_buffer[length()] = 0;
		MultiByteToWideChar(CP_ACP, 0, str, -1, _buffer, length());
	}
}

inline	basic_string<char>::basic_string(const wchar_t * str)
	: _length(0), _buffer(static_cast<T *>(0))
{
	resize(static_cast<const unsigned int>(wcslen(str)));

	if (_buffer)
	{
		_buffer[length()] = 0;
		WideCharToMultiByte(CP_ACP, 0, str, -1, _buffer, length(), NULL, NULL);
	}
}
#endif // _UNICODE

FSTL_NAMESPACE_END
#endif // _FSTL_STRING
// ---------------------------------------------------------------------------------------------------------------------------------
// string - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

