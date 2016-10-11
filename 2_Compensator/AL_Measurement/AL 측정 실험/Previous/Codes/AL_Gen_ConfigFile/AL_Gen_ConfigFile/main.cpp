#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

template <typename T>
void swap(T *a, T *b)
{
	T tmp;
	tmp = *a; *a = *b; *b = tmp;
}

void main()
{
	printf("이름을 입력하세요 : ");
	char name[200];
	scanf("%s", name);


	char file_name[200] = ".\\Data\\S1\\config_.cfg";
	
	FILE *wf = fopen(file_name, "w");

	fprintf(wf,"[Number] 1\n");
	
	fprintf(wf,"[Name] %s\n",name);
	fprintf(wf,"[Frequencies] 12\n");
	fprintf(wf,"[(Hz)] 10 20 35 50 70 95 120 150 200 280 400 550\n");  ////////////// 나중 수정할 수 있음
	fprintf(wf,"[(micrometer)] 6.2 8.3 1.7 1.1 0.36 0.75 0.7 0.45 0.42 0.6 0.7 0.8\n");  /////////////// 나중 수정할 수 있음
	fprintf(wf,"[ISI(ms)] 1000\n");
	fprintf(wf,"[Duration(ms)] 1000\n");
	fprintf(wf,"[Reversals] 7\n");
	fprintf(wf,"[Step(dB)] 4 4 3 1 1 1 1\n"); //////////////////// 나중 수정할 수 있음
	fprintf(wf,"[Feedback] OFF\n");
	fprintf(wf,"(Hz)\n");

	int freq[12] = { 10, 20 ,35 ,50, 70, 95 ,120, 150 ,200 ,280, 400, 550 };  ////////////////// 나중 frequency set 수정되면 반드시 수정되어야 함

	srand(time(NULL));
	
	for(int j=0;j<40;j++) {
		for(int i=0;i<12;i++) {
			swap<int>(&freq[i], &freq[rand()%12]);
		}
	}
	for(int i=0;i<12;i++) {
		fprintf(wf,"%d %d\n",i+1,freq[i]);
	}
	fprintf(wf,"\n-- 0\n");
	fclose(wf);

}