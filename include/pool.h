#ifndef __pool
#define __pool

template <class T> struct pool {
	T elem;
	int free;
	int next;
};
template<class T> inline int get_free1 (pool<T>* p, int n) {
	for (int i = 0; i < n; ++i) if (p[i].free) { p[i].free = 0; return i; }
	return -1;
}

template<class T> inline void free_pool (pool<T>* p, int n) {
	for (int i = 0; i < n; ++i) p[i].free = 1;
}

#endif
