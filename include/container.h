#ifndef __container
#define __container

template<class S, class T> inline void clear (S& container) {

	// clears container of pointers. deletes the pointers.
  typedef typename S::iterator Si;
	for (Si i = container.begin(), j = container.end(); i != j; ++i) {
		T* t = *i;
		delete t;
	}
	container.clear ();
}

template<class T> inline void erase (T& container, unsigned int q) {

	// erases qth item in container

	typedef typename T::iterator Ti;

	Ti iter = container.begin();
		for (unsigned int p = 0; p < q; ++p, ++iter);
	if (iter != container.end()) container.erase (iter);

}

template<class S, class T> inline T& get (S& container, unsigned int q) {

	// gets qth item in container

   typedef typename S::iterator Si;

	Si iter = container.begin ();
		for (unsigned int p = 0; p < q; ++p, ++iter);
	return *iter;

}

template<class S, class T, class I> inline T& get (S& container, I& iter, unsigned int q) {

	// gets qth item of container and its iterator
	iter = container.begin ();
	for (unsigned int p = 0; p < q; p++, ++iter);
	return *iter;

}

#endif
