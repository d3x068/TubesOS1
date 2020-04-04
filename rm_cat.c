/* remove.c  */

#include "FileIO.c"
#define ERROR (-1)

int catfd(int i_fd, int o_fd)
{
    char buff[512];
    ssize_t n_bytes;
    while ((n_bytes = read(i_fd, buff, sizeof(buff))) > 0)
    {
        if (write(o_fd, buff, n_bytes) != n_bytes)
            return -1;
    }
    return (n_bytes < 0) ? -1 : 0;
}

int main(int argc, char *argv[])
{
  if (argc==2)
  {
    if (unlink(agrv[1]) != ERROR)
    {
      deleteFile(argv[1])
    }
  }
  if (argc==1)
  {
      if (catfd(0, 1) != 0)
          printString("failed to copy standard input");
  }
  else
  {
      int i=1;
      while(i<argc)
      {
          int fd; 
          fd = open(argv[i],O_RDONLY);
          if (fd < 0)
              printString("failed to open for reading");
          else
          {
              if (catfd(fd,1)!=0)
                  printString("failed to copy to standard output");
              close(fd);
          }
          i++;
      }
  }
  exit (0);
}