#ifndef __CRYTO_H__
#define __CRYTO_H__
#include "head.h"

#define RSA_EN_LEN 256
#define RSA_DE_LEN 245
#define SER_EN_LEN 384
#define SER_DE_LEN 373

char* rsa_encrypt(const char* str);
char* rsa_sign(char* str, const char* user_name);
char* rsa_decrypt(char* str, const char* user_name);
char* rsa_verify(char* str);
int rsa_generate_key(const char* user_name);
#endif
