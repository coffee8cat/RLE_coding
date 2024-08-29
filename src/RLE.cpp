#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "RLE.h"

void buf_flush(char seq_length, char *out_buf, FILE *fp_target)
{
    fwrite((void*)&seq_length, sizeof(char), 1, fp_target);
    printf("%d", seq_length);
    for(size_t i = 0; i < (size_t)abs(seq_length); i++)
    {
        fwrite((void*)&out_buf[i], sizeof(char), 1, fp_target);
        printf("%c", out_buf[i]);
    }
}

void seq_flush(char seq_length, char ch, FILE *fp_target)
{
    fwrite((void*)&seq_length, sizeof(char), 1, fp_target);
    fwrite((void*)&ch, sizeof(char), 1, fp_target);
    printf("%d", seq_length);
    printf("%c", ch);
}

void RLE_encode(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    char prev_ch = 0;
    char curr_ch = 0;
    fread((void*)&prev_ch, sizeof(char), 1, fp_source);
    fread((void*)&curr_ch, sizeof(char), 1, fp_source);

    bool is_repeat = (prev_ch == curr_ch);
    bool is_repeat_before = false;

    fpos_t position = 1;
    fsetpos(fp_source, &position);

    char out_buf[128] = {};

    char seq_length = 1;

    while (fread((void*)&curr_ch, sizeof(char), 1, fp_source))
    {
        if (curr_ch == prev_ch && is_repeat)
        {
            seq_length++;
        }

        else if (curr_ch != prev_ch && !is_repeat)
        {
            if(is_repeat_before)
            {
                is_repeat_before = false;
            }
            out_buf[abs(seq_length--)] = prev_ch;
        }

        else if (curr_ch == prev_ch && !is_repeat)
        {
            if(!is_repeat_before)
            {
                buf_flush(seq_length, out_buf, fp_target);
            }

            seq_length = 2;
            is_repeat = true;
        }

        else if (curr_ch != prev_ch && is_repeat)
        {
            seq_flush(seq_length, prev_ch, fp_target);

            seq_length = 0;
            is_repeat = false;
            is_repeat_before = true;
        }

        if(curr_ch == '\0')
        {
            if (is_repeat)
            {
                seq_flush(seq_length, prev_ch, fp_target);
            }
            else
            {
                buf_flush(seq_length, out_buf, fp_target);
            }
        }
        prev_ch = curr_ch;
    }

    if (is_repeat)
    {
        seq_flush(seq_length, prev_ch, fp_target);
    }
    else
    {
        buf_flush(seq_length, out_buf, fp_target);
    }
    printf("\n");
}

void RLE_decode(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    char ch = 0;
    char counter = 0;

    while(fread((void*)&counter, sizeof(char), 1, fp_source) && counter != EOF)
    {
        if(counter < 0)
        {
            for(size_t i = 0; i < (size_t)abs(counter); i++)
            {
                fread((void*)&ch, sizeof(char), 1, fp_source);
                fwrite((void*)&ch, sizeof(char), 1, fp_target);
            }
        }
        else
        {
            fread((void*)&ch, sizeof(char), 1, fp_source);
            for(size_t i = 0; i < (size_t)counter; i++)
            {
                fwrite((void*)&ch, sizeof(char), 1, fp_target);
            }
        }
    }
}
