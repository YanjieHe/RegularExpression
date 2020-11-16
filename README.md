# RegularExpression

## Examples

### Expression

```
auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
```

### NFA

![NFA1.png](docs/images/NFA1.png)

### DFA

![DFA1.png](docs/images/DFA1.png)

### Expression

```
auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
```

### NFA

![NFA2.png](docs/images/NFA2.png)

### DFA

![DFA2.png](docs/images/DFA2.png)

### Expression

```
auto e3 = (Range(U'0', U'9') | Symbol(U'a'))->Many();
```

### NFA

![NFA3.png](docs/images/NFA3.png)

### DFA
![DFA3.png](docs/images/DFA3.png)

### Expression

```
auto e4 = (Range(U'0', U'9')->Many() | (LineBegin() + Symbol(U'a')->Many()));
```

### NFA

![NFA4.png](docs/images/NFA4.png)

### DFA
![DFA4.png](docs/images/DFA4.png)