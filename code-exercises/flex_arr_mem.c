#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct 
{
    char *buf;
    uint32_t len;
} LegacyString;

typedef struct
{
    uint32_t len;
    // Flexible Array Memeber
    char buf[]; 
} ModernString;

int main(void)
{
    char *str = "Oxford";
    size_t str_len = strlen(str) + 1;
    printf("strlen %zu\n", str_len);
    size_t ms_size = sizeof(ModernString);
    printf("ms_size %zu\n", ms_size);

    LegacyString *ls = (LegacyString *)calloc(1, sizeof(LegacyString));
    if (ls == NULL) return -1;
    ls->len = str_len;
    ls->buf = (char *)calloc(1, sizeof(char) * str_len);
    if (ls->buf == NULL) return -1;

    ModernString *ms = (ModernString *)calloc(1, ((sizeof(char) * ms_size) + str_len)); 
    if (ms == NULL) return -1;
    ms->len = str_len;
    
    // Manually copy the string 
    for (size_t i = 0; i < str_len; i++)
        *(ms->buf + i) = *(str + i);
    
    // Read the string 
    for (size_t i = 0; i < str_len; i++)
        printf("%c", *(ms->buf + i));

    
    free(ms);
    free(ls->buf);
    free(ls);

    return 0;
}
