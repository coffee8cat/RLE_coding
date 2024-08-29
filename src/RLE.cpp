#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "RLE.h"

void buf_flush(char seq_length, char *out_buf, FILE *fp_target)
{
    fwrite((void*)&seq_length, size_of(char), 1, fp_target);
    for(char i = 0; i < abs(seq_length); i++)
    {
        fwrite((void*)&out_buf[i], size_of(char), 1, fp_target);
    }
}

void seq_flush(char seq_length, char ch, FILE *fp_target)
{
    fwrite((void*)&seq_length, size_of(char), 1, fp_target);
    for(char i = 0; i < abs(seq_length); i++)
    {
        fwrite((void*)&ch, size_of(char), 1, fp_target);
    }
}

void RLE_encoder(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    char ch_prev = 0;
    char ch_curr = 0;
    fread((void*)&ch_prev, sizeof(char), 1, fp_source);
    fread((void*)&ch_curr, sizeof(char), 1, fp_source);

    bool is_repeat = (ch_prev == ch_curr);
    bool is_first_repeat = false;

    ftell(fp_source, 1, SEEK_SET);

    char buf_iter = 0;
    char out_buf[128] = {};

    char seq_length = 1;

    while (prev_ch != '\0')
    {
        fread((void*)&ch_curr, sizeof(char), 1, fp_source);

        if (curr_ch == prev_ch && is_repeat)
        {
            seq_length++;
        }
        else if (curr_ch != prev_ch && !is_repeat)
        {
            out_buf[abs(seq_lenght--)] = prev_ch;
        }
        else if (curr_ch == prev_ch && !is_repeat)
        {
            buf_flush(seq_length, out_buf, fp_target);

            seq_length = 2;
            is_repeat = true;
        }
        else if (curr_ch != prev_ch && is_repeat)
        {
            seq_flush(seq_flush, prev_ch, fp_target);

            seq_length = 0;
            is_repeat = false;
        }

        if (curr_ch == '\0')
        {
            if (is_repeat)
            {
                seq_flush(seq_length, prev_ch, fp_target);
            }
            else
            {
                buf_flush(seq_length, out_buf, fp_target);
            }
            break;
        }
        prev_ch = curr_ch;
    }
}

void RLE_decoder(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    size_t n_copies = 0;
    char ch = ' ';
    char counter = 0;

    while(fread((void*)&counter, sizeof(char), 1, fp_source))
    {
        if(counter < 0)
        {
            for(char i = 0; i < abs(counter); i++)
            {
                fread((void*)&ch, sizeof(char), 1, fp_source);
                fwrite((void*)&ch, sizeof(char), 1, fp_target);
            }
        }
        else
        {
            fread((void*)&ch, sizeof(char), 1, fp_source);
            for(size_t i = 0; i < counter; i++)
            {
                fwrite((void*)&ch, sizeof(char), 1, fp_target);
            }
        }

    }
}
