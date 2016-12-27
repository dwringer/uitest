/* This is a naive and probably inaccurate implementation
   of a Park-Miller Pseudo-Random Number Generator */
/*** Compiled with MSVC this can be used to make a .dll ***/
#define T_ull unsigned long long
#define T_ld long double


T_ull PRNG_G = 44485709377909;
T_ull PRNG_N = 281474976710656;
__declspec( dllexport ) T_ull PRNG_SEED = 1;


T_ld prng_randf(T_ull x, T_ull g, T_ull n) {
  T_ld val = (T_ld)x / n;
  PRNG_SEED = ((x * g) % n);
  return val;
}

T_ld randf() {
  return prng_randf(PRNG_SEED, PRNG_G, PRNG_N);
}

__declspec( dllexport ) double random() {
  return (double)randf();
}

__declspec( dllexport ) void random_seed(T_ull seed) {
	PRNG_SEED = seed;
}
