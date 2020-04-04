void printString (char* word)
{
  char al = '\n';
  char ah = 0x0e;
  int ax = ah * 256 + al;
  interrupt(0x10, ax, 0, 0, 0);

  while (*word != '\0')
  {
    al = *word;
    ax = ah * 256 + al;
    interrupt (0x10, ax, 0, 0, 0);
    word++;
  }
  return;
}

void readString (char* line)
{

  int i = 0;
  char temp = 0;
  while (1)
  {
    temp = interrupt (0x16, 0, 0, 0, 0);
    
    if (temp == 0xd)
      break;

    if (temp == 0x8)
    {
      if (i > 0)
      {
        interrupt (0x10, 0x0e * 256 + temp, 0, 0, 0);
        i--;
      }
      else
        continue;
    }

    else
    {
      line[i++] = temp;
      interrupt (0x10, 0x0e * 256 + temp, 0, 0, 0); 
    }
  }

  if (i > LENGTH_LINE - 2)
    i = LENGTH_LINE - 2;
  line[i++] = 0xa;
  line[i] = '\0';
  return;
}
