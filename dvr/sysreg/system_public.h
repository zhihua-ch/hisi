
#ifndef _SYSTEM_PUBLIC_API_H
#define _SYSTEM_PUBLIC_API_H

#define dprintfbin(buf, size) 					\
	do {							\
		int i; 						\
		for (i = 0; i < size - 1; i++){ 		\
			if (0 == i % 16){ 			\
				if (0 != i)			\
					printf("\n");		\
				printf("0x%04x: ", i); 			\
			}						\
			printf("%02x ", ((char*)buf)[i]);		\
		}							\
		printf("%02x\n", ((char*)buf)[i]); 			\
	} while(0)


void u_delay(unsigned long us);

#endif

