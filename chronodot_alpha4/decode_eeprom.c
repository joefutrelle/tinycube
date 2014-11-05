#include <stdio.h>
#include <stdint.h>

// expects input format
// " %02x %02x %02x %02x %02x %02x %02x %02x"

int main() {
  char in_buf[128];
  while(fgets(in_buf, sizeof(in_buf), stdin) != NULL) {
    unsigned char buf[8];
    int i = 0, j = 0;
    for(;in_buf[i] != '\n';j++,i+=3) {
      int b;
      sscanf(&in_buf[i], " %02x", &b);
      buf[j] = b;
    }
    printf("%04d-%02d-%02dT%02d:%02d:%02d %.2fC\n",
	   (buf[0] >> 1) + 2000, // yyyy
	   ((buf[0] & 0x01) << 3) | (buf[1] >> 5), // mm
	   buf[1] & 0x1F, // dd
	   ((buf[2] & 0x03) << 4) | (buf[3] >> 4), // HH
	   ((buf[3] & 0x0F) << 2) | (buf[4] >> 6), // MM
	   buf[4] & 0x3F, // SS
	   (float)(buf[5] << 8 | buf[6]) / 4); // temp
  }
}
