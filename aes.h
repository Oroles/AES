

//#define Nb 4
//#define Nk 4
//#define Nr 10
//#define Size 128

void cipher(char in[16], unsigned long w[44], char (*out)[16]);
void invCipher(char in[16], unsigned long w[44], char (*out)[16]);
void keyExpansion(char key[16], unsigned long w[44] );
