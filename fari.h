typedef struct Element Element;
struct Element{
    char* mot;
    Element *suivant;
} ;

// structure de contrôle
typedef struct Liste{
	Element* premier;
}Liste;




typedef struct tableau tableau;
struct tableau{
	int a;
    int* tableauint;
    time_t x;
    
} ;

typedef struct Fari_data Fari_data;
struct Fari_data{
    Liste * source;
    Liste * headers;
    Liste * libraries;
    Liste * flags;
    char * executable_name;
    char * fari_error_msg;
    Liste * commands;
    char * error_msg0;
    char * command;
    char * error_msg1;
    char * fari_msg;
};

