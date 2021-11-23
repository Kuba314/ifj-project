#include "tokensim.h"

int token_gen(token_t *token)
{
    int r = get_next_token(token);
    printf("get token: %s  [%d]", term_to_readable(token->token_type), token->token_type);
    if (token->token_type == T_IDENTIFIER) {
        printf(" (%s)", token->string.ptr);
    }
    else if (token->token_type == T_INTEGER) {
        printf(" (%ld)", token->integer);
    }
    printf("\n");
    if (r != E_OK) {
        printf("Get token error: %d\n", r);
    }
    return r;
}
void token_unget(token_t *token)
{
    unget_token(token);

}
