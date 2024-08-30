#ifndef _RLE_TEXT_COMPRESSION_H__
#define _RLE_TEXT_COMPRESSION_H__

void RLE_encode(FILE *fp_source, FILE *fp_target);


void RLE_decode(FILE *fp_source, FILE *fp_target);
void add_ch_in_buf(char out_buf[], char *seq_length, char ch, bool *is_repeat_before);
void buf_flush(char seq_length, char out_buf[], char output[], size_t *out_ind);

void seq_start(char *seq_length, char out_buf[], char output[], size_t *out_ind, bool *is_repeat_before, bool *is_repeat);
void seq_flush(char seq_length, char ch, char output[], size_t *out_ind);
void seq_end(char *seq_length, char ch, char output[], size_t *out_ind, bool *is_repeat, bool *is_repeat_before);

void check_buf(char *seq_length, char output[], size_t *out_ind, char ch, char out_buf[]);

void read_seq_length(char *seq_length, char *counter, char ch);
void write_rep_seq(char decode_buffer[], size_t *buf_ind, size_t decode_buf_size, FILE *fp_target, char *seq_length, char ch);
void end_seq(char *seq_length, char *counter, char ch);
void add_ch_in_decode_buf(char decode_buffer[], size_t *buf_ind, size_t decode_buf_size, FILE *fp_target, char ch, char *counter);
void decode_buffer_flush(char *decode_buffer, size_t *buf_ind, FILE *fp_target);

void check_files(FILE *fp1, FILE *fp2);

#endif //_RLE_TEXT_COMPRESSION_H__
