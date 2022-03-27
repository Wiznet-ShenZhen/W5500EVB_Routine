#include <string.h>
void base64encode(char *s, char *r) {
  char padstr[4]={0,};
  char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned int i=0, j=0, c=0;
  unsigned long n=0;
  
  c = strlen(s) % 3;
  if (c > 0) { 
    for (i=0; c < 3; c++) { 
      padstr[i++] = '='; 
    } 
  }
  padstr[i]=0;

  i = 0;
  for (j=0; j < strlen(s); j+=3) { 
    // these three 8-bit (ASCII) characters become one 24-bit number
    n = s[j]; 
    n <<= 8;
    n += s[j+1]; 
    if (j+2 > strlen(s)) {
      n &= 0xff00;
    }
    n <<= 8;
    n += s[j+2];
    if (j+1 > strlen(s)) {
      n &= 0xffff00;
    }

    // this 24-bit number gets separated into four 6-bit numbers
    // those four 6-bit numbers are used as indices into the base64 character list
    r[i++] = base64chars[(n >> 18) & 63];
    r[i++] = base64chars[(n >> 12) & 63];
    r[i++] = base64chars[(n >> 6) & 63];
    r[i++] = base64chars[n & 63];
  }
  i -= strlen(padstr);
  for (j=0; j<strlen(padstr); j++) {
    r[i++] = padstr[j];  
  }
  r[i] = 0;
}

