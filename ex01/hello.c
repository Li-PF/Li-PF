 #include<stdio.h>
 int main()
 {
 	char input[100];
	printf("请输入内容：");
	fgets(input,sizeof(input),stdin);
	printf("你输入的内容为：%s",input); 
 	return 0;
 }
