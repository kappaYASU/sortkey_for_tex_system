/*
Sanskrit Sort key generator for TeX Index System.(3/5/2004)
Rewritten for utf-8 (4/16/2021)
 
This program is designed to generate sort key of Sanskrit language in TeX system.
This program presupposes TeX index system. If you have generated XX.idx file by TeX \makeindex command,
you can add  the sort codes to XX.idx file.
For example, if the following line, 
 
\indexentry {Ud\-dyo\-ta\-ka\-ra}{1}

is written in original .idx file (hyphenation mark "\-" and meaningless space in index entry will 
be neglected), my program will re-write as follows:

\indexentry {fututwrmurbrrbwsb@Ud\-dyo\-ta\-ka\-ra}{1}

"f" is a code of "u" and "ut" is that of "d". If you use Makeindex or other index program, you can get
XX.ind file, where the each entry has sort key in Sanskrit dictionary order. 

Besides sutra or karika numbers are transformed to be placed in ascending order as follows:
From \indexentry {Ny\={a}yas\={u}tra!1. 1. 12}{8} and \indexentry {Ny\={a}yas\={u}tra!1. 1. 2}{9}
To   \indexentry {uvwrcwrbxtgurwsb@Ny\={a}yas\={u}tra!001001012@1. 1. 12}{8} and
	\indexentry {uvwrcwrbxtgurwsb@Ny\={a}yas\={u}tra!001001002@1. 1. 2}{9}

In this conversion the original idx file is
remaining as XX.idx.org

The version with "sktcode.tbl" is written 3/21/2005

This program is written by Yasuhiro Okazaki Ph. D.
(Hiroshima Prefectural Takamiya Senior High School)

E-mail kappa-y@nn.iij4u.or.jp
 
This program is re-written for utf-8 2021/03/27
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#undef      Mac_Carbon
#undef      jissjis
#undef      DEBUG

struct char_data{
    unsigned char 	init_char;
    int	char_len;
    unsigned char  char_body[10];
    int	true_code;
    struct char_data*	next_nn;
    struct char_data*	next_dd;
};
typedef	struct char_data 	char_data;

struct code_entry {
unsigned char	org_string[512];
unsigned char			tobe_coded[256];
int			int_code[128];
unsigned char			op_code[256];
};

unsigned char		write_buf[1024];

typedef	struct code_entry 	code_entry;

const 	char* sort_key_code[] = {"b","c","d","e","f","g","h","i","j","k","l","m","n",\
			"rr","rs","rt","ru","rv","sr","ss","st","su","sv","tr","ts","tt","tu","tv","ur","us","ut","uu","uv",\
			"vr","vs","vt","vu","vv","wr","ws","wt","wu","xr","xs","xt","xu"};
char num_form[6];
int	numbering_flag;

code_entry*	temp_entry;
code_entry	entry_list[5];
code_entry*	make_enty( int depth );


FILE* i_file;
FILE* o_file;

char	letter;
char* mark_i ="indexentry";

#ifdef jissjis
static char _JISB[2][4] = {
  {0x1b,'$','B','\0'},
  {0x1b,'$','@','\0'}
};
static char _JISE[3][4] = {
  {0x1b,'(','J','\0'},
  {0x1b,'(','B','\0'},
  {0x1b,'(','H','\0'}
};
#define isjiss(s) \
((strncmp(s,_JISB[0],3)==0)||(strncmp(s,_JISB[1],3)==0))
#define isjise(s) \
((strncmp(s,_JISE[0],3)==0)||(strncmp(s,_JISE[1],3)==0)||(strncmp(s,_JISE[2],3)==0))

#endif

char_data*	read_data( void );
void			real_coding( unsigned char* bp, int list);
void			coding( int depth );
char_data*    make_data(char_data* start,int code_index, char* entry);
unsigned char*	skip_3byte( unsigned char* c);

int			code_index;
char_data		*start_data, *temp;
void		process(char lit);
code_entry*	make_entry(int depth);
void		usage( void );
int		already_flag, case_sensitive;
int		list_depth;
unsigned char*	write_code( char* str_buf, int list);
int count_byte( unsigned char c);

unsigned char*	write_code( char* str_buf, int list)
{
		int				i, num, num_bp, code_bp;
		unsigned char 		*real_entry;
		char				num_buf[10], num_buf_2[10];
		if(list) strcat(str_buf, "!");
		code_bp = 0;
		while(entry_list[list].int_code[code_bp] > -1)
		{
			if(entry_list[list].int_code[code_bp] < 46){
			strcat( str_buf, sort_key_code[entry_list[list].int_code[code_bp]]);
			}
			if(entry_list[list].int_code[code_bp] == 46){
			if(entry_list[list].int_code[code_bp+1] < 13)
			{printf("Irreagal Sanskrit word!\n");
			}
			else
			{
			if(entry_list[list].int_code[code_bp+1] < 18)
			strcat( str_buf, "rv");
			if(entry_list[list].int_code[code_bp+1] < 23 && entry_list[list].int_code[code_bp+1]  > 17)
			strcat( str_buf, "sv");
			if(entry_list[list].int_code[code_bp+1] < 28 && entry_list[list].int_code[code_bp+1]  > 22)
			strcat( str_buf, "tv");
			if(entry_list[list].int_code[code_bp+1] < 33 && entry_list[list].int_code[code_bp+1]  > 27)
			strcat( str_buf, "uv");
			if(entry_list[list].int_code[code_bp+1] < 38 && entry_list[list].int_code[code_bp+1]  > 32)
			strcat( str_buf, "vv");
			if(entry_list[list].int_code[code_bp+1] < 46 && entry_list[list].int_code[code_bp+1]  > 37)
			strcat( str_buf, "p");
			if(entry_list[list].int_code[code_bp+1] == 49)
			strcat( str_buf, "vv" );
			}
			}
			if(entry_list[list].int_code[code_bp] == 47){
			if(entry_list[list].int_code[code_bp+1] == 49){
			strcat( str_buf, "xt" );}
			else{
			strcat( str_buf, "q");}
			}
			if(entry_list[list].int_code[code_bp] == 48)
			strcat( str_buf, "uv");
			if(entry_list[list].int_code[code_bp] >= 100 && entry_list[list].int_code[code_bp] <= 109){
			num_buf_2[0] = '0' + (entry_list[list].int_code[code_bp] -100);
			num = 1;
			code_bp++;
			while(entry_list[list].int_code[code_bp] >= 100 && entry_list[list].int_code[code_bp] >= 0)
			{
				num_buf_2[num] = '0' + (entry_list[list].int_code[code_bp] - 100);
				num++;
				code_bp++;
			}
			num_buf_2[num] = '\0';
			num_bp = numbering_flag - num;
			for(i=0; i < num_bp; i++) num_buf[i] = '0';
			num_buf[i] = '\0';
			strcat( str_buf, num_buf);
			strcat( str_buf, num_buf_2);
			}
			else
			{
			code_bp++;
			}
			
		}
		real_entry = entry_list[list].org_string;
		if(*real_entry == '!') real_entry++;

		strcat(str_buf, "@");
		strcat(str_buf, (char*)real_entry);
		
		return	(unsigned char*)str_buf;
}
#define NON_CODE_S 52
#define NON_CODE_E 53
		
void			real_coding( unsigned char* bp, int list)
{
	int			*code_p;
    int         coding_flag;
	char_data		*data_t;
	code_p = entry_list[list].int_code;
    coding_flag = 1;
		while(*bp != '\0')
		{
			if(numbering_flag && *bp >= '0' && *bp <= '9'){
			*code_p = 100 + (*bp - '0');
			code_p++;
			bp++;
			goto	find_out;
			}
            // coding_flag = 1;
			data_t = start_data;
			while(data_t->next_nn != NULL)
			{
				if(*bp == data_t->init_char){
				while(data_t != NULL)
				{
					if(!strncmp((const char*)bp, (const char*)data_t->char_body,data_t->char_len)){
                        if(data_t->true_code == NON_CODE_S){
                            coding_flag = 0;
                            bp += data_t->char_len;
                            goto find_out;
                        }
                        if(data_t->true_code == NON_CODE_E){
                            coding_flag = 1;
                            bp += data_t->char_len;
                            goto find_out;
                        }
                        if(!coding_flag){
                            bp += data_t->char_len;
                            goto find_out;
                        }
					*code_p = data_t->true_code;
					bp += data_t->char_len;
					code_p++;
					goto	find_out;
					}
				data_t = data_t->next_dd;
				}
				bp++;
				goto	find_out;
				}
				data_t = data_t->next_nn;
			}
			bp++;
	find_out:	bp = bp;		
		}
		*code_p =  -1;
}	
unsigned char* skip_3byte(unsigned char* c)
{
    int k_flag;
    k_flag = count_byte(*c);
    if(k_flag > 3){
        c++;
        c++;
    }
    return c;
}
#ifdef jissjis
unsigned char* skip_2byte(unsigned char* s_ptr)
{
	if(*s_ptr >=0xA1) {
	s_ptr++;
	return s_ptr;
	}
	if(*s_ptr <= 0x9F && 0x81 <= *s_ptr){
	s_ptr++;
	if(*s_ptr < 0x40 ) printf("It is pseudo-kanji\n");
	s_ptr++;
	return s_ptr;
	}
	if(isjiss( (const char*)s_ptr )){
	s_ptr += 3;
	while(!isjise((const char*)s_ptr))
	{
		s_ptr += 2;
	}
	s_ptr += 3;
	return s_ptr;
	}
	return s_ptr;
}
#endif
code_entry*	make_entry(int depth)
{
	code_entry*	new_entry;
	new_entry = &entry_list[depth];
	return new_entry;
}
void	coding( int depth)
{
	int	list, char_len, k_flag ;
	unsigned char *temp_p;
    unsigned char *buf_p;
    unsigned char temp_char[4];
	list = 0;
	while(list <= depth)
	{
	temp_p = entry_list[list].org_string;
	buf_p = entry_list[list].tobe_coded;
		while(*temp_p != '\0')
		{
            while((k_flag = count_byte(*temp_p))>3){
                temp_p++;
                temp_p++;
                temp_p++;
            }
			if(0x20 <= *temp_p && *temp_p < 0x7F){
			if(*temp_p == ' '){
			if(*(temp_p+1) == '{' || *(temp_p+1) == '}')
			temp_p++;
			}
			if( *temp_p == '|' && list == depth) break;
                if(!case_sensitive && *temp_p >= 'A' && *temp_p <= 'Z'){
                    *buf_p = tolower(*temp_p);
                }
                else
                {
                    *buf_p = *temp_p;
                }
                buf_p++;
            }
            else
            {
#ifdef DEBUG
                if((char_len = count_byte(*temp_p))>0){
                    if(char_len >= 3){
                        memset(temp_char, '\0',4);
                        strncpy((char*)temp_char, (char*)temp_p, 3);
                        printf("%s: %2x%2x%2x, The number of character bytes: %d\n", temp_char, *temp_p&0xff,temp_p[1]&0xff,temp_p[2]&0xff,char_len);
                    }
                    if(char_len == 2){
                        memset(temp_char, '\0',4);
                        strncpy((char*)temp_char, (char*)temp_p, 2);
                        printf("%s: %2x%2x, The number of character bytes: %d\n", temp_char, *temp_p&0xff,temp_p[1]&0xff,char_len);
                    }
                }
#endif
            *buf_p = *temp_p;
            buf_p++;
            }
            temp_p++;
		}
		*buf_p = '\0';
	list++;
	}
	list = 0;
	while( list <= depth)
	{
	buf_p = entry_list[list].tobe_coded;
		if(*buf_p == '!' ) {
			buf_p++;
			}
		real_coding(buf_p, list);
		list++;
	}
	strcpy((char*)write_buf, "{");
	list = 0;
	buf_p = write_buf;
	while( list <= depth)
	{
	buf_p  = write_code((char*)buf_p, list);
	list++;	
	}
	fputs((char*)buf_p, o_file);
	putc('}', o_file);	
	
}
		
		
	
void	process( char lit)
{

    char	tex_com[32];//, esc_buf[5]
    int	i, nest, k_flag, jis_flag; //esc_i, ;
	unsigned char	c;
	i = 0;
	putc(lit, o_file);
	while((c = getc(i_file)) != '{' )
	{
		putc(c, o_file);
		if(c != ' '){
		tex_com[i] = c;
		i++;
		}
	}
	tex_com[i] = '\0';
	if((i = strcmp(tex_com, mark_i)) == 0)
	{
		i = 0;
		nest = 1;
		already_flag = 0;
		list_depth=0;
		//jis_flag = 0;
		temp_entry = make_entry( list_depth);
        while(1){
			c = (unsigned char)getc(i_file);
            jis_flag = 0;
#ifdef jissjis
			if( c <= 0x9F && 0x81 <= c ){
			temp_entry->org_string[i] = c;
			i++;
			c = (unsigned char)getc(i_file);
			temp_entry->org_string[i] = c;
			i++;
			}
#endif
            while((k_flag = count_byte(c)) > 3){//for 3byte-code of kanji or
                jis_flag = 1;
                temp_entry->org_string[i] = c;
                i++;
                c = (unsigned char)getc(i_file);
                temp_entry->org_string[i] = c;
                i++;
                c = (unsigned char)getc(i_file);
                temp_entry->org_string[i] = c;
                i++;
              //  c = (unsigned char)getc(i_file);
            }
			switch( c ){
				case '{':	nest = nest + 1;
						break;
				case '}':	nest = nest - 1;
						break;
				case '@':	already_flag = 1;
						break;
					}
				if(nest == 0) {
				temp_entry->org_string[i] = '\0';
					break;
				}
				if(i>511) break;	
				if(c == '!' && nest == 1){
						list_depth++;
						temp_entry->org_string[i] = '\0';
						temp_entry = make_entry( list_depth );
						temp_entry->org_string[0] = '!';					
						i = 1;
				}
				else
				{
                    if(jis_flag){
                        jis_flag = 0;
                    }
                    else
                    {
						temp_entry->org_string[i] = c;
						i++;
                    }
				}
            
        }
		
		if(already_flag){
		printf("This line already has sort key: %s\n", temp_entry->org_string);
		putc('{', o_file);
		for(i=0;i<=list_depth;i++)
		fputs((const char*)entry_list[list_depth].org_string, o_file);
		putc('}', o_file);
		already_flag = 0;
        }
		else
		{
		coding(list_depth);
		}
    
    }
	else
	{
	putc('{', o_file);
	}
}
// Generate code list
char_data*	make_data(char_data* start,int code_index, char* entry)
{
	int	len;
	unsigned char	init_c;
	char_data		*root_temp, *prev_temp;	
	temp = malloc(sizeof(char_data));
	if(temp == NULL){
	printf("No memory!!\n");
	return NULL;
	}
	len = (int)strlen( entry );
	init_c = entry[0];
	temp->char_len = len;
	strcpy((char*)temp->char_body, entry);
	temp->init_char = init_c;
	temp->true_code = code_index;
	temp->next_nn = NULL;
	temp->next_dd = NULL;
	if(start == NULL) return temp;
	root_temp = start;
	prev_temp = NULL;
	while(root_temp)
	{

		if(root_temp->init_char == init_c){
			if(root_temp->char_len < len){
			if(prev_temp == NULL){
			temp->next_dd = root_temp;
			temp->next_nn = root_temp->next_nn;
			root_temp->next_nn = NULL;
			return temp;
			}
			prev_temp->next_nn = temp;
			temp->next_nn = root_temp->next_nn;
			temp->next_dd = root_temp;
			root_temp->next_nn = NULL;
			return start;
			}
			prev_temp = root_temp;
			root_temp = root_temp->next_dd;
		while(root_temp)
		{
			if(root_temp->char_len < len){
			prev_temp->next_dd = temp;						
			temp->next_dd = root_temp;			
			return start;
			}
			prev_temp = root_temp;
			root_temp = root_temp->next_dd;
		}
		prev_temp->next_dd = temp;
		return start;
		}
		prev_temp = root_temp;
		root_temp = root_temp->next_nn;
	}
	prev_temp->next_nn = temp;
	return start;	
}

int count_byte( unsigned char c)
{
    int cp = (int)c;
    if((cp & 0xf0) == 0xe0 ){
      if((cp & 0xff) >= 0xe3)return 6;
        return 3;}
    if((cp & 0xe0) == 0xc0 ) return 2;
    if((cp & 0xc0) == 0x80 ) return 1;
    if((cp & 0x80) == 0) return 0;
    return 4;
}
char_data*	read_data( void )
{
	FILE*		data_file;
    char    letters[256];
    char*   letter;
#ifdef DEBUG
    char    temp_char[4];
    int     char_len;
#endif
	char  temp_buf[16];
	int	comment_flag,  buf_i, case_flag, read_flag;
	data_file = fopen("sktcode.tbl", "r");
	if(data_file == NULL){
	printf("Can not open character data file named \"sktcode.tbl\"\n");
	return 	NULL;
	}
	start_data = NULL;
	code_index = -1;
	comment_flag = 0;
	case_sensitive = 0;
	case_flag = -1;
	read_flag = 0;
	buf_i = -1;
    letter = letters;
	while((fgets(letter,256, data_file))!= NULL)
	{
        while(*letter!= '\0'){
            if(comment_flag){comment_flag = 0;
                break;
            }
            if(*letter == '\r' || *letter == '\n'){
                if(buf_i > 0){
                temp_buf[buf_i] = '\0';
                start_data = make_data( start_data ,code_index, temp_buf);
                letter = letters;
                buf_i = -1;
                read_flag = 0;
                    break;
                }
            }
		switch(*letter)
		{
		case '%':	comment_flag = 1;
				break;
		case '!':	if(!comment_flag){
				buf_i = -1;
				read_flag = 1;
				code_index++;
				}
                letter++;
				break;
		case '\t':	if(comment_flag) break;
			if(read_flag){
				if(buf_i <= 0){
				buf_i = 0;
				}
				else{
				temp_buf[buf_i] = '\0';
				start_data = make_data( start_data ,code_index,  temp_buf);
				buf_i = 0;
				}
			}
                letter++;
				break;
		case '#':	if(comment_flag == 0){
				if(case_flag < 0) case_flag = 0;
				}
                letter++;
				break;
		case 'Y':
		case 'y':	if(case_flag == 0) {
				case_sensitive = 1; 
				case_flag = 1;
                letter++;
				break;
				}
		case 'n':
		case 'N':	if(case_flag == 0){
				case_sensitive = 0;
				case_flag = 1;
                letter++;
				break;
				}		
		default:	if(comment_flag) break;
				if(buf_i >= 0 && read_flag ){
				temp_buf[buf_i] = *letter;
				buf_i++;
				}
#ifdef DEBUG
                char_len = count_byte(*letter);
                if(char_len >= 3){
                    memset(temp_char, '\0',4);
                    strncpy(temp_char, letter, 3);
                    printf("%s: %2x%2x%2x, The number of character bytes: %d\n", temp_char, *letter&0xff,letter[1]&0xff,letter[2]&0xff,char_len);
                }
                if(char_len == 2){
                    memset(temp_char, '\0',4);
                    strncpy(temp_char, letter, 2);
                    printf("%s: %2x%2x, The number of character bytes: %d\n", temp_char, *letter&0xff,letter[1]&0xff,char_len);
                }
                if(char_len == 1){
                    printf("Low-byte of multibyte-code\n");
                }
#endif
                letter++;
				break;
		}
            
        }
	}
	fclose(data_file);
		return start_data;
}				
#ifndef Mac_Carbon
void		usage( void )
{
	printf("The usage of this program is as below.\n\n");
	printf("sktsk [option -h/-n] [numbering form \"###\"] input-file-name\n\n");
	printf("0ption switch \"-h\" will tell you the usage of this program. \n");
	printf("0ption switch \"-n\" will be useful, in the case when your index entry\n");
	printf("includes numbers, for example, karika number or sutra number. \n");
	printf("In ordinal processing by index application, k. 112 will be placed before k. 5.\n");
	printf("To prevent such a case, k. 5 must be transformed to k. 005\n");
	printf("This option switch makes the sort key \"0...0nn\" from \"nn\" in index field.\n");
	printf("You indicates the highest figure in index field by the number of \"#.\" \n");
	printf("In the case where the highest figure is 128, and the input file \"skt.idx,\"\n");
	printf("you type as follows:\n\n");
	printf("sksk -n ### skt.idx\n\n");
	printf("If you type \"sktsk -n skt.idx\", it is the same as\n");
	printf("\"sktsk -n #### skt.idx\". This form will cover all cases when you need numbering\n\n");
	printf("I hope your happy texing and happy sanskrit study. Thank you.\n");
	printf("Yasuhiro Okazaki. March 19th 2005\n");
}
#endif
#ifdef Mac_Carbon
int	sktmain( char*  input_file_name)
{
/*	char_data*	temp_t;
	int			num; *//*This code for Debug*/

#else
int main(int argc, char* argv[])
{
	char input_file_name[64];
#ifdef DEBUG
	char_data*	temp_t;
	int			num;
	FILE *d_f;/*This code for Debug*/
    char next_nn_char[12];/*This code for Debug*/
    char next_dd_char[12];/*This code for Debug*/
#endif
#endif
	char org_file_name[255];
	numbering_flag = 0;
	strcpy(num_form, "###");
//	strcpy(input_file_name, "doctor.pdx"); /*This code for Debug*/
//	strcpy(org_file_name, input_file_name); /*This code for Debug*/
    //char debug_file[] = "debug.sdx";/*This code for Debug*/
    printf("Sanskrit Sort Code Generator for TeX Index System\n  Copyright (C) 2021  Yasuhiro Okazaki\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions\n");
#ifdef Mac_Carbon
	strcpy(org_file_name, input_file_name);
#else
switch( argc ){
	case 1:	printf("Please type Input file as \"sktsk ???.idx, etc.,.\"\n Or \"sktsk -h\" and you will know how to use\n");
			return 0;
	case 2:	if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "-H")){
			usage();
			return 0;
			}
			strcpy(input_file_name, argv[1]);
			strcpy(org_file_name, input_file_name);
			break;
	case 3:	if(!strcmp(argv[1], "-n") || !strcmp(argv[1], "-N")){
			strcpy(num_form, "####");
			strcpy(input_file_name, argv[2]);
			strcpy(org_file_name, input_file_name);			
			break;
			}
	case 4:	if(!strcmp(argv[1], "-n") || !strcmp(argv[1], "-N")){
			strcpy(num_form, argv[2]);
			strcpy(input_file_name, argv[3]);
			strcpy(org_file_name, input_file_name);			
			break;
			}
	default:	usage();
			return 0;
	}
#endif

	strcat(org_file_name, ".org");
	printf("Original file is conserved as %s\n", org_file_name);
	if(rename(input_file_name, org_file_name))
	{printf("Can not open the input file!\n");
	return 0;
	}
	i_file = fopen(org_file_name, "r");
	if(i_file == NULL){
	printf("Can not open the input file!\n");
	return 0;
	}
	o_file = fopen(input_file_name, "w");
    /*For debug */ //  o_file = fopen(debug_file, "w");
	if(o_file == NULL){
	printf("Can not open the output file!\n");
	return 0;
	}
#ifdef Mac_Carbon
	printf("Do you use numbering form in your index? If you use it, you write numbering form as follows\n");
	printf("\"###\" means \"001\" and,  \"####\" does \"0001\"");
	scanf("%s", num_form);
#endif	
	if(num_form[0] == '#'){
	numbering_flag = 1;
	while(num_form[numbering_flag] != '\0'){
	numbering_flag++;
	}
	}
	start_data = read_data();
	if(start_data == NULL) return 0;	
#ifdef DEBUG
    temp = start_data;
	d_f = fopen("debug.result", "w");
	num = 1;
	while(temp){
	temp_t = temp;
	fprintf(d_f, "%d :", num);
	while(temp_t){
        if(temp_t->next_dd == NULL){
            strcpy(next_dd_char, "NULL");
        }
        else
        {
            strcpy((char*)next_dd_char, (char*)temp_t->next_dd->char_body);
        }
        if(temp_t->next_nn == NULL){
            strcpy(next_nn_char, "NULL");
        }
        else
        {
            strcpy((char*)next_nn_char, (char*)temp_t->next_nn->char_body);
        }
			fprintf(d_f, "Initchar %2x:%s:length:%d Code:%d; Nextdd:%s,Nextnn:%s \n ",temp->init_char&0xff, temp_t->char_body,temp_t->char_len, temp_t->true_code, next_dd_char,next_nn_char);
	temp_t = temp_t->next_dd;
	}
	fprintf(d_f, "\n\n\n");
	num++;
	temp = temp->next_nn;
	}
if(case_sensitive){fprintf(d_f,"Case sensitive On\n");
}
else
{fprintf(d_f, "Case sensitive Off\n");
	}
	fclose(d_f);
#endif
	while((letter = getc(i_file)) != EOF)
	{
	if(letter == '\\'){
		process(letter);
		}
		else
		{
		putc(letter, o_file);
 		}
	}
	fclose(i_file);
	fclose(o_file);
	printf("All process has done\n");
	return 0;
}
