#ifndef Y86EMUL_H
#define Y86EMUL_H

void stringDirective(char * tok);
void longDirective(char * tok);
unsigned long hextodecUnsigned(char * num, unsigned char * y86);
double power(int num, int exponent, unsigned char * y86);
void twoscomplement(char * number,  unsigned char * y86);
int inBounds(int adr);
void sizeDirective(char * tok);
void bssDirective(char * tok);
unsigned int hextobyte(char * byte,unsigned char * y86);
long getImmediateValue(unsigned long tempPC, unsigned char * y86);
void byteDirective(char * tok);
void textDirective(char * tok);
void Execution();
unsigned long getAddressValue(unsigned long tempPC,unsigned char * y86);
char * getByteChars(unsigned int byte, unsigned char * y86);
long hextodecSigned(char * num,unsigned char * y86);


int inBounds(int adr){
	if(adr < 0){
	//	status = ADR;
		return 1;
	}
	return 0;
}

unsigned int hextobyte(char * byte, unsigned char * y86){
	int i;
	unsigned int sum = 0;
	for(i = strlen(byte) - 1; i >= 0; i--){
		char digit = byte[i];
		if(digit > '9'){
			sum += (digit - 'a' + 10) * power(16, strlen(byte) - (i+1), y86) ;}
		else{
			sum += (digit - '0') * power(16, strlen(byte) - (i+1), y86);}}
	return sum;
}

unsigned long getplaceValue(unsigned long tempprogCounter, unsigned char * y86){
	char bigEndian[9];
	int i;
	for(i = 0; i < 9; i++){
		bigEndian[i] = '\0';}
	strcat(bigEndian, getByteChars(y86[tempprogCounter + 3], y86));
	strcat(bigEndian, getByteChars(y86[tempprogCounter + 2], y86));
	strcat(bigEndian, getByteChars(y86[tempprogCounter + 1], y86));
	strcat(bigEndian, getByteChars(y86[tempprogCounter], y86));
	return hextodecUnsigned(bigEndian, y86);
}

long getImmediateValue(unsigned long tempprogCounter, unsigned char * y86){
	char bigEndian[9];
	int i;
	for(i = 0; i < 9; i++){
		bigEndian[i] = '\0';
	}
	strcat(bigEndian, getByteChars(y86[tempprogCounter + 3], y86));
	strcat(bigEndian, getByteChars(y86[tempprogCounter + 2], y86));
	strcat(bigEndian, getByteChars(y86[tempprogCounter + 1], y86));
	strcat(bigEndian, getByteChars(y86[tempprogCounter],y86));
	return hextodecSigned(bigEndian, y86);
}

char * getByteChars(unsigned int byte, unsigned char * y86){
	char hex[16]={"0123456789abcdef"};
	char * byteChar = calloc(1,3);
	int i = 1;
	while(byte){
		byteChar[i] = hex[byte % 16];
		byte = byte/16;
		i--;}
	if(byteChar[0] == '\0'){
		byteChar[0] = '0';}
	if(byteChar[1] == '\0'){
		byteChar[1] = '0';}
	return byteChar;
}

long hextodecSigned(char * num, unsigned char * y86){
	char* bDigits[16] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};
	int i, isNeg = 0;
	char* binary = (char*) calloc(1,strlen(num) * 4 + 1);
	for(i = 0; i < strlen(num); i++){
		char digit = num[i];
		if(digit > '9'){
			strcat(binary, bDigits[digit - 'a' + 10]) ;	}
		else{
			strcat(binary, bDigits[digit - '0']);}}
	if(binary[0] == '1'){
		twoscomplement(binary, y86);
		isNeg = 1;}
	int j;
	long sum = 0;
	for(j = strlen(binary) - 1; j >= 0; j--){
		sum += (binary[j] - '0') * power(2, strlen(binary) - (j+1), y86);}
	if(isNeg == 1)
		return -1 * sum;
	else
		return sum;
}

unsigned long hextodecUnsigned(char * num, unsigned char * y86){
	int j;
	unsigned long sum = 0;
	for(j = strlen(num) - 1; j >= 0; j--){
		char digit = num[j];
		if(digit > '9'){
			sum += (digit - 'a' + 10) * power(16, strlen(num) - (j+1), y86) ;}
		else{
			sum += (digit - '0') * power(16, strlen(num) - (j+1), y86);}
	}
	return sum;
}

void twoscomplement(char * number, unsigned char * y86){
		int i;
	for(i = strlen(number) - 1; i >= 0; i--){
		if(number[i] == '1'){
			number[i] = '0';
			break;}
		else if(number[i] == '0'){
			number[i] = '1';}
		else{
			printf("NaN\n");
			exit(1);
		}
	}
	int x;
	for(x = 0; x < strlen(number); x++){
		if(number[x] == '0'){
			number[x] = '1';}
		else if(number[x] == '1')	{
			number[x] = '0';}
		else{
			printf("NaN\n");
			exit(1);
		}
	}
}

double power(int num, int exponent, unsigned char * y86){
	if(exponent == 0){
		return 1.0;}
	if(exponent > 0){
		if(exponent == 1){
			return num;}
		int i;
		double number = num;
		for(i = 2; i <= exponent; i++){
			number *= num;
		}
		return number;
	}
	else {
		exponent = exponent * -1;
		if(exponent == 1){
			return 1.0/num;}
		int i;
		double denominator = (double) num;
		for(i = 2; i <= exponent; i++){
			denominator *= num;}
		double number = 1/denominator;
		return number;
	}
}

#endif
