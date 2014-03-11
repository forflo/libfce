/*
 provides functions for dynamically growing strings
 */
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#ifdef TEST
#include <CUnit/Cunit.h>
#include <CUnit/Basic.h>
#endif

/* Initializes a new dynamic string structure 
	Return: Null on memory failure. Valid pointer on a dyn_string 
		structure on success */
struct dyn_string *ds_get(){
	struct dyn_string *string = (struct dyn_string *) 
								malloc (sizeof(struct dyn_string) * 1);
	string->chars = 0;
	string->space = DYN_INIT_SIZE - 1; /* space without \0 */
	string->data = (char *) malloc(sizeof(char) * DYN_INIT_SIZE);
	if (string->data == NULL){
		return NULL;
	}	
	return string;
}

/* Initializes a new dynamic string structure and fills it with an
	initial String
	Return: Null on memory failure. Valid pointer on a dyn_string 
		structure on success */
struct dyn_string *ds_get_string(char *str){
	struct dyn_string *string = ds_get();
	ds_append_string(string, str);
	return string;	
}

/* Adapts a given dynamic string to the memory it consumes 
	Return: Null on memory failure. Valid pointer on a dyn_string 
		structure on success */
int ds_adapt_string(struct dyn_string *string){
	if(NULL != (string->data = (char *) realloc(string->data, 
									sizeof(char) * string->chars + 1))) {
		string->space = string->chars + 1;
		return 0;
	} else {
		return 1;
	}
}

//TODO: make returncode
/* Appends a given string
	Param: string = Pointer to a valid dyn_strint structure 
		source = String to append
	Return: -1 on failure, 0 on success */
int ds_append_string(struct dyn_string *string, const char *source){
	int i, j;
	int source_len = strlen(source);
	int future_len = string->chars + source_len;
	if(string == NULL)
		return -1;
	/* verlängern, solange Puffer zu klein */
	while(1){
		if(future_len > string->space){
			string->space += DYN_REALLOC_SIZE;
			string->data = (char *) realloc(string->data, 
									sizeof(char) * string->space);
		} else {
			break;
		} 
	}

	/* String anhängen */
	for(j=0, i=string->chars; j<=source_len; j++, i++){
		string->data[i] = source[j]; /* Schreibt auch das \0 */
	}	
	string->chars += source_len;
	return 0;
}

//TODO: make returncode
/* Removes all specified trailing chars
	Param: c = Char to remove
		strint = Pointer to a valid dyn_string structure
	Return: -1 on failure, 0 on success */
int ds_remove_trailing_char(struct dyn_string *string, char c){
	int rev = string->chars - 1; 
	if (string == NULL)
		return -1;
	while(rev >= 0){
		if(string->data[rev--] == c){
			string->chars--;
		} else {
			break;
		}
	}
	string->data[string->chars] = '\0';
	ds_adapt_string(string);
	return 0;
}

/* Removes all trailing whitespaces 
	Param: strint = Pointer to a valid dyn_string structure */
int ds_remove_trailing_whitespaces(struct dyn_string *string){
	int i, rev = string->chars - 1; 
	char ws[7] = {'\n', '\t', '\a', '\v', '\b' ,'\r', '\f'};
	if (string == NULL)
		return -1;
	while(rev >= 0){
		switch(string->data[rev]){
			case '\n':
				rev --;
				string->chars--;
				break;
			case '\t':
				rev --;
				string->chars--;
				break;
			case '\a':
				rev --;
				string->chars--;
				break;
			case '\v':
				rev --;
				string->chars--;
				break;
			case '\b':
				rev --;
				string->chars--;
				break;
			case '\r':
				rev --;
				string->chars--;
				break;
			case '\f':
				rev --;
				string->chars--;
				break;
			default:
				rev = -1;
				break;
		}

	}
	string->data[string->chars] = '\0';
	ds_adapt_string(string);
	return 0;
}

/* Appends a given char to the dynamic string
	Param: string = Pointer to a valid dyn_string structure 
		source = char to append
	Return: -1 on failure, 0 on success */
int ds_append_char(struct dyn_string *string, const char source){
	char buffer[2] = {'\0', '\0'};
	if(string == NULL)
		return -1;
	buffer[0] = source; 
	return ds_append_string(string, buffer);
}

/* Frees the space occupied by a dynamic string strucutre
	Param: string = Pointer to a valid dyn_string structure
	Return: -1 on failure, 0 on success */
int ds_free(struct dyn_string *string){
	if(string == NULL)
		return -1;
	free(string->data);	
	free(string);
	return 0;
}

#ifdef TEST
/*
 ---Begin optional Testcode---
 */

int dummy(void){return 0;}

void test_dyn_str(void){
	dyn_string_t *string = ds_get();
	ds_free(string);
}

void test_dyn_str_2(void){
	int i, iter = 10000;
	char *str = "Hallo Welt!"; 
	dyn_string_t *string = ds_get();
	ds_append_string(string, str);
	CU_ASSERT(string != NULL);
	CU_ASSERT(string->chars == strlen(str));
	CU_ASSERT(string->space == DYN_INIT_SIZE - 1);
	CU_ASSERT(string->data != NULL);
	printf("\n[test_dyn_str_2] String von string ist: %s\n", string->data);
	printf("%d, %d", string->chars, string->space);
	ds_free(string);

	dyn_string_t *string2 = ds_get();
	for(i=0; i<iter; i++)
		ds_append_string(string, str);
	CU_ASSERT(string2->chars == strlen(str) * iter);
	CU_ASSERT(string2 != NULL);
	CU_ASSERT(string2->data != NULL);
	printf("[test_dyn_str_2] String von string2 ist: %s\n", string2->data);
	printf("[test_dyn_str_2] String von string2 hat die Größe: %d\n", string2->space);
	ds_free(string);
}

void test_dyn_str_3(void){
	int i, iter = 1000;
	char a = 'a';
	dyn_string_t *string = ds_get();
	for(i=0; i<iter; i++)
		ds_append_char(string, a);	
	CU_ASSERT(string->chars == iter);
	CU_ASSERT(string->chars == strlen(string->data));
	for(i=0; i<iter; i++)
		CU_ASSERT(string->data[i] = a);

	printf("\n[test_dyn_str_3] String ist für %d Zeichen reserviert\n", string->space);	
	printf("[test_dyn_str_3] String ist: %s\n", string->data);
	ds_free(string);
}

void test_dyn_str_4(void){
	int i, cc=0;
	char *table[10] = {"Dies", "ist", "ein", "Text", "für", "eine", "Testfunktion", ",", "Komma", "Komma"};	
	char *ts = "DiesisteinTextfüreineTestfunktion,KommaKomma";
	dyn_string_t *string = ds_get();

	for(i=0; i<10; i++){
		ds_append_string(string, table[i]);
		cc += strlen(table[i]);
	}
	CU_ASSERT(string->chars == cc);
	CU_ASSERT(string->space == DYN_INIT_SIZE - 1);
	CU_ASSERT(!strcmp(string->data, ts));
	ds_free(string);
}

void test_dyn_str_5(void){
	char *ts = "Hallo";
	dyn_string_t *string = ds_get();
	ds_append_string(string, ts);
	ds_adapt_string(string);
	CU_ASSERT(string->chars == strlen(ts));
	CU_ASSERT(string->space == strlen(ts) + 1);
	CU_ASSERT(0 == strcmp(string->data, ts)); 
	ds_free(string);
}

void test_dyn_str_6(void){
	char *ts = "Hallo\n\n\n";
	dyn_string_t *string = ds_get();
	ds_append_string(string, ts);
	ds_remove_trailing_char(string, '\n');
	CU_ASSERT(string->chars == strlen("Hallo"));
	CU_ASSERT(string->space == strlen("Hallo") + 1);
	CU_ASSERT(0 == strcmp(string->data, "Hallo"));	
	printf("\n[test_dyn_str_6] String: %s\n", string->data);
	ds_free(string);	
}

void test_dyn_str_7(void){
	char *ts = "Hallo\n\b\a\n\n";
	dyn_string_t *string = ds_get_string(ts);
	ds_remove_trailing_whitespaces(string);
	CU_ASSERT(string->chars == 5);
	CU_ASSERT(string->space == 6);
	CU_ASSERT(0 == strcmp(string->data, "Hallo"));
	printf("\n[test_dyn_str_7] String: [%s]\n", string->data);
	ds_free(string);

	char *tts = "Hallo\nFO\b\a\n\n";
	string = ds_get_string(tts);
	ds_remove_trailing_whitespaces(string);
	CU_ASSERT(string->chars == 8);
	CU_ASSERT(string->space == 9);
	CU_ASSERT(0 == strcmp(string->data, "Hallo\nFO"));
	printf("\n[test_dyn_str_7] String: [%s]\n", string->data);
	ds_free(string);
}

int main(int argc, char **argv){
	CU_pSuite pSuite = NULL;

	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	pSuite = CU_add_suite("Dynamische wachsende Strings", dummy, dummy); 
	if (NULL == pSuite){
		CU_cleanup_registry();
		return CU_get_error();
	}

	if((NULL == CU_add_test(pSuite, 
			"String erzeugen und verwerfen", 
			test_dyn_str)) || 
		(NULL == CU_add_test(pSuite, 
			"String erzeugen, wachsen lassen und verwefen", 
			test_dyn_str_2)) ||
		(NULL == CU_add_test(pSuite, 
			"String erzeugen, mit einzelnen Zeichen befüllen und verwerfen", 
			test_dyn_str_3)) || 
		(NULL == CU_add_test(pSuite, 
			"String erzeugen, mit realistischem Text befüllen und verwerfen", 
			test_dyn_str_4)) ||
		(NULL == CU_add_test(pSuite, 
			"String erzeugen und nicht benötigten Speicher freigeben. Danach verwerfen", 
			test_dyn_str_5)) ||
		(NULL == CU_add_test(pSuite, 
			"String erzeugen und Newlines abschneiden; danach verwerfen", 
			test_dyn_str_6)) ||
		(NULL == CU_add_test(pSuite, 
			"String erzeugen und Whitespaces abschneiden; danach verwerfen", 
			test_dyn_str_7))
	){
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}

/*
 ---End Testcode---
 */
#endif
