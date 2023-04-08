//18011092 Arda Kasikci
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>
#define P 3.14
typedef struct pgm {
 	char type[5];
    int n;
    int m;
    int max;
	int** data;
}PGM;

int** conv(PGM* p, double** kernel, int size){
	
	int i,j,x,y;
	double sum = 0.0;
	double** temp;
	
	//Photo copy
	int** data = (int**)malloc((p->n)*sizeof(int*));
	if(data == NULL) {
		printf("Can not allocate memory for the array...\n");
		return;
	}
	for(i=0;i<(p->n);i++){
		data[i]=(int*)malloc((p->m)*sizeof(int));
	}
	
	//ALLOCATION
	temp = (double**)malloc(((p->n)-size+1)*sizeof(double*));
	if(temp == NULL) {
		printf("Can not allocate memory for the array...\n");
		return;
	}
	for(i=0;i<((p->n)-size+1);i++){
		temp[i]=(double*)malloc(((p->m)-size+1)*sizeof(double));
	}
	
	//CONV
	for(i=0;i <= ((p->n) - size);i++){
		for(j=0;j <= ((p->m) - size);j++){
			for(x=0 ; x<size; x++){
				for(y=0; y<size; y++){
					sum += p->data[i+x][j+y]*kernel[x][y];
				}
			}
			temp[i][j]=sum;
			sum=0.0;
			data[i][j]=p->data[i][j];
		}
	}
	
	int start= (size-1)/2;
	
	for(i=start;i<(p->n-start);i++){
		for(j=start;j<(p->m-start);j++){
			data[i][j] = (int) temp[i-start][j-start];
		}
	}
	
	free(temp);
	
	return data;
	
}
void min_max(PGM* p, int** data) {
	int max= INT_MIN, min = INT_MAX;
	int i,j;
	double oran;
	for(i=0; i<(p->n); i++) {
		for(j=0; j<(p->m); j++) {
			if(data[i][j] > max) {
				max=data[i][j];
			}
			else if(data[i][j] < min) {
				min=data[i][j];
			}
		}
	}
	
	for(i=0;i<(p->n);i++){
		for(j=0;j<(p->m);j++){
			oran=(double)(data[i][j]-min)/(max-min);
			data[i][j]=oran*255;
		}
	} 
}
double** create_GaussFilter(int size, double sigma){
	int i,j,x,y;
	double toplam=0.0;
	double bolum,s,us;
	
	s = 2.0*sigma*sigma;//Sigma fonk degeri
	bolum = P*s;//Bolum degeri
	
	double** gauss=(double**)malloc(size*sizeof(double*));
	if(gauss== NULL) {
		printf("Can not allocate memory for the array...\n");
		return 0;
	}
	for(i=0;i<size;i++){
		gauss[i]=(double*)malloc(size*sizeof(double));
	}
	
	int gauss_size = (size-1)/2;//For dongusu icin sinirlar.
	
	for (x = -(gauss_size); x <= gauss_size; x++) {//GAUSS FONKSIYONU
        for (y = -(gauss_size); y <= gauss_size; y++) {
            us = -(x * x + y * y);
            gauss[x + gauss_size][y + gauss_size] = (exp(us / s)) / (bolum);
            toplam += gauss[x + gauss_size][y + gauss_size];
        }
    }
	
	//printf("%d",gauss_size);
	
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
        	gauss[i][j] /= toplam;
		}
	}
	
	return gauss;
}
void skip_comm(FILE *fp){
	char temp;
	char temp2[100];
	while ((temp = fgetc(fp)) != EOF && isspace(temp)) {
	}

	if (temp == '#') {
		fgets(temp2, sizeof(temp2), fp);
		skip_comm(fp);
	} else {
		fseek(fp, -1, SEEK_CUR);
	}
} 
int readPgm(PGM* p,char dosya[]){
	FILE *fp;
	int i,j;
	
	fp=fopen(dosya ,"rb");
	if(!fp){
		printf("'input.pgm' isimli dosya bulunmuyor.\n");
        return 0;
	}
	
	fscanf(fp,"%s",p->type);
	skip_comm(fp);
	fscanf(fp,"%d %d",&p->m,&p->n);
	fscanf(fp,"%d",&p->max);
	
	//ALLOCATION
	p->data=(int**)malloc((p->n)*sizeof(int*));
	if(p->data == NULL) {
		printf("Can not allocate memory for the array...\n");
		return 0;
	}
	for(i=0;i<(p->n);i++){
		p->data[i]=(int*)malloc((p->m)*sizeof(int));
	}
	
	//FILE READ
	if (strcmp(p->type, "P2") == 0) {
		for(i=0;i<(p->n);i++){
			for(j=0;j<(p->m);j++){
				fscanf(fp, "%d", &p->data[i][j]);
			}
		}
	}else{
		
		for(i=0;i<(p->n);i++){
			for(j=0;j<(p->m);j++){
				unsigned char tmp;
				fscanf(fp, "%c", &tmp);
				p->data[i][j]=((int)tmp);
			}
		}
	}
	
	fclose(fp);
	return 1;
}
void write_pgm(PGM *p,int** data,char isim[]){
	int i=0,j=0;
	FILE* fp1=fopen(isim,"wb");
		if(!fp1){
		printf("Dosya olusturulamadi.\n");
        return;
	}
	fprintf(fp1,"%s\n",p->type);
	fprintf(fp1,"%d %d\n",p->m,p->n);
	fprintf(fp1,"%d\n",p->max);
	
	//FILE WRITE
	if(strcmp(p->type, "P2") == 0){
		
		for(i=0; i<p->n;i++){
			for(j=0; j<p->m;j++){
				if(j<(p->m)-1){
					fprintf(fp1,"%d ",data[i][j]);
	
				}else{
					fprintf(fp1,"%d",data[i][j]);
					fprintf(fp1,"\n");
	
				}
			}
		}
		
	}
	else{
		
		for(i=0; i<p->n;i++){
			for(j=0; j<p->m;j++){
				char tmp = data[i][j];
				fprintf(fp1,"%c",tmp);
			}
		}
		
	}
	fclose(fp1);
}
void sobel_filter(PGM* p){
	
	int size = 3,i,j;
	
	//Allocation
	double** filter_x = (double **)malloc(3*sizeof(double*));
	double** filter_y = (double **)malloc(3*sizeof(double*));
	int** x_y = (int **)malloc((p->n)*sizeof(int*));
	if(x_y == NULL) {
		printf("Can not allocate memory for the array...\n");
		return;
	}
	for(i=0;i<(p->n);i++){
		x_y[i]=(int*)malloc((p->m)*sizeof(int));
	}
	if(filter_x == NULL || filter_y == NULL) {
		printf("Can not allocate memory for the array...\n");
		return;
	}
	for(i=0;i<3;i++){
		filter_x[i]=(double*)malloc(3*sizeof(double));
		filter_y[i]=(double*)malloc(3*sizeof(double));
	}
	//****
	
	filter_x[0][0] = -1.0;filter_x[0][1] = 0.0;filter_x[0][2] = 1.0;filter_x[1][0] = -2.0;filter_x[1][1] = 0.0;filter_x[1][2] = 2.0;filter_x[2][0] = -1.0;filter_x[2][1] = 0.0;filter_x[2][2] = 1.0;
	
	
	filter_y[0][0] = -1.0;filter_y[0][1] = -2.0;filter_y[0][2] = -1.0;filter_y[1][0] = 0.0;filter_y[1][1] = 0.0;filter_y[1][2] = 0.0;filter_y[2][0] = 1.0;filter_y[2][1] = 2.0;filter_y[2][2] = 1.0;
	
	int** x = conv(p,filter_x,size);
	int** y = conv(p,filter_y,size);
	for(i=0;i<p->n;i++){
		for(j=0;j<p->m;j++){
			x_y[i][j] = sqrt(x[i][j]*x[i][j] + y[i][j]*y[i][j]);
		}
	}
	
	min_max(p,x);
	write_pgm(p,x,"x.pgm");

	min_max(p,y);
	write_pgm(p,y,"y.pgm");
	
	min_max(p,x_y);
	write_pgm(p,x_y,"x_y.pgm");
	
	free(x);
	free(y);
	free(x_y);
	free(filter_x);
	free(filter_y);
	
	return;
	
}
void laplacian_filter(PGM* p){
	int size = 3,i,j;
	
	//Allocation
	double** lap1 = (double **)malloc(3*sizeof(double*));
	double** lap2 = (double **)malloc(3*sizeof(double*));
	
	if(lap1 == NULL || lap2 == NULL) {
		printf("Can not allocate memory for the array...\n");
		return;
	}
	for(i=0;i<3;i++){
		lap1[i]=(double*)malloc(3*sizeof(double));
		lap2[i]=(double*)malloc(3*sizeof(double));
	}
	//****
	
	lap1[0][0] = 0.0;lap1[0][1] = -1.0;lap1[0][2] = 0.0;
	lap1[1][0] = -1.0;lap1[1][1] = 4.0;lap1[1][2] = -1.0;
	lap1[2][0] = 0.0;lap1[2][1] = -1.0;lap1[2][2] = 0.0;
	
	
	lap2[0][0] = -1.0;lap2[0][1] = -1.0;lap2[0][2] = -1.0;
	lap2[1][0] = -1.0;lap2[1][1] = 8.0;lap2[1][2] = -1.0;
	lap2[2][0] = -1.0;lap2[2][1] = -1.0;lap2[2][2] = -1.0;
	
	int** l1 = conv(p,lap1,size);
	int** l2 = conv(p,lap2,size);
	
	min_max(p,l1);
	write_pgm(p,l1,"l1.pgm");

	min_max(p,l2);
	write_pgm(p,l2,"l2.pgm");
	
	free(l1);
	free(l2);
	free(lap1);
	free(lap2);
	
	return;
}
int main(){
	int i,j;
	//Dosya okuma
	char dosya[50];
	printf("Okunacak PGM dosya ismini giriniz: \n");
	scanf("%s",&dosya);
	PGM* pgm=(PGM*)malloc(sizeof(PGM));
	if(!readPgm(pgm,dosya)){
 		return 0;
	}
	printf("Dosya Okundu.\n");	
	//Okunan Matrisi Yazdirma
	/*
	printf("%s\n",pgm->type);
	printf("%d %d\n",pgm->m,pgm->n);
	printf("%d\n",pgm->max);
	for(i=0;i<pgm->n;i++){
		for(j=0;j<pgm->m;j++){
			printf(" %d ", pgm->data[i][j]);
		}printf("\n");
	}*/
	
	//Gauss Uygulamasý
	int size =3;
	int sigma=1;
	double** gauss = create_GaussFilter(size,sigma);
	//Gauss uygulanan matrisin pgm'e aktarilmasi
	int** temp = conv(pgm,gauss,size);
	for(i=0;i<pgm->n;i++){
		for(j=0;j<pgm->m;j++){
			 pgm->data[i][j]=temp[i][j];
		}
	}
	//Sobel Uygulanmasý
	sobel_filter(pgm);
	
	//Laplace Uygulanmasý
	//laplacian_filter(pgm);
	
	return 0;
}
