#include <stdio.h>
#include "error.h"
#include "ast.h"
#include <string.h>
#include <ctype.h>


#define OUTPUT_CODE_LINE(code) \
    printf("%s\n",code)

#define OUTPUT_CODE_PART(code) \
    printf("%s",code)

#define EMPTY_LINE \
    printf("\n")

#define COMMENT(comm) \
    printf("#%s\n",comm)

void exponent_float_to_integer(){
    OUTPUT_CODE_LINE("LABEL FLOAT_TO_INT_EXPONENT");
    OUTPUT_CODE_LINE("POPS GF@result");

    OUTPUT_CODE_LINE("PUSHS GF@RESULT");
    OUTPUT_CODE_LINE("RETURN");

}

/* GF@loop_iterator
LABEL EXPONENTIATION
POPS GF@exponent
POPS GF@base

TYPE GF@type1 GF@base
TYPE GF@type2 GF@exponent

JUMPIFEQ EXPONENT_FLOAT string@float GF@type2   if e is float (unsupported)

JUMPIFEQ FLOAT_BASE string@float GF@type1       if base is float, dont convert to float, otherwise convert it if it is integer
INT2FLOAT gf@op1 gf@op1

LABEL FLOAT_BASE
JUMPIFEQ EXP_ZERO GF@exponent float@0x0p+0             if e == 0 in b^e
LT GF@stackresult GF@exponent int@0                   // If exponent is smaller than zero, stackresult is true
JUMPIFEQ POS_EXPONENT GF@stack_result bool@false      // Exponent is positive
MUL GF@exponent int@-1                                // Negative exponent is turned to positive
LABEL POS_EXPONENT


MOVE GF@result GF@base
SUB GF@exponent GF@exponent int@1                       if exponent is 3, i only want to multiply the original value 2 times.
PUSHS GF@result

LABEL EXP_LOOP_START

MOVE GF@loop_iterator int@0                            for (int i = 0;...
PUSHS GF@exponent
MULS
ADD GF@loop_iterator int@1                              i++;)
JUMPIFEQ EXP_LOOP_END GF@loop_iterator GF@exponent      ...i<(base i - 1);
JUMP EXP_LOOP_START


LABEL EXPONENT_FLOAT
EXIT int@6


LABEL EXP_ZERO
JUMPIFEQ ZERO_ZERO GF@base int@0    0^0 is invalid
MOVE GF@result int@1                a^0 where a != 0 is equal to 1.
RETURN


LABEL ZERO_ZERO  
EXIT int@6

*/


/*
FOR
BREAK
^

MOCNINA a^b
stack = a, b (napravo je top); a*a*a*...*a b-times
pops GF@exponent
pops GF@base

if base == 0 and exponent == 0:
    EXIT int@8

if base!=0 and exponent == 0 : 
    move GF@result int@1; 
    pushs GF@result

if exponent>0: 
    move GF@result GF@base;
    PUSH GF@result ;; stack = 4
    for (int i = 0; i<b):
        PUSH GF@base     stack = 4, 4 ;;; stack = 16,4
        MULS;            stack = 16   ;;;

4^2
if exponent<0:
    move GF@result GF@base;
    PUSH GF@result ;; stack = 4
    for (int i = 0; i<GF@base):
        PUSH GF@base     stack = 4, 4 ;;; stack = 16,4
        MULS;            stack = 16   ;;;
    POPS GF@result       stack = empty;
    PUSHS int@1          stack = 1
    PUSHS GF@result      stack = 16
    DIVS                 stack = 1/16

*/

#include <stdio.h>
#include "ctype.h"

void look_for_declarations(ast_node_t *root);

void process_string(char *s)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i] <= 32)
        {
            printf("\\%03d", (int)s[i]);
        }
        else if (s[i] == '#')
        {
            printf("\\035");
        }
        else if (s[i] == '\\')
        {
            printf("\\092");
        }
        else
        {
            printf("%c", s[i]);
        }
        i++;
    }
    printf("\n");
}

static int global_func_counter=0;

void process_binop_node(ast_node_t *binop_node);
void process_unop_node(ast_node_t *unop_node);
void process_node(ast_node_t *cur_node);
void generate_result();

bool get_id_name(symbol_t * node_symbol, char ** name, char ** suffix){
    if (node_symbol->is_declaration){
        *name = node_symbol->name.ptr;
        *suffix = node_symbol->suffix.ptr;
        return true;

    }
    else{
        return get_id_name(node_symbol->declaration,name,suffix);
    }
}

int count_children(ast_node_list_t children_list){
    ast_node_t *first = children_list;
    int counter=0;
    while (first!=NULL){
        counter++;
        first=first->next;
    }
    return counter;
}


// FUNC CALL GENERATING

void generate_func_call_argument_push(int i){
    OUTPUT_CODE_PART("DEFVAR TF@%"); printf("%d\n",i);
    OUTPUT_CODE_PART("MOVE TF@%"); printf("%d ",i);
}

void push_integer_arg(uint64_t integer){
    printf("int@%ld\n",integer);
}

void push_number_arg(double number){
    printf("float@%a\n",number);
}

void push_bool_arg(bool boolean){
    if (boolean==1){
        printf("bool@true\n");
    }
    else{
        printf("bool@false\n");
    }
}

void push_string_arg(char * string){
    OUTPUT_CODE_PART("string@"); process_string(string); OUTPUT_CODE_LINE("");
}

void push_id_arg(symbol_t * symbol){

    char * name;
    char * suffix;
    get_id_name(symbol,&name,&suffix);

    printf("LF@%s%%%s\n",name,suffix); 
}
void push_nil_arg(){
    printf("nil@nil\n");
}

void check_nil_write(){
        OUTPUT_CODE_LINE("LABEL $nil_write");
        OUTPUT_CODE_LINE("POPS GF@op1");
        OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");
        OUTPUT_CODE_LINE("JUMPIFEQ IS_NIL string@nil GF@type1");
        OUTPUT_CODE_LINE("WRITE GF@op1");
        OUTPUT_CODE_LINE("JUMP END_WRITE");
        OUTPUT_CODE_LINE("LABEL IS_NIL");
        OUTPUT_CODE_LINE("WRITE string@nil");
        OUTPUT_CODE_LINE("LABEL END_WRITE");
        OUTPUT_CODE_LINE("PUSHS GF@op1");
        OUTPUT_CODE_LINE("RETURN");
}

void generate_write(int arg_count){
    for(int i = 0;i<arg_count;i++){
        OUTPUT_CODE_PART("PUSHS TF@%");printf("%d\n",i);
        OUTPUT_CODE_LINE("CALL $nil_write");
        OUTPUT_CODE_PART("POPS TF@%");printf("%d\n",i);
    }
}

//def func (a, b)
// func(foo());
void process_node_func_call(ast_node_t *cur_node)
{   
    OUTPUT_CODE_LINE("CREATEFRAME");
    int arg_count = count_children(cur_node->func_call.arguments);
    ast_node_t *cur_argument = cur_node->func_call.arguments;
    for (int i = 0;i<arg_count;i++){
        switch(cur_argument->node_type){
            case AST_NODE_INTEGER:
                generate_func_call_argument_push(i);
                push_integer_arg(cur_argument->integer);
                break;
            case AST_NODE_NUMBER:
                generate_func_call_argument_push(i);
                push_number_arg(cur_argument->number);
                break;
            case AST_NODE_BOOLEAN:
                generate_func_call_argument_push(i);
                push_bool_arg(cur_argument->boolean);
                break;
            case AST_NODE_SYMBOL:
                generate_func_call_argument_push(i);
                push_id_arg(&cur_argument->symbol);
                break;
            case AST_NODE_STRING:
                generate_func_call_argument_push(i);
                push_string_arg(cur_argument->string.ptr); 
                break;
            case AST_NODE_NIL:
                generate_func_call_argument_push(i);
                push_nil_arg();
                break;
            case AST_NODE_FUNC_CALL:
                process_node_func_call(cur_argument); //TODO Func call v returne
                int children = count_children(cur_argument->func_call.arguments);
                for(int k = 0; k< children;k++){
                    generate_func_call_argument_push(i); printf("TF@retval%d\n",k);
                    i++;
                }
                break;
            case AST_NODE_UNOP:
                process_unop_node(cur_argument);
                OUTPUT_CODE_LINE("POPS GF@result");
                generate_func_call_argument_push(i);
                generate_result();
                break;
            case AST_NODE_BINOP:
                process_binop_node(cur_argument);
                OUTPUT_CODE_LINE("POPS GF@result");
                generate_func_call_argument_push(i);
                generate_result();
                break;
        }
        cur_argument=cur_argument->next;
    }
    //if not write
    if (strcmp(cur_node->func_call.name.ptr,"write")){
        OUTPUT_CODE_PART("CALL $"); printf("%s\n",cur_node->func_call.name.ptr);
    }
    else{
        generate_write(arg_count);
    }
}

// FUNC CALL GENERATING END



//FUNC DEF GENERATING START
void generate_func_start(char*function_name)
{
    OUTPUT_CODE_PART("LABEL $"); OUTPUT_CODE_LINE(function_name);
    OUTPUT_CODE_LINE("PUSHFRAME");
}

void generate_func_arg(symbol_t *symbol,int i){
    char *id;
    char *suffix;
    get_id_name(symbol,&id,&suffix);
    OUTPUT_CODE_PART("DEFVAR LF@"); printf("%s%%%s\n",id,suffix);
    OUTPUT_CODE_PART("MOVE LF@");  printf("%s%%%s ",id,suffix); OUTPUT_CODE_PART("LF@%"); printf("%d\n",i);
}

void generate_func_retval_dec(int i){
    OUTPUT_CODE_PART("DEFVAR LF@retval"); printf("%d\n",i);
    OUTPUT_CODE_PART("MOVE LF@retval"); printf("%d ",i); OUTPUT_CODE_LINE("nil@nil");
}


bool generate_func_end(char *function_name)
{
    OUTPUT_CODE_PART("LABEL $"); OUTPUT_CODE_PART(function_name); OUTPUT_CODE_LINE("%end");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
    EMPTY_LINE;
}


void process_node_func_def(ast_node_t *cur_node){
    //printf("Processing func def node.\n");
    generate_func_start(cur_node->func_def.name.ptr);
    //printf("generated head\n");

    ast_node_t *arg = cur_node->func_def.arguments;
    int arg_counter = 0;
    while (arg!=NULL){
        generate_func_arg(&arg->symbol,arg_counter);
        arg=arg->next;
        arg_counter++;
    }

    int retval_counter = 0;
    ast_node_t *retval_type = cur_node->func_def.return_types;
    while (retval_type!=NULL){
        generate_func_retval_dec(retval_counter);
        retval_type=retval_type->next;
        retval_counter++;
    }

    look_for_declarations(cur_node->func_def.body);
    process_node(cur_node->func_def.body);

    generate_func_end(cur_node->func_def.name.ptr);
    global_func_counter++;

}

//FUNC DEF GENERATING END

void ret_integer_arg(uint64_t integer){
    printf("int@%ld\n",integer);
}

void ret_number_arg(double number){
    printf("float@%a\n",number);
}

void ret_bool_arg(bool boolean){
    if (boolean==1){
        printf("bool@true\n");
    }
    else{
        printf("bool@false\n");
    }
}

void ret_string_arg(char * string){
    OUTPUT_CODE_PART("string@"); process_string(string); OUTPUT_CODE_LINE("");
}

void ret_id_arg(symbol_t * symbol){
    char * name;
    char * suffix;
    get_id_name(symbol,&name,&suffix);
    printf("LF@%s%%%s\n",name,suffix);

}

void ret_nil_arg(){
    printf("nil@nil\n");
}


void generate_func_def_retval_assign(int i){
    OUTPUT_CODE_PART("MOVE LF@retval"); printf("%d ",i);
}

void ret_binop_arg(){
    OUTPUT_CODE_LINE("POPS GF@result");
}


void process_binop_node(ast_node_t *binop_node);

void process_unop_node(ast_node_t *unop_node){
    switch(unop_node->unop.type){
        case AST_NODE_UNOP_LEN:
            process_binop_node(unop_node->unop.operand);
            OUTPUT_CODE_LINE("POPS GF@result");
            OUTPUT_CODE_LINE("STRLEN GF@result GF@result");
            OUTPUT_CODE_LINE("PUSHS GF@result");
            break;
        case AST_NODE_UNOP_NOT:
            //todo
            break;
        case AST_NODE_UNOP_NEG:
            //todo
            break;
    }
}

void process_binop_node(ast_node_t *binop_node){
    if (binop_node->node_type == AST_NODE_BINOP){
        process_binop_node(binop_node->binop.left);
        process_binop_node(binop_node->binop.right);
    }
    else{
        switch(binop_node->node_type){
            case AST_NODE_UNOP:
                process_unop_node(binop_node);
                break;
            case AST_NODE_INTEGER:
                OUTPUT_CODE_PART("PUSHS ");        
                ret_integer_arg(binop_node->integer);
                break;
            case AST_NODE_NUMBER:
                OUTPUT_CODE_PART("PUSHS ");  
                ret_number_arg(binop_node->number);
                break;
            case AST_NODE_BOOLEAN:
                OUTPUT_CODE_PART("PUSHS ");  
                ret_bool_arg(binop_node->boolean);
                break;
            case AST_NODE_SYMBOL:
                OUTPUT_CODE_PART("PUSHS ");  
                ret_id_arg(&(binop_node->symbol));
                break;
            case AST_NODE_STRING:
                OUTPUT_CODE_PART("PUSHS ");  
                ret_string_arg(binop_node->string.ptr);
                break;
            case AST_NODE_NIL:
                OUTPUT_CODE_PART("PUSHS ");  
                ret_nil_arg();
                break;
            case AST_NODE_FUNC_CALL:
                process_node_func_call(binop_node);
                OUTPUT_CODE_LINE("PUSHS TF@retval0");
                break;
        }
        return;
    }
    switch(binop_node->binop.type){
        case AST_NODE_BINOP_ADD:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("ADDS");
            break;
        case AST_NODE_BINOP_SUB:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("SUBS");
            break;
        case AST_NODE_BINOP_MUL:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("MULS");
            break;
        case AST_NODE_BINOP_DIV:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("CALL $float_zerodivcheck");
            OUTPUT_CODE_LINE("DIVS");
            break;
        case AST_NODE_BINOP_INTDIV:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $int_zerodivcheck");
            OUTPUT_CODE_LINE("IDIVS");
            break;
        case AST_NODE_BINOP_MOD:
            //MOD
            /*
            A%B = E
     
            A//B = C
            D = C*B
            E = A-D
            E = A - (A//B)*B
     
            */
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $int_zerodivcheck");
            OUTPUT_CODE_LINE("POPS GF@op2");
            OUTPUT_CODE_LINE("POPS GF@op1"); //Saving A and B

            OUTPUT_CODE_LINE("PUSHS GF@op1");
            OUTPUT_CODE_LINE("PUSHS GF@op2");//Pushing them back (but we know their values now)
                                             //Stack top is on the left.
            OUTPUT_CODE_LINE("IDIVS");       //Stack = (A//B)
            OUTPUT_CODE_LINE("PUSHS GF@op2");//Stack = B, (A//B)
            OUTPUT_CODE_LINE("MULS");        //Stack = B*(A//B) 
            OUTPUT_CODE_LINE("POPS GF@op2"); //Stack = --; GF@op2 =  B*(A//B) 
            OUTPUT_CODE_LINE("PUSHS GF@op1");//Stack = A
            OUTPUT_CODE_LINE("PUSHS GF@op2");//Stack = B*(A//B), A
            OUTPUT_CODE_LINE("SUBS");        //Stack = A-B*(A//B)
            break;
        case AST_NODE_BINOP_POWER:
            //todo
            break;
        case AST_NODE_BINOP_LT:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("LTS");
            break;
        case AST_NODE_BINOP_GT:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("GTS");
            break;
        case AST_NODE_BINOP_LTE:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("GTS");
            OUTPUT_CODE_LINE("NOTS");
            break;
        case AST_NODE_BINOP_GTE:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("LTS");
            OUTPUT_CODE_LINE("NOTS");
            break;
        case AST_NODE_BINOP_EQ:
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("EQS");
            break;
        case AST_NODE_BINOP_NE:
            OUTPUT_CODE_LINE("CALL $CONV_CHECK");
            OUTPUT_CODE_LINE("EQS");
            OUTPUT_CODE_LINE("NOTS");
            break;
        case AST_NODE_BINOP_AND:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("ANDS");
            break;
        case AST_NODE_BINOP_OR:
            OUTPUT_CODE_LINE("CALL $NIL_CHECK");
            OUTPUT_CODE_LINE("ORS");
            break;
        case AST_NODE_BINOP_CONCAT:
            OUTPUT_CODE_LINE("POPS GF@string1");
            OUTPUT_CODE_LINE("POPS GF@string0");
            OUTPUT_CODE_LINE("CONCAT GF@result GF@string0 GF@string1");
            OUTPUT_CODE_LINE("PUSHS GF@result");
            break;
    }
}

void generate_result(){
    printf("GF@result\n");
}

void process_return_node(ast_node_t *return_node){
    int retval_counter1 = count_children(return_node->return_values.values);
    ast_node_t *cur_retval =return_node->return_values.values;
    for (int i = 0;i<retval_counter1;i++){
        switch(cur_retval->node_type){
            case AST_NODE_INTEGER:
                generate_func_def_retval_assign(i);
                ret_integer_arg(cur_retval->integer);
                break;
            case AST_NODE_NUMBER:
                generate_func_def_retval_assign(i);
                ret_number_arg(cur_retval->number);
                break;
            case AST_NODE_BOOLEAN:
                generate_func_def_retval_assign(i);
                ret_bool_arg(cur_retval->boolean);
                break;
            case AST_NODE_SYMBOL:
                generate_func_def_retval_assign(i);
                ret_id_arg(&cur_retval->symbol); 
                break;
            case AST_NODE_STRING:
                generate_func_def_retval_assign(i);
                ret_string_arg(cur_retval->string.ptr); 
                break;
            case AST_NODE_NIL:
                generate_func_def_retval_assign(i);
                ret_nil_arg();
                break;
            case AST_NODE_FUNC_CALL:
                process_node_func_call(cur_retval); //TODO Func call v returne
                int children = count_children(cur_retval->func_call.def->return_types);
                for(int k = 0; k< children && i < retval_counter1;k++){
                    generate_func_def_retval_assign(i); printf("TF@retval%d\n",k);
                    i++;
                }
                break;
            case AST_NODE_BINOP:
                process_binop_node(cur_retval);
                ret_binop_arg();
                generate_func_def_retval_assign(i);
                generate_result();
                break;
            case AST_NODE_UNOP:
                process_unop_node(cur_retval);
                ret_binop_arg();
                generate_func_def_retval_assign(i);
                generate_result();
                break;
        }
        cur_retval=cur_retval->next;
    }
}






void generate_integer_assignment(ast_node_t *rvalue){
    printf("int@%ld\n",rvalue->integer);
}

void generate_id_assignment(ast_node_t *rvalue){
    char * id;
    char * suffix;
    get_id_name(&rvalue->symbol,&id,&suffix);
    printf("LF@%s%%%s\n",id,suffix);
}

void generate_number_assignment(ast_node_t *rvalue){
    printf("float@%a\n",rvalue->number);
}

void generate_bool_assignment(ast_node_t *rvalue){
    if (rvalue->boolean==1){
        printf("bool@true\n");
    }
    else{
        printf("bool@false\n");
    }
}

void generate_string_assignment(ast_node_t *rvalue){
    OUTPUT_CODE_PART("string@"); process_string(rvalue->string.ptr); OUTPUT_CODE_LINE(""); //prerobit
}

void generate_nil_assignment(){
    printf("nil@nil\n");

}


void generate_func_call_assignment(ast_node_t *rvalue,int lside_counter){
    process_node_func_call(rvalue);

    if (rvalue->next){ //If the func call is not the last in assignment right side, only the first retval is used.
        OUTPUT_CODE_LINE("PUSHS TF@retval0");
    }

    if(rvalue->next == NULL){ //We can pad with nil and return more than one value if the last item in list is function.

        int ret_count = count_children(rvalue->func_call.def->return_types);
        for (int i = 0; i<ret_count; i++){
            OUTPUT_CODE_PART("PUSHS TF@retval"); printf("%d\n",i); //Push all children.
        }
        //printf("LSIDE %d..... RET_COUNT %d\n",lside_counter,ret_count);
        for (int k = 0; k<lside_counter-ret_count;k++){            //If need be, pad with nils
            OUTPUT_CODE_LINE("PUSHS nil@nil"); 
        }
    }
    
}

void generate_func_call_assignment_decl(ast_node_t *rvalue){
    process_node_func_call(rvalue);
    OUTPUT_CODE_LINE("MOVE GF@result TF@retval0");
}

void generate_binop_assignment(ast_node_t *rvalue){
    process_binop_node(rvalue);
    OUTPUT_CODE_LINE("POPS GF@result");
}

void generate_unop_assignment(ast_node_t *rvalue){
    process_unop_node(rvalue);
    OUTPUT_CODE_LINE("POPS GF@result");
}


void generate_declaration(symbol_t  * symbol){
    char * id;
    char * suffix;
    get_id_name(symbol,&id,&suffix);
    printf("DEFVAR LF@%s%%%s\n",id,suffix);
}

void generate_move(symbol_t * symbol){
    char * id;
    char * suffix;
    get_id_name(symbol,&id,&suffix);
    printf("MOVE LF@%s%%%s ",id,suffix);
}




void process_declaration_node(ast_node_t *cur_node, bool is_in_loop){
    ast_node_t * rvalue = cur_node->declaration.assignment;
    if (is_in_loop){
        generate_declaration(&cur_node->declaration.symbol);
        return;
    }
    if (!rvalue){
        generate_move(&cur_node->declaration.symbol);
        generate_nil_assignment(); //done
        return;
    }
    else{
        switch(rvalue->node_type){
            case AST_NODE_SYMBOL:
                generate_move(&cur_node->declaration.symbol);
                generate_id_assignment(rvalue); //done
                break;
            case AST_NODE_INTEGER:
                generate_move(&cur_node->declaration.symbol);
                generate_integer_assignment(rvalue); //done
                break;
            case AST_NODE_NUMBER:
                generate_move(&cur_node->declaration.symbol);
                generate_number_assignment(rvalue); //done
                break;
            case AST_NODE_BOOLEAN:
                generate_move(&cur_node->declaration.symbol);
                generate_bool_assignment(rvalue); //done
                break;
            case AST_NODE_STRING:
                generate_move(&cur_node->declaration.symbol);
                generate_string_assignment(rvalue); //done
                break;
            case AST_NODE_NIL:
                generate_move(&cur_node->declaration.symbol);
                generate_nil_assignment(); //done
                break;
            case AST_NODE_FUNC_CALL:
                generate_func_call_assignment_decl(rvalue);
                generate_move(&cur_node->declaration.symbol);
                generate_result();
                break;
            case AST_NODE_BINOP:
                generate_binop_assignment(rvalue);
                generate_move(&cur_node->declaration.symbol);
                generate_result();
                break;
            case AST_NODE_UNOP:
                generate_unop_assignment(rvalue);
                generate_move(&cur_node->declaration.symbol);
                generate_result();
                break;
        }
    }
}


//PROCESSING ASSIGNMENT NODE

void generate_integer_push(ast_node_t *rvalue){
    printf("int@%ld\n",rvalue->integer);
}

void generate_symbol_push(ast_node_t *rvalue){
    char * id;
    char * suffix;
    get_id_name(&rvalue->symbol,&id,&suffix);
    printf("LF@%s%%%s\n",id,suffix);
}

void generate_number_push(ast_node_t *rvalue){
    printf("float@%a\n",rvalue->number);
}


void generate_bool_push(ast_node_t *rvalue){
    if (rvalue->boolean==1){
        printf("bool@true\n");
    }
    else{
        printf("bool@false\n");
    }
}


void generate_string_push(ast_node_t *rvalue){
    OUTPUT_CODE_PART("string@"); process_string(rvalue->string.ptr); OUTPUT_CODE_LINE(""); 
}


void generate_nil_push(){
    printf("nil@nil\n");
}




void process_assignment_node(ast_node_t *cur_node){
    ast_node_t * identifier_iterator = cur_node->assignment.identifiers;
    int lside_counter = 0;
    while (identifier_iterator!=NULL){
        lside_counter++;
        identifier_iterator=identifier_iterator->next;
    }
    int rside_counter = 0;
    ast_node_t * expression = cur_node->assignment.expressions;
    while(expression!=NULL){
        switch(expression->node_type){
            case AST_NODE_SYMBOL:
                OUTPUT_CODE_PART("PUSHS ");
                generate_symbol_push(expression); //done
                break;
            case AST_NODE_INTEGER:
                OUTPUT_CODE_PART("PUSHS ");
                generate_integer_push(expression); //done
                break;
            case AST_NODE_NUMBER:
                OUTPUT_CODE_PART("PUSHS ");
                generate_number_push(expression); //done
                break;
            case AST_NODE_BOOLEAN:
                OUTPUT_CODE_PART("PUSHS ");
                generate_bool_push(expression); //done
                break;
            case AST_NODE_STRING:
                OUTPUT_CODE_PART("PUSHS ");
                generate_string_push(expression); //done
                break;
            case AST_NODE_NIL:
                OUTPUT_CODE_PART("PUSHS ");
                generate_nil_push(); //done
                break;
            case AST_NODE_FUNC_CALL:
                generate_func_call_assignment(expression,lside_counter-rside_counter);
                break;
            case AST_NODE_BINOP:
                generate_binop_assignment(expression);
                OUTPUT_CODE_PART("PUSHS ");
                generate_result();
                break;
             case AST_NODE_UNOP:
                generate_unop_assignment(expression);
                OUTPUT_CODE_PART("PUSHS ");
                generate_result();
                break;
        }
        rside_counter++;
        expression=expression->next;
    }
    for(int j = 0; j<rside_counter-lside_counter;j++){
        OUTPUT_CODE_LINE("POPS GF@trash"); //Losing unwanted expression results.
    }

    ast_node_t * identifier;
    int cur_max = lside_counter-1;
    for(int l = 0; l<lside_counter;l++){
        identifier = cur_node->assignment.identifiers;
        //printf("L: %d\n",l);
        for(int k = 0;k<lside_counter;k++){
            //printf("K: %d\n",k);
            if(k==cur_max){
                OUTPUT_CODE_LINE("POPS GF@result");
                //printf("SOM TU, najdi sa\n");
                //printf("SYMBOL JE %p\n",identifier->symbol.declaration);
                generate_move(&identifier->symbol);
                generate_result();
                }
            identifier=identifier->next;
        }
        cur_max--;
    }
}


void output_label(char * label, int counter){
    printf("%%%d%%%s%%%d",global_func_counter,label,counter);
}


void eval_condition(){
    OUTPUT_CODE_LINE("LABEL EVAL_CONDITION");
    OUTPUT_CODE_LINE("POPS GF@result");

    OUTPUT_CODE_LINE("TYPE GF@type1 GF@result");
    OUTPUT_CODE_LINE("JUMPIFEQ IS_FALSE GF@type1 string@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ IS_BOOL GF@type1 string@bool");
    OUTPUT_CODE_LINE("JUMP IS_TRUE");                           //All other types are true

    OUTPUT_CODE_LINE("LABEL IS_BOOL");
    OUTPUT_CODE_LINE("JUMPIFEQ IS_FALSE GF@result bool@false"); //bool false == false
    OUTPUT_CODE_LINE("JUMP IS_TRUE");                           //bool true  == true
    OUTPUT_CODE_LINE("JUMP END_EVAL_CHECK");

    OUTPUT_CODE_LINE("LABEL IS_FALSE");
    OUTPUT_CODE_LINE("MOVE GF@result bool@false"); //result = false
    OUTPUT_CODE_LINE("JUMP END_EVAL_CHECK");

    OUTPUT_CODE_LINE("LABEL IS_TRUE");
    OUTPUT_CODE_LINE("MOVE GF@result bool@true"); //result = true
    OUTPUT_CODE_LINE("JUMP END_EVAL_CHECK");

    OUTPUT_CODE_LINE("LABEL END_EVAL_CHECK");
    OUTPUT_CODE_LINE("PUSHS GF@result");
    OUTPUT_CODE_LINE("RETURN");
}

//GF@ifcondtype
void generate_if_code(char *label, ast_node_t * condition, ast_node_t * body, int counter){

    if (condition){
        process_node(condition);
    }
    OUTPUT_CODE_LINE("CALL EVAL_CONDITION"); //TODO zjednodusit

    OUTPUT_CODE_LINE("POPS GF@result");

    OUTPUT_CODE_PART("JUMPIFEQ ");output_label(label,counter); OUTPUT_CODE_LINE(" GF@result bool@false");

    process_node(body);
    OUTPUT_CODE_PART("JUMP "); output_label(label,0);OUTPUT_CODE_LINE("");
    OUTPUT_CODE_PART("LABEL "); output_label(label,counter);OUTPUT_CODE_LINE("");

}

void process_if_node(ast_node_t *cur_node){
    int if_counter = 1;
    ast_node_t * condition = cur_node->if_condition.conditions;
    ast_node_t * body = cur_node->if_condition.bodies;
    char * label = cur_node->if_condition.label.ptr;
    while(condition!=NULL){
        generate_if_code(label,condition,body,if_counter);
        condition=condition->next;
        body=body->next;
        if_counter++;
    }
    if(body){
        process_node(body);
    }
    OUTPUT_CODE_PART("LABEL "); output_label(label,0);OUTPUT_CODE_LINE("");
}

void process_while_node(ast_node_t *cur_node){
    ast_node_t * condition = cur_node->while_loop.condition;
    ast_node_t * body = cur_node->while_loop.body;
    char * label = cur_node->while_loop.label.ptr;
    OUTPUT_CODE_PART("LABEL "); output_label(label,0);OUTPUT_CODE_LINE("");
    process_node(condition);
    OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
    OUTPUT_CODE_LINE("POPS GF@result");
    OUTPUT_CODE_PART("JUMPIFEQ ");output_label(label,1); OUTPUT_CODE_LINE(" GF@result bool@false");
    process_node(body);
    OUTPUT_CODE_PART("JUMP "); output_label(label,0);OUTPUT_CODE_LINE("");
    OUTPUT_CODE_PART("LABEL "); output_label(label,1);OUTPUT_CODE_LINE("");
}

void process_repeat_until(ast_node_t *cur_node){
    ast_node_t * condition = cur_node->repeat_loop.condition;
    ast_node_t * body = cur_node->repeat_loop.body;
    char * label = cur_node->repeat_loop.label.ptr;
    OUTPUT_CODE_PART("LABEL "); output_label(label,0);OUTPUT_CODE_LINE("");
    process_node(body);
    process_node(condition);
    OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
    OUTPUT_CODE_LINE("POPS GF@result");
    OUTPUT_CODE_PART("JUMPIFEQ ");output_label(label,1); OUTPUT_CODE_LINE(" GF@result bool@false");
    OUTPUT_CODE_PART("JUMP "); output_label(label,0);OUTPUT_CODE_LINE("");
    OUTPUT_CODE_PART("LABEL "); output_label(label,1);OUTPUT_CODE_LINE("");
}

void process_node(ast_node_t *cur_node){
    //printf("Processing node\n");
    switch (cur_node->node_type){
        case AST_NODE_INVALID:
            break;
        case AST_NODE_FUNC_DECL:
            // Declarations are ignored in code generator. 
            break;
        case AST_NODE_SYMBOL:
            OUTPUT_CODE_PART("PUSHS ");  
            ret_id_arg(&(cur_node->symbol));
            break;
        case AST_NODE_INTEGER:
            OUTPUT_CODE_PART("PUSHS ");printf("int@%ld\n",cur_node->integer);
            break;
        case AST_NODE_NUMBER:
            OUTPUT_CODE_PART("PUSHS ");printf("float@%a\n",cur_node->number);
            break;
        case AST_NODE_STRING:
            OUTPUT_CODE_PART("PUSHS ");printf("string@%s\n",cur_node->string.ptr);
            break;
        case AST_NODE_NIL:
            OUTPUT_CODE_LINE("PUSHS nil@nil");
            break;
        case AST_NODE_BOOLEAN:
            OUTPUT_CODE_PART("PUSHS ");generate_bool_push(cur_node);
            break;
        case AST_NODE_FUNC_DEF:
            process_node_func_def(cur_node);
            break;

        case AST_NODE_FUNC_CALL:
            process_node_func_call(cur_node);
            break;
        
        case AST_NODE_DECLARATION:
            process_declaration_node(cur_node,false);
            break;
        
        case AST_NODE_ASSIGNMENT:
            process_assignment_node(cur_node);
            break;

        case AST_NODE_IF:
            process_if_node(cur_node);
            break;

        case AST_NODE_WHILE:
            process_while_node(cur_node);
            break;

        case AST_NODE_RETURN:
            process_return_node(cur_node);
            break;

        case AST_NODE_BINOP:
            process_binop_node(cur_node);
            break;
        case AST_NODE_BODY:
            for(ast_node_t *it = cur_node->body.statements; it!=NULL; it = it->next ){
                process_node(it);
            }
        default:
            break;
    }
}

void look_for_declarations(ast_node_t *root)
{
    switch(root->node_type) {
    case AST_NODE_DECLARATION:
        process_declaration_node(root,true);
        break;
    case AST_NODE_BODY:
        {
        ast_node_t *body_statement = root->body.statements;
        while (body_statement){
            look_for_declarations(body_statement);
            body_statement=body_statement->next;
        }
        break;
        }
    case AST_NODE_IF:
        {
            ast_node_list_t bodies = root->if_condition.bodies;
            while(bodies) {
                look_for_declarations(bodies);
                bodies = bodies->next;
            }
        }
        break;
    case AST_NODE_WHILE:
        look_for_declarations(root->while_loop.body);
        break;
    case AST_NODE_REPEAT:
        look_for_declarations(root->repeat_loop.body);
        break;
    case AST_NODE_FOR:
        look_for_declarations(root->for_loop.iterator);
        look_for_declarations(root->for_loop.body);
        break;
    }
}


void generate_reads(){
    OUTPUT_CODE_LINE("LABEL $reads");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("READ LF@retval0 string");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_readi(){
    OUTPUT_CODE_LINE("LABEL $readi");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("READ LF@retval0 int");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_readn(){
    OUTPUT_CODE_LINE("LABEL $readn");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("READ LF@retval0 float");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_tointeger(){
    OUTPUT_CODE_LINE("LABEL $tointeger");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("DEFVAR LF@param0");

    OUTPUT_CODE_LINE("MOVE LF@param0 LF@%0");

    OUTPUT_CODE_LINE("JUMPIFNEQ TOINT_GOOD LF@param0 nil@nil");
    OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL TOINT_GOOD");
    OUTPUT_CODE_LINE("FLOAT2INT LF@retval0 LF@param0");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_chr(){
    OUTPUT_CODE_LINE("LABEL $chr");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("DEFVAR LF@%param0");
    OUTPUT_CODE_LINE("MOVE LF@%param0 LF@%0");

    OUTPUT_CODE_LINE("GT GF@result LF@%param0 int@255");
    OUTPUT_CODE_LINE("JUMPIFEQ CHR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("LT GF@result LF@%param0 int@0");
    OUTPUT_CODE_LINE("JUMPIFEQ CHR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("JUMPIFEQ CHR_NIL LF@%param0 nil@nil");

    OUTPUT_CODE_LINE("JUMP CHR_OK");
    OUTPUT_CODE_LINE("LABEL CHR_OUT");
    OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
    OUTPUT_CODE_LINE("JUMP CHR_END");
    OUTPUT_CODE_LINE("LABEL CHR_OK");
    OUTPUT_CODE_LINE("INT2CHAR LF@retval0 LF@%param0");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL CHR_NIL");
    OUTPUT_CODE_LINE("EXIT int@8");

}

void generate_ord(){
    OUTPUT_CODE_LINE("LABEL $ord");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("DEFVAR LF@%param0");
    OUTPUT_CODE_LINE("DEFVAR LF@%param1");
    OUTPUT_CODE_LINE("MOVE LF@%param0 LF@%0");
    OUTPUT_CODE_LINE("MOVE LF@%param1 LF@%1");

    OUTPUT_CODE_LINE("STRLEN GF@trash LF@%param0");//Get length of string

    OUTPUT_CODE_LINE("GT GF@result LF@%param1 GF@trash");//If index greater than strlen
    OUTPUT_CODE_LINE("JUMPIFEQ ORD_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("LT GF@result LF@%param1 int@1");  // If index lower than 1
    OUTPUT_CODE_LINE("JUMPIFEQ ORD_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("SUB LF@%param1 LF@%param1 int@1");
    OUTPUT_CODE_LINE("STRI2INT LF@retval0 LF@%param0 LF@%param1");
    OUTPUT_CODE_LINE("JUMP ORD_END");
    OUTPUT_CODE_LINE("LABEL ORD_OUT");
    OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
    OUTPUT_CODE_LINE("LABEL ORD_END");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void check_type(){
    
}

void int_zerodivcheck(){
    OUTPUT_CODE_LINE("LABEL $int_zerodivcheck");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("JUMPIFEQ $zero_division_int GF@op2 int@0");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL $zero_division_int");
    OUTPUT_CODE_LINE("EXIT int@9");
    OUTPUT_CODE_LINE("RETURN");
}

void float_zerodivcheck(){
    OUTPUT_CODE_LINE("LABEL $float_zerodivcheck");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("JUMPIFEQ $zero_division_float GF@op2 float@0x0.0p+0");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL $zero_division_float");
    OUTPUT_CODE_LINE("EXIT int@9");
    OUTPUT_CODE_LINE("RETURN");
}

void nil_check(){

    OUTPUT_CODE_LINE("LABEL $NIL_CHECK");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("JUMPIFEQ NIL_FOUND GF@op1 nil@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ NIL_FOUND GF@op2 nil@nil");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL NIL_FOUND");
    OUTPUT_CODE_LINE("EXIT int@8");
}

void check_for_conversion(){
    OUTPUT_CODE_LINE("LABEL $CONV_CHECK");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type2 GF@op2");


    OUTPUT_CODE_LINE("JUMPIFEQ TYPES_OK GF@type1 GF@type2");

    OUTPUT_CODE_LINE("JUMPIFEQ TYPES_OK GF@type1 string@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ TYPES_OK GF@type2 string@nil");

    OUTPUT_CODE_LINE("JUMPIFEQ FIRST_OP_INT GF@type1 string@int");
    OUTPUT_CODE_LINE("JUMPIFEQ SEC_OP_INT GF@type2 string@int");

    OUTPUT_CODE_LINE("LABEL FIRST_OP_INT");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op1 GF@op1");
    OUTPUT_CODE_LINE("JUMP TYPES_OK");

    OUTPUT_CODE_LINE("LABEL SEC_OP_INT");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op2 GF@op2"); 
    OUTPUT_CODE_LINE("JUMP TYPES_OK");

    OUTPUT_CODE_LINE("LABEL TYPES_OK");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");

}

void generate_substring(){
OUTPUT_CODE_LINE("LABEL $substr");
OUTPUT_CODE_LINE("PUSHFRAME");
OUTPUT_CODE_LINE("DEFVAR LF@retval0");
OUTPUT_CODE_LINE("MOVE LF@retval0 string@");

OUTPUT_CODE_LINE("DEFVAR LF@%param0");
OUTPUT_CODE_LINE("DEFVAR LF@%param1");
OUTPUT_CODE_LINE("DEFVAR LF@%param2");

OUTPUT_CODE_LINE("DEFVAR LF@iterator");
OUTPUT_CODE_LINE("DEFVAR LF@stringend");
OUTPUT_CODE_LINE("DEFVAR LF@letter");

OUTPUT_CODE_LINE("MOVE LF@%param0 LF@%0");
OUTPUT_CODE_LINE("MOVE LF@%param1 LF@%1");
OUTPUT_CODE_LINE("MOVE LF@%param2 LF@%2");

OUTPUT_CODE_LINE("STRLEN GF@trash LF@%param0");//Get length of string

OUTPUT_CODE_LINE("GT GF@result LF@%param1 GF@trash");//If index i greater than strlen
OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

OUTPUT_CODE_LINE("LT GF@result LF@%param1 int@1");  // If index i lower than 1
OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");


OUTPUT_CODE_LINE("GT GF@result LF@%param2 GF@trash");//If index j greater than strlen
OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

OUTPUT_CODE_LINE("LT GF@result LF@%param2 int@1");  // If index j lower than 1
OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

OUTPUT_CODE_LINE("LT GF@result LF@%param2 LF@%param1");  // If index i bigger than j
OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_NIL LF@%param1 nil@nil"); // if i is nil
OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_NIL LF@%param2 nil@nil"); // if j is nil

OUTPUT_CODE_LINE("MOVE LF@iterator LF@%param1");
OUTPUT_CODE_LINE("SUB LF@iterator LF@iterator int@1");

OUTPUT_CODE_LINE("MOVE LF@stringend LF@%param2");
OUTPUT_CODE_LINE("SUB LF@stringend LF@stringend int@1");

OUTPUT_CODE_LINE("LABEL LOOP");
OUTPUT_CODE_LINE("GETCHAR LF@letter LF@%param0 LF@iterator");
OUTPUT_CODE_LINE("CONCAT LF@retval0 LF@retval0 LF@letter");
OUTPUT_CODE_LINE("JUMPIFEQ DONE LF@iterator LF@stringend");
OUTPUT_CODE_LINE("ADD LF@iterator LF@iterator int@1");
OUTPUT_CODE_LINE("JUMP LOOP");

OUTPUT_CODE_LINE("LABEL DONE");
OUTPUT_CODE_LINE("POPFRAME");
OUTPUT_CODE_LINE("RETURN");

OUTPUT_CODE_LINE("LABEL SUBSTR_OUT");
OUTPUT_CODE_LINE("MOVE LF@retval0 string@");
OUTPUT_CODE_LINE("POPFRAME");
OUTPUT_CODE_LINE("RETURN");

OUTPUT_CODE_LINE("LABEL SUBSTR_NIL");
OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
OUTPUT_CODE_LINE("POPFRAME");
OUTPUT_CODE_LINE("RETURN");

}

void generate_builtin(){
    generate_reads();
    EMPTY_LINE;
    generate_readi();
    EMPTY_LINE;
    generate_readn();
    EMPTY_LINE;
    generate_tointeger();
    EMPTY_LINE;
    generate_chr();
    EMPTY_LINE;
    generate_ord();
    EMPTY_LINE;
    generate_substring();
    EMPTY_LINE;
    int_zerodivcheck();
    EMPTY_LINE;
    float_zerodivcheck();
    EMPTY_LINE;
    nil_check();
    EMPTY_LINE;
    check_for_conversion();
    EMPTY_LINE;
    check_nil_write();
    EMPTY_LINE;
    eval_condition();
    EMPTY_LINE;


}

void generate_header(){
    OUTPUT_CODE_LINE(".IFJcode21");
    EMPTY_LINE;
    COMMENT("Global variables:");
    OUTPUT_CODE_LINE("DEFVAR GF@result");
    OUTPUT_CODE_LINE("DEFVAR GF@stackresult");
    OUTPUT_CODE_LINE("DEFVAR GF@op1");
    OUTPUT_CODE_LINE("DEFVAR GF@op2");
    OUTPUT_CODE_LINE("DEFVAR GF@type1");
    OUTPUT_CODE_LINE("DEFVAR GF@type2");
    OUTPUT_CODE_LINE("DEFVAR GF@trash");
    OUTPUT_CODE_LINE("DEFVAR GF@string0");
    OUTPUT_CODE_LINE("DEFVAR GF@string1");
    OUTPUT_CODE_LINE("DEFVAR GF@loop_iterator");
    OUTPUT_CODE_LINE("JUMP $$main");
    EMPTY_LINE;
    COMMENT("Built-in functions:");

}

void process_node_program(ast_node_t *cur_node){
    generate_builtin();
    ast_node_t *top_level_definitions = cur_node->program.global_statement_list;
    while (top_level_definitions){
        if (top_level_definitions->node_type == AST_NODE_FUNC_DEF){
            process_node(top_level_definitions);
        }
        top_level_definitions=top_level_definitions->next;
    }
    OUTPUT_CODE_LINE("LABEL $$main");
    ast_node_t *top_level_call = cur_node->program.global_statement_list;
    while (top_level_call){
        if (top_level_call->node_type == AST_NODE_FUNC_CALL){
            process_node(top_level_call);
        }
        top_level_call=top_level_call->next;
    }
}

void avengers_assembler(ast_node_t *ast){
    generate_header();
    process_node_program(ast);
}