
#include <counter.h>

void
Counter_Reset(
    Counter counter,
    int length)
{
  char *text = counter;
  int index = length - 1;

  for (int i = 0; i < index; i++) {
    text[i] = ' ';
  }

  text[index] = '\0';
}

static inline void
Counter_Shift(
    Counter counter,
    int length)
{
  char *text = counter;
  if (text[length] == '\0') {
    Counter_Reset(counter, length + 1);
    length = 0;
  }

  for (int i = length; i > 0; i--) {
    text[i] = text[i-1];
  }

  text[0] = '1';
}

void
Counter_IncrementOne(Counter counter) {
  int length = 0;
  char *text = counter;

  while (text[length] != ' ' && text[length] != '\0') {
    length++;
  }

  if (length <= 0) {
    return Counter_Shift(counter, length);
  }

  int index = length - 1;
  while (index >= 0 && text[index] == '9') {
    text[index] = '0';
    index--;
  }

  if (index < 0) {
    return Counter_Shift(counter, length);
  }
  text[index]++;
}
