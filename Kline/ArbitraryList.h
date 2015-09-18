//https://home.comcast.net/~tom_forsyth/blog.wiki.html#[[Resizable%20arrays%20without%20using%20STL]]
// An arbitrary-sized list template class.
// Holds ordered data, but some functions also have unordered versions for speed.
template <class T>
class ArbitraryList {
	// Careful with changing these - needs to agree with ArbitraryListGrannyTypeDef
	int		iReservedSize;		// The current reserved size of the list.
	int		iSize;			// The current size of the list.
	T		*pT;				// The list.

public:

	// Optional initial size setting.
	void Init( int iInitialSize = 0, int iInitialReservedSize = 0 ) {
		pT = NULL;
		iSize = 0;
		iReservedSize = 0;
		if ( iInitialReservedSize > iInitialSize ) {
			ReserveTo( iInitialReservedSize, true );
			SizeTo( iInitialSize, false );
		} else if ( iInitialSize > 0 ) {
			SizeTo( iInitialSize, true, true );
		}
	}

	void Fini() {
		if ( pT == NULL ) {
			ASSERT( iReservedSize == 0 );
			ASSERT( iSize == 0 );
		} else {
			ASSERT( iReservedSize > 0 );
			ASSERT( iSize >= 0 );
			delete [] pT;
			iReservedSize = 0;
			iSize = 0;
			pT = NULL;
		}
	}


	// Constructor, with optional initial size setting.
	ArbitraryList( int iInitialSize = 0 ) {
		Init( iInitialSize );
	}

	// Destructor.
	~ArbitraryList( void ) {
		Fini();
	}

	// Returns the pointer to the given item.
	T *Item( const int iItem ) {
		ASSERT( iItem < iSize );
		return ( &pT[iItem] );
	}

	const T *ConstItem( const int iItem ) const {
		ASSERT( this != NULL );
		ASSERT( iItem < iSize );
		return ( &pT[iItem] );
	}

	// Or use the [] operator.
	T &operator []( const int iItem ) {
		return *Item( iItem );
	}

	const T &operator []( const int iItem ) const {
		return *ConstItem( iItem );
	}

	// Returns the pointer to the first item.
	T *Ptr( void ) {
		return ( pT );
	}

	// Returns the size of the list
	int Size( void ) const {
		return iSize;
	}

	// Returns the pointer to the last item plus one - same sense as STL end()
	T *PtrEnd( void ) {
		return ( pT + iSize );
	}

	// Grows or shrinks the list to this number of items.
	// Preserves existing items.
	// Items that fall off the end of a shrink may vanish.
	// Returns the pointer to the first item.
	// Set bBinAtZero to false if you don't want the memory to be freed,
	// even though the size is 0. This speeds things up for a list that is
	// frequently used, even though it's zeroed between uses.
	// Set bAllocExactly to true if you want iReservedSize to be exactly the same as iSize.
	T *SizeTo( int iNum, bool bBinAtZero = true, bool bAllocExactly = false ) {
		ASSERT( iNum >= 0 );
		int iOldSize = iSize;
		iSize = iNum;

		if ( iNum == iReservedSize ) {
			// Already have exactly the right space - all is well.
			if ( iReservedSize == 0 ) {
				ASSERT( pT == NULL );
			} else {
				ASSERT( pT != NULL );
			}
			return pT;
		} else if ( iNum < iReservedSize ) {
			// We have enough space.
			if ( ( iNum == 0 ) && bBinAtZero ) {
				// Shrunk to 0 - bin the memory.
				delete [] pT;
				pT = NULL;
				iReservedSize = 0;
				return NULL;
			}
			if ( !bAllocExactly || ( iNum == iReservedSize ) ) {
				// ...and we don't need to resize down.
				return pT;
			}
		}

		// We got here, so we need to resize the array in some way, either up or down.
		ASSERT( iNum > 0 );
		int iNewSize = iNum;
		if ( !bAllocExactly ) {
			// Usually grow by 50% more than needed to avoid constant regrows.
			iNewSize = ( iNum * 3 ) >> 1;
			if ( iNewSize < 8 ) {
				iNewSize = 8;
			}
			ASSERT( iNewSize > iReservedSize );
		}
		if ( pT == NULL ) {
			ASSERT( iReservedSize == 0 );
			pT = new T[iNewSize];
		} else {
			ASSERT( iReservedSize > 0 );
			T *pOldT = pT;
			pT = new T[iNewSize];
			int iCopySize = iOldSize;
			if ( iCopySize > iNewSize ) {
				iCopySize = iNewSize;
			}
			for ( int i = 0; i < iCopySize; i++ ) {
				pT[i] = pOldT[i];
			}
			delete [] pOldT;
		}
		ASSERT( pT != NULL );
		iReservedSize = iNewSize;
		return ( pT );
	}

	// Preallocates to at least this big.
	void ReserveTo( int iNum, bool bAllocExactly = false ) {
		ASSERT( iNum >= iSize );
		if ( iNum <= iReservedSize ) {
			return;
		}
		int iOldSize = iSize;
		SizeTo( iNum, false, bAllocExactly );
		iSize = iOldSize;
	}

	// Frees all the array memory. Just a fancy way of saying SizeTo(0).
	void FreeMem( void ) {
		SizeTo( 0, true );
	}

	// Removes the given item number by copying the last item
	// to that position and shrinking the list.
	// Looking for the old RemoveItem() function? This is the new name.
	void RemoveItemUnord( int iItemNumber ) {
		ASSERT( iItemNumber >= 0 );
		ASSERT( iItemNumber < iSize );
		pT[iItemNumber] = pT[iSize - 1];
		SizeTo( iSize - 1 );
	}

	// Removes the given item number by shuffling the other items up
	// and shrinking the list.
	void RemoveItemOrd( int iItemNumber ) {
		ASSERT( iItemNumber >= 0 );
		ASSERT( iItemNumber < iSize );

		const int iStopPoint = iSize - 1;
		for ( int i = iItemNumber; i < iStopPoint; i++ ) {
			pT[i] = pT[i + 1];
		}
		SizeTo( iSize - 1 );
	}

	// Adds one item to the list and returns a pointer to that new item.
	T *AddItem( void ) {
		SizeTo( iSize + 1 );
		return ( &pT[iSize - 1] );
	}

	// Adds the supplied item to the list.
	void AddItem( T const &t ) {
		*( AddItem() ) = t;
	}

	// Inserts an item in the specified place in the list, shuffles everything below it down one, and returns a pointer to the item.
	T *InsertItem( int Position ) {
		ASSERT( ( Position >= 0 ) && ( Position <= iSize ) );
		SizeTo( iSize + 1 );
		for ( int i = iSize - 1; i >= Position + 1; i-- ) {
			pT[i] = pT[i - 1];
		}
		return ( &pT[Position] );
	}

	// Inserts an item in the specified place in the list, shuffles everything below it down one.
	void InsertItem( int Position, T &t ) {
		*( InsertItem( Position ) ) = t;
	}

	// Copy the specified data into the list.
	void CopyFrom( int iFirstItem, const T *p, int iNumItems ) {
		if ( iSize < ( iFirstItem + iNumItems ) ) {
			SizeTo( iFirstItem + iNumItems );
		}
		for ( int i = 0; i < iNumItems; i++ ) {
			*( Item( i + iFirstItem ) ) = p[i];
		}
	}

	// A copy from another arbitrary list of the same type.
	void CopyFrom( int iFirstItem, const ArbitraryList<T> &other, int iFirstOtherItem = 0, int iNumItems = -1 ) {
		if ( iNumItems == -1 ) {
			iNumItems = other.Size() - iFirstOtherItem;
		}
		if ( iSize < ( iFirstItem + iNumItems ) ) {
			SizeTo( iFirstItem + iNumItems );
		}
		ASSERT( other.Size() >= ( iFirstOtherItem + iNumItems ) );
		for ( int i = 0; i < iNumItems; i++ ) {
			*( Item( i + iFirstItem ) ) = *( other.ConstItem( i + iFirstOtherItem ) );
		}
	}

	// A copy from another list, but it always adds new items to the end of the current list.
	void AddFrom( ArbitraryList<T> &other, int iFirstOtherItem = 0, int iNumItems = -1 ) {
		if ( iNumItems == -1 ) {
			iNumItems = other.Size() - iFirstOtherItem;
		}
		int iFirstItem = iSize;
		SizeTo( iFirstItem + iNumItems );
		CopyFrom( iFirstItem, other, iFirstOtherItem, iNumItems );
	}

	// A simple find. Returns the list position, or -1 if not found.
	int FindItem( const T &t ) {
		for ( int i = 0; i < iSize; i++ ) {
			if ( pT[i] == t ) {
				return i;
			}
		}
		return -1;
	}

	// Reallocates the memory so that the capacity is exactly the same as the size.
	// Useful for a list that has been constructed, but will now remain the same size for a long time.
	void ShrinkToFit() {
		SizeTo( iSize, true, true );
	}

	// Copy constructor.
	ArbitraryList( const ArbitraryList<T> &other ) {
		int iNumItems = other.Size();

		pT = NULL;
		iSize = 0;
		iReservedSize = 0;
		SizeTo( iNumItems );
		for ( int i = 0; i < iNumItems; i++ ) {
			*( Item( i ) ) = *( other.Item( i ) );
		}
	}
};


// See, it's foreach, except it's my "special" version, so I called it... I'll get me coat.

#ifndef DEBUG

#define forsytheach(T,it,list) for ( T *it = list.Ptr(), *last = list.PtrEnd(); it != last; ++it )
// Same, but easier syntax for lists-of-values that you don't mind copying.
#define forsytheachval(T,ref,list) for ( T *it = list.Ptr(), *last = list.PtrEnd(), ref = ((it!=last)?*it:T()); it != last; ref = *(++it) )
// Same, but easier syntax for lists-of-pointers-to-objects
#define forsytheachptr(T,ref,list) for ( T **it = list.Ptr(), **last = list.PtrEnd(), *ref = ((it!=last)?*it:NULL); it != last; ref = *(++it) )

#else //#ifndef DEBUG

// Debug versions that try to self-check that you didn't modify the list in the middle of the loop.
// If you do, that can cause a reallocate, and then everything fails horribly.
// In those cases, you need to do a loop of NumItems from 0 to thing.Size() and inside the loop
// explicitly do Thing *pThing = thing[ThingNum]

#define forsytheach(T,it,list) for ( T *it = list.Ptr(), *first = list.Ptr(), *last = list.PtrEnd(); internal_functional_assert(first==list.Ptr()), internal_functional_assert(last==list.PtrEnd()), it != last; ++it )
// Same, but easier syntax for lists-of-values that you don't mind copying.
#define forsytheachval(T,ref,list) for ( T *it = list.Ptr(), *first = list.Ptr(), *last = list.PtrEnd(), ref = ((it!=last)?*it:T()); internal_functional_assert(first==list.Ptr()), internal_functional_assert(last==list.PtrEnd()), it != last; ref = *(++it) )
// Same, but easier syntax for lists-of-pointers-to-objects
#define forsytheachptr(T,ref,list) for ( T **it = list.Ptr(), **first = list.Ptr(), **last = list.PtrEnd(), *ref = ((it!=last)?*it:NULL); internal_functional_assert(first==list.Ptr()), internal_functional_assert(last==list.PtrEnd()), it != last; ref = *(++it) )

#endif
