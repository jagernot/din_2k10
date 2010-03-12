#ifndef __random
#define __random

/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.
*/

const int N = 624;
const int M = 397;
const int MATRIX_A = 0x9908b0df; /* constant vector a */
const int UPPER_MASK = 0x80000000; /* most significant w-r bits */
const int LOWER_MASK = 0x7fffffff; /* least significant r bits */

static unsigned int mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
inline void setSeed (unsigned int s)
{
    mt[0]= s & 0xffffffff;
    for (mti=1; mti<N; mti++) {
        mt[mti] =
	    (1812433253 * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        //mt[mti] &= 0xffffffff;
        /* for >32 bit machines */
    }
}

inline unsigned int getRand32 (void)
{
    unsigned int y;
    static unsigned int mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk, nm=N-M, mn=-nm, n1=N-1, m1=M-1;

        if (mti == N+1)   /* if init_genrand() has not been called, */
           setSeed (0);

        for (kk=0;kk<nm;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<n1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+mn] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[n1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[n1] = mt[m1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= (y >> 18);

    return y;

}

inline unsigned int getRand16 (void) {
	return getRand32()&0xffff;
}

inline unsigned int getRand8 (void) {
	return getRand32()&0xff;
}

inline unsigned int getRand7 (void) {
	return getRand32()&0x7f;
}

inline unsigned int getRandBit (void) {
	return getRand32()&0x01;
}

inline double getRand021 (void) {
	return (getRand16() * 1.0 / 0xffff);
}

template<typename T> class rnd {

	T min, max;
	T delta;

	public:
		rnd (T mi, T ma) {
			min = mi;
			max = ma;
			delta = max - min;
		}
		T operator() () {
			return (T) (min + delta * getRand021());
		}
};

#endif
