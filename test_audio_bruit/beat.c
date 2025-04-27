#include <math.h>
#include <stdio.h>

int main() {
  const char digits1[] = "1370";
  const char digits2[] = "12346543";
  const int table[] = {9, 12, 12, 14, 13, 12, 11, 14,
                       9, 12, 12, 14, 13, 12, 11, 14};

  for (unsigned int t = 0;; t++) {
    // Extraire les chiffres depuis les chaînes
    int part1 = ((15 + 1 * (digits1[(t >> 17) & 3] - '0')) / 8) *
                    (digits2[(t >> 12) & 7] - '0') * ((t / 4) % 192) +
                160;

    part1 = (part1 % 256) / 2;

    // table lookup pour puissance de 2
    int index = ((t >> 11 & 15) | ((t ^ (t / 32 % 8) * t)) & 11);
    int shift = table[index & 15];
    int mask = (1 << shift) - 1;
    int part2 = sqrt(5000.0 * (t % 65536 & mask));
    part2 &= 128;

    putchar((part1 + part2) & 255);
  }
}
