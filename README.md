# BigInt Library (Учебный проект)

Учебный проект на C++ для работы с большими целыми числами (`bigint`) и математическими операциями: арифметика, простые числа, факторизация, генерация случайных чисел, комбинаторика.

> ⚠️ Учебный проект. Для экстремально больших чисел использовать специализированные библиотеки вроде GMP.

---

## Возможности

- **Тип `bigint`**: поддержка произвольной длины, арифметика, сравнения.
- **Расширенные алгоритмы** (`bigint_extra.h`):  
  - `egcd(x, y)` — расширенный бинарный НОД с коэффициентами Безу.  
  - `modular_inverse(x, y)` — обратный по модулю.  
- **Случайные числа**: `random_bigint()`, `random_bigint(length)`, `random_prime_bigint()`.
- **Простые числа**: тесты Миллера–Раббина, Ферма, комбинированная проверка `test_if_prime(n)`.
- **Факторизация**: `factorise(bigint a)` с Pollard Rho, решетом и Миллер–Раббином.
- **Комбинаторика**: `factorial(n)`, `binomial(n,k)`, `partperm(n,k)`, `fibonacci(n)`.
- **Модульная арифметика**: `pow_mod(n, power, m)`, `mul_mod(a, b, m)`.

---

## Пример использования

```cpp
#include "bigint.h"
#include "bigint_extra.h"
#include <iostream>

int main() {
    bigint a = random_bigint(5);
    bigint b = random_bigint(5);
    
    std::cout << "a + b = " << a + b << "\n";
    std::cout << "gcd(a, b) = " << egcd(a, b)[2] << "\n";
    
    bigint p = random_prime_bigint(6);
    std::cout << "Random prime: " << p << "\n";
    
    factorisation f = factorise(p * a);
    f.print();
    
    return 0;
}
