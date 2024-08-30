#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "RLE.h"

void RLE_encode(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    char out_buf[128] = {};

    fseek(fp_source, 0, SEEK_END);
    size_t source_size = ftell(fp_source);
    fseek(fp_source, 0, SEEK_SET);

    size_t out_ind = 0;

    char *output = (char*)calloc(source_size * 2, sizeof(char)); // cringeeeeeee
    char *input  = (char*)calloc(source_size, sizeof(char)); // aaaaaaaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa
    fread((void*)input, sizeof(char), source_size, fp_source);

    bool is_repeat = (input[0] == input[1]);
    bool is_repeat_before = false;

    char seq_length = (char)is_repeat;

    for(size_t curr = 1; curr < source_size; curr++)
    {
        if (input[curr] == input[curr - 1] && is_repeat)
            seq_length++;

        else if (input[curr] != input[curr - 1] && !is_repeat)
            add_ch_in_buf(out_buf, &seq_length, input[curr - 1], &is_repeat_before);

        else if (input[curr] == input[curr - 1] && !is_repeat)
            seq_start(&seq_length, out_buf, output, &out_ind, &is_repeat_before, &is_repeat);

        else if (input[curr] != input[curr - 1] && is_repeat)
            seq_end(&seq_length, input[curr - 1], output, &out_ind, &is_repeat, &is_repeat_before);

        check_buf(&seq_length, output, &out_ind, input[curr - 1], out_buf);
        input[curr - 1] = input[curr];
    }

    buf_flush(seq_length, out_buf, output, &out_ind);
    fwrite((void*)output, sizeof(char), out_ind, fp_target);
}

void add_ch_in_buf(char out_buf[], char *seq_length, char ch, bool *is_repeat_before)
{
    if(*is_repeat_before)
    {
        *is_repeat_before = false;
    }
    out_buf[abs((*seq_length)--)] = ch;
}

void buf_flush(char seq_length, char out_buf[], char output[], size_t *out_ind)
{
    output[*out_ind] = seq_length;
    (*out_ind)++;
    for(size_t i = 0; i < (size_t)abs(seq_length); i++) // memcpy!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
        output[*out_ind] = out_buf[i];
        (*out_ind)++;
    }
}

void seq_flush(char seq_length, char ch, char output[], size_t *out_ind)
{
    output[*out_ind] = seq_length;
    (*out_ind)++;
    output[*out_ind] = ch;
    (*out_ind)++;
}

void seq_start(char *seq_length, char out_buf[], char output[], size_t *out_ind, bool *is_repeat_before, bool *is_repeat)
{
    if(!*is_repeat_before)
    {
        buf_flush(*seq_length, out_buf, output, out_ind);
    }

    *seq_length = 2;
    *is_repeat = true;

}
void seq_end(char *seq_length, char ch, char output[], size_t *out_ind, bool *is_repeat, bool *is_repeat_before)
{
    seq_flush(*seq_length, ch, output, out_ind);

    *seq_length = 0;
    *is_repeat = false;
    *is_repeat_before = true;
}

void check_buf(char *seq_length, char output[], size_t *out_ind, char ch, char out_buf[])
{
    if (*seq_length == 127)
    {
        seq_flush(*seq_length, ch, output, out_ind);
        *seq_length = 0;
    }
    else if (*seq_length == -127)
    {
        buf_flush(*seq_length, out_buf, output, out_ind);
        *seq_length = 0;
    }
}

void RLE_decode(FILE *fp_source, FILE *fp_target)
{
    assert(fp_source);
    assert(fp_target);

    char ch         = 0;
    char seq_length = 0;
    char counter    = 0;

    fseek(fp_source, 0, SEEK_END);
    size_t source_size = ftell(fp_source);
    fseek(fp_source, 0, SEEK_SET);

    size_t decode_buf_size = 1024;
    size_t buf_ind         = 0;

    char *decode_buffer = (char*)calloc(decode_buf_size, sizeof(char));
    char *input         = (char*)calloc(source_size, sizeof(char));
    fread((void*)input, sizeof(char), source_size, fp_source);

    for(size_t i = 0; i < source_size; i++)
    {
        if(seq_length == 0)
            read_seq_length(&seq_length, &counter, input[i]);

        else
        {
            if(seq_length > -1)
                write_rep_seq(decode_buffer, &buf_ind, decode_buf_size, fp_target, &seq_length, input[i]);

            else
            {
                if(counter < seq_length * (-1))
                    add_ch_in_decode_buf(decode_buffer, &buf_ind, decode_buf_size, fp_target, input[i], &counter);

                else
                    end_seq(&seq_length, &counter, input[i]);
            }
        }
    }

    decode_buffer_flush(decode_buffer, &buf_ind, fp_target);
}

void read_seq_length(char *seq_length, char *counter, char ch)
{
    *seq_length = ch;
    *counter = 0;
}

void write_rep_seq(char decode_buffer[], size_t *buf_ind, size_t decode_buf_size, FILE *fp_target, char *seq_length, char ch)
{
    for(size_t k = 0; k < *seq_length; k++)
    {
        if(*buf_ind >= decode_buf_size)
            decode_buffer_flush(decode_buffer, buf_ind, fp_target);

        decode_buffer[*buf_ind] = ch;
        (*buf_ind)++;
    }
    *seq_length = 0;
}

void end_seq(char *seq_length, char *counter, char ch)
{
    *counter = 0;
    *seq_length = ch;
}

void add_ch_in_decode_buf(char decode_buffer[], size_t *buf_ind, size_t decode_buf_size, FILE *fp_target, char ch, char *counter)
{
    if(*buf_ind >= decode_buf_size)
            decode_buffer_flush(decode_buffer, buf_ind, fp_target);

    decode_buffer[*buf_ind] = ch;
    (*buf_ind)++;
    (*counter)++;
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
