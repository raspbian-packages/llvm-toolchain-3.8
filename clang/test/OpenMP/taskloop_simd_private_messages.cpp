// RUN: %clang_cc1 -verify -fopenmp %s

void foo() {
}

bool foobool(int argc) {
  return argc;
}

struct S1; // expected-note 2 {{declared here}} expected-note 2 {{forward declaration of 'S1'}}
extern S1 a;
class S2 {
  mutable int a;

public:
  S2() : a(0) {}
};
const S2 b;
const S2 ba[5];
class S3 {
  int a;

public:
  S3() : a(0) {}
};
const S3 ca[5];
class S4 {
  int a;
  S4(); // expected-note {{implicitly declared private here}}

public:
  S4(int v) : a(v) {}
};
class S5 {
  int a;
  S5() : a(0) {} // expected-note {{implicitly declared private here}}

public:
  S5(int v) : a(v) {}
};

S3 h;
#pragma omp threadprivate(h) // expected-note 2 {{defined as threadprivate or thread local}}

template <class I, class C>
int foomain(I argc, C **argv) {
  I e(4);
  I g(5);
  int i;
  int &j = i;
#pragma omp taskloop simd private // expected-error {{expected '(' after 'private'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private() // expected-error {{expected expression}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(S1) // expected-error {{'S1' does not refer to a value}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(a, b) // expected-error {{private variable with incomplete type 'S1'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argv[1]) // expected-error {{expected variable name}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(e, g)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(h) // expected-error {{threadprivate or thread local variable cannot be private}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd shared(i)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
  {
    int v = 0;
    int i;
#pragma omp taskloop simd private(i)
    for (int k = 0; k < argc; ++k) {
      i = k;
      v += i;
    }
  }
#pragma omp parallel shared(i)
#pragma omp parallel private(i)
#pragma omp taskloop simd private(j)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(i)
  for (int k = 0; k < argc; ++k)
    ++k;
  return 0;
}

void bar(S4 a[2]) {
#pragma omp parallel
#pragma omp taskloop simd private(a)
  for (int i = 0; i < 2; ++i)
    foo();
}

namespace A {
double x;
#pragma omp threadprivate(x) // expected-note {{defined as threadprivate or thread local}}
}
namespace B {
using A::x;
}

int main(int argc, char **argv) {
  S4 e(4);
  S5 g(5);
  int i;
  int &j = i;
#pragma omp taskloop simd private // expected-error {{expected '(' after 'private'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private() // expected-error {{expected expression}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argc)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(S1) // expected-error {{'S1' does not refer to a value}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(a, b) // expected-error {{private variable with incomplete type 'S1'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(argv[1]) // expected-error {{expected variable name}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(e, g) // expected-error {{calling a private constructor of class 'S4'}} expected-error {{calling a private constructor of class 'S5'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(h) // expected-error {{threadprivate or thread local variable cannot be private}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(B::x) // expected-error {{threadprivate or thread local variable cannot be private}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd shared(i)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
  {
    int i;
#pragma omp taskloop simd private(i)
    for (int k = 0; k < argc; ++k)
      ++k;
  }
#pragma omp parallel shared(i)
#pragma omp parallel private(i)
#pragma omp taskloop simd private(j)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp taskloop simd private(i)
  for (int k = 0; k < argc; ++k)
    ++k;
  static int si;
#pragma omp taskloop simd private(si) // OK
  for(int k = 0; k < argc; ++k)
    si = k + 1;

  return 0;
}

