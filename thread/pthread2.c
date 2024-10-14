#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int g_var1 = 0;
pthread_mutex_t mutex;

void *inc_gv() {
	int i,j;
	for (i = 0;i<10;i++) {
		pthread_mutex_lock(&mutex);
		g_var1++;
		for (j=0;j<5000000;j++);
		printf(" %d", g_var1);
		fflush(stdout);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main() {
	pthread_t thread1, thread2;
	pthread_mutex_init(&mutex, NULL);

	int T1ret, T2ret;

	T1ret = pthread_create(&thread1, NULL, inc_gv, NULL);
	T2ret = pthread_create(&thread2, NULL, inc_gv, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("\nT1 & T2 return: %d, %d\n", T1ret, T2ret);
	return 0;
}
