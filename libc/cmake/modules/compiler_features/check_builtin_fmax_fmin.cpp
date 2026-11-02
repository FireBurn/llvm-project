[[gnu::used]] float try_builtin_fmaxf(float x, float y) { return __builtin_fmaxf(x, y); }
[[gnu::used]] float try_builtin_fminf(float x, float y) { return __builtin_fminf(x, y); }

[[gnu::used]] double try_builtin_fmaxf(double x, double y) { return __builtin_fmax(x, y); }
[[gnu::used]] double try_builtin_fminf(double x, double y) { return __builtin_fmin(x, y); }

extern "C" void _start() {}
