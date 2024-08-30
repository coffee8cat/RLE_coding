#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "RLE.h"

void buf_flush(char seq_length, char *out_buf, char output[], size_t *out_ind)
{
    // printf("%d", seq_length);
    output[*out_ind] = seq_length;
    (*out_ind)++;
    for(size_t i = 0; i < (size_t)abs(seq_length); i++)
    {
        output[*out_ind] = out_buf[i];
        (*out_ind)++;
        // printf("%c", out_buf[i]);
    }
}

void seq_flush(char seq_length, char ch, char output[], size_t *out_ind)
{
    output[*out_ind] = seq_length;
    (*out_ind)++;
    output[*out_ind] = ch;
    (*out_ind)++;
    // printf("%d", seq_length);
    // printf("%c", ch);
}

void RLE_encode(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    char out_buf[128] = {};

    fseek(fp_source, 0, SEEK_END);
    size_t source_size = ftell(fp_source);
    fseek(fp_source, 0, SEEK_SET);

    size_t out_ind = 0;

    char *output = (char*)calloc(source_size * 2, sizeof(char));
    char *input  = (char*)calloc(source_size, sizeof(char));
    fread((void*)&input[0], sizeof(char), source_size, fp_source);

    bool is_repeat = (input[0] == input[1]);
    bool is_repeat_before = false;

    char seq_length = (char)is_repeat;

    for(size_t curr = 1; curr < source_size; curr++)
    {
        if (input[curr] == input[curr - 1] && is_repeat)
        {
            seq_length++;
        }

        else if (input[curr] != input[curr - 1] && !is_repeat)
        {
            if(is_repeat_before)
            {
                is_repeat_before = false;
            }
            out_buf[abs(seq_length--)] = input[curr-1];
        }

        else if (input[curr] == input[curr - 1] && !is_repeat)
        {
            if(!is_repeat_before)
            {
                buf_flush(seq_length, out_buf, output, &out_ind);
            }

            seq_length = 2;
            is_repeat = true;
        }

        else if (input[curr] != input[curr - 1] && is_repeat)
        {
            seq_flush(seq_length, input[curr - 1], output, &out_ind);

            seq_length = 0;
            is_repeat = false;
            is_repeat_before = true;
        }

        if (seq_length == 127)
        {
            seq_flush(seq_length, input[curr - 1], output, &out_ind);
            seq_length = 0;
        }
        else if (seq_length == -127)
        {
            buf_flush(seq_length, out_buf, output, &out_ind);
            seq_length = 0;
        }
        input[curr - 1] = input[curr];
    }
    fwrite((void*)&output[0], sizeof(char), out_ind, fp_target);
    printf("%s", output);
}

void RLE_decode(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    char ch = 0;
    char seq_length = 0;
    char counter = 0;

    fseek(fp_source, 0, SEEK_END);
    size_t source_size = ftell(fp_source);

    size_t decode_buf_size = 1024;
    size_t buf_ind = 0;

    char *decode_buffer = (char*)calloc(decode_buf_size, sizeof(char));
    char *input  = (char*)calloc(source_size, sizeof(char));
    fread((void*)&input[0], sizeof(char), source_size, fp_source);

    for(size_t i = 0; i < source_size; i++)
    {
        if(seq_length == 0)
        {
            seq_length = input[i];
            counter = 0;
        }
        else
        {
            if(seq_length & (0x1 << 7))
            {
                ch = input[i];
                for(size_t k = 0; k < seq_length; k++)
                {
                    if(buf_ind >= decode_buf_size)
                        decode_buffer_flush(decode_buffer, &buf_ind, fp_target);

                    decode_buffer[buf_ind] = ch;
                    buf_ind++;
                }
                seq_length = 0;
            }
            else
            {
                if(counter < seq_length * (-1))
                {
                    if(buf_ind >= decode_buf_size)
                            decode_buffer_flush(decode_buffer, &buf_ind, fp_target);

                    decode_buffer[buf_ind] = ch;
                    buf_ind++;
                    counter++;
                }
                else
                    seq_length = 0;
            }
        }
    }

    decode_buffer_flush(decode_buffer, &buf_ind, fp_target);
}

void decode_buffer_flush(char *decode_buffer, size_t *buf_ind, FILE *fp_target)
{
    fwrite((void*)decode_buffer, sizeof(char), *buf_ind, fp_target);
    memset((void*)decode_buffer, 0, *buf_ind);
    *buf_ind = 0;
}

void check_files(FILE *fp1, FILE *fp2)
{
    char ch1 = 0;
    char ch2 = 0;
    while(fread((void*)&ch1, sizeof(char), 1, fp1) && fread((void*)&ch2, sizeof(char), 1, fp2))
    {
        if(ch1 != ch2)
        {
            printf("%d ", ftell(fp1));
            fseek(fp1, 0, SEEK_END);
            printf("%d\n\n", ftell(fp1));
        }
    }
}
