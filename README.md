# A DFA-based Regular Expression Library for C++

## Run All the Tests

Input the following commands in the terminal to run all the tests.

```bash
mkdir build
cd build
cmake ..
cmake --build .
./RegularExpression
```

## API

### Regular Expression Notations

The followings are the functions used for constructing regular expressions.

```cpp
RegularExpression::Ptr operator|(const RegularExpression::Ptr& x, const RegularExpression::Ptr& y);
RegularExpression::Ptr operator+(const RegularExpression::Ptr& x, const RegularExpression::Ptr& y);

RegularExpression::Ptr Symbol(char32_t c);
RegularExpression::Ptr Literal(const u32string& text);
RegularExpression::Ptr Range(char32_t lower, char32_t upper);
RegularExpression::Ptr LineBegin();
RegularExpression::Ptr LineEnd();
RegularExpression::Ptr RepeatExactly(const RegularExpression::Ptr& x, int times);
RegularExpression::Ptr RepeatAtLeast(const RegularExpression::Ptr& x, int times);
RegularExpression::Ptr Repeat(const RegularExpression::Ptr& x, int atLeast, int atMost);
```

### Use Regular Expressions

After compiling the regular expressions to DFA matrices, you can use the member functions of DFA matrix to match string patterns.

```cpp
/**
 * DFAMatrix::FullMatch
 * 
 * @param  {u32string} str : check if the pattern can be applied to all of the string
 * @return {bool}          : returns true if can
 */
bool DFAMatrix::FullMatch(const u32string &str) const;


/**
 * DFAMatrix::Search
 *
 * @param  {u32string::const_iterator} strBegin : start of the target character range
 * @param  {u32string::const_iterator} strEnd   : end of the target character range
 * @return {u32string::const_iterator}          : The start position of the first occurrence of the pattern. It equals to "strEnd" if the pattern is not found.
 */
u32string::const_iterator DFAMatrix::Search(u32string::const_iterator strBegin, u32string::const_iterator strEnd) const;


/**
 * DFAMatrix::Match
 *
 * Match the pattern from the beginning.
 *
 * @param  {u32string::const_iterator} strBegin : start of the target character range
 * @param  {u32string::const_iterator} strEnd   : end of the target character range
 * @param  {bool} greedyMode                    : If true, search for the longest match. Otherwise, return immediately once matched.
 * @return {int}                                : the length of the matched string. -1 if no match.
 */
int DFAMatrix::Match(u32string::const_iterator strBegin, u32string::const_iterator strEnd, bool greedyMode) const;
```

## Examples

### Basic Usage

```cpp
#include <iostream>
#include <NFA.hpp>

using std::cout;
using std::endl;
using namespace regex::notations;

int main() {
  auto e = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
  auto matrix = e->Compile();
  if (matrix.FullMatch(U"ab")) {
    cout << "matched" << endl;
  } else {
    cout << "not matched" << endl;
  }
  return 0;
}
```

### Expression

```cpp
auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
```

### NFA

![NFA1.png](docs/images/NFA1.png)

### DFA

![DFA1.png](docs/images/DFA1.png)

### Expression

```cpp
auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
```

### NFA

![NFA2.png](docs/images/NFA2.png)

### DFA

![DFA2.png](docs/images/DFA2.png)

### Expression

```cpp
auto e3 = (Range(U'0', U'9') | Symbol(U'a'))->Many();
```

### NFA

![NFA3.png](docs/images/NFA3.png)

### DFA
![DFA3.png](docs/images/DFA3.png)

### Expression

```cpp
auto e4 = (Range(U'0', U'9')->Many() | (LineBegin() + Symbol(U'a')->Many()));
```

### NFA

![NFA4.png](docs/images/NFA4.png)

### DFA
![DFA4.png](docs/images/DFA4.png)