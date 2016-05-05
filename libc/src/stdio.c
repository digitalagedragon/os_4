#define STDIO_C
#include <stdio.h>
#include <sys/call.h>
#include <string.h>
#include <stdlib.h>

FILE *stdin, *stdout, *stderr;

void init_stdio(void) {
  syscall3(0x80, 5, (int) "/dev/stdin", 0);
  syscall3(0x80, 5, (int) "/dev/stdout", 1);
  stdin = malloc(sizeof(FILE));
  stdout = malloc(sizeof(FILE));
  stdin->fd = 0;
  stdin->buf = malloc(BUFSIZ);
  stdin->bufwp = stdin->bufrp = stdin->err = 0;
  stdout->fd = 1;
  stdout->buf = malloc(BUFSIZ);
  stdout->bufwp = stdout->bufrp = stdout->err = 0;
}

int putchar(int c) {
  return fputc(c, stdout);
}

int getchar() {
  return fgetc(stdin);
}

int fputc(int c, FILE *stream) {
  stream->buf[stream->bufwp++] = c;
  if(c == '\n') {
    syscall4(0x80, 4, stream->fd, (int) stream->buf, stream->bufwp);
    stream->bufwp = 0;
  }
  return 1;
}

int fflush(FILE *stream) {
  syscall4(0x80, 4, stream->fd, (int) stream->buf, stream->bufwp);
  stream->bufwp = 0;
  return 0;
}

int fgetc(FILE *stream) {
  if(stream->bufwp > stream->bufrp) {
    return stream->buf[stream->bufrp++];
  } else if(stream->bufwp == stream->bufrp) {
    syscall4(0x80, 3, stream->fd, (int) stream->buf, 1);
    stream->bufwp = 1;
    stream->bufrp = 0;
    return stream->buf[stream->bufrp++];
  } else {
    printf("Can't happen in fgetc()\n");
  }
  return 0;
}

#define gc(f) (*((f)++))
#define ugc(f) --(f)

#define LEN_8  0
#define LEN_16 1
#define LEN_32 2
#define LEN_64 3

const char digits[] = "0123456789abcdef";

void itoa_u(unsigned long i, unsigned int radix) {
  if(i >= radix)
    itoa_u(i / radix, radix);
  putchar(digits[i % radix]);
}

void itoa_s(signed long i, int radix) {
  if(i < 0) {
    i = -i;
    putchar('-');
  }
  itoa_u((unsigned long) i, radix);
}

int puts(const char *s) {
  int i;
  for(i = 0; i < strlen(s); i++) putchar(s[i]);
  return 0;
}

 int printf(const char *fmt, ...) {
  __builtin_va_list ap;
  __builtin_va_start(ap, fmt);
  int r = vprintf(fmt, ap);
  __builtin_va_end(ap);
  return r;
}

int vprintf(const char *fmt, __builtin_va_list ap) {
  char c;
  int length = -1;
  while(*fmt) {
    if(*fmt == '%') {
      fmt++;
      switch(c = gc(fmt)) {
      case 'h':
	if((c = gc(fmt)) == 'h') {
	  length = LEN_8;
	} else {
	  length = LEN_16;
	  ugc(fmt);
	}
	break;
      case 'l':
	if((c = gc(fmt)) == 'l') {
	  length = LEN_64;
	} else {
	  length = LEN_32;
	  ugc(fmt);
	}
	break;
      case 'd':
      case 'i':
	switch(length) {
	case LEN_64:
	  itoa_s(__builtin_va_arg(ap, signed long long), 10);
	  break;
	default:
	  itoa_s(__builtin_va_arg(ap, signed int), 10);
	  break;
	}
	break;
      case 'u':
	switch(length) {
	case LEN_64:
	  itoa_u(__builtin_va_arg(ap, unsigned long), 10);
	  break;
	default:
	  itoa_u(__builtin_va_arg(ap, unsigned int), 10);
	  break;
	}
	break;
      case 'o':
	switch(length) {
	case LEN_64:
	  itoa_u(__builtin_va_arg(ap, unsigned long), 8);
	  break;
	default:
	  itoa_u(__builtin_va_arg(ap, unsigned int), 8);
	  break;
	}
	break;
      case 'x':
	switch(length) {
	case LEN_64:
	  itoa_u(__builtin_va_arg(ap, unsigned long), 16);
	  break;
	default:
	  itoa_u(__builtin_va_arg(ap, unsigned int), 16);
	  break;
	}
	break;
      case 'c':
	putchar(__builtin_va_arg(ap, unsigned int));
	break;
      case 's':
	puts(__builtin_va_arg(ap, char *));
	break;
      case '%':
	putchar('%');
	break;
      }
    } else {
      putchar(gc(fmt));
    }
  }
  return 0;
}

#if 0
int printf(char *fmt, ...) {
  syscall4(0x80, 4, 1, (int) fmt, strlen(fmt));
  return 0;
} 
#endif
