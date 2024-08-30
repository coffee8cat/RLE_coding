#ifndef _RLE_TEXT_COMPRESSION_H__
#define _RLE_TEXT_COMPRESSION_H__

void RLE_encode(FILE *fp_source, FILE *fp_target);
void buf_flush(char seq_length, char *out_buf, char output[], size_t *out_ind);
void seq_flush(char seq_length, char ch, char output[], size_t *out_ind);

void RLE_decode(FILE *fp_source, FILE *fp_target);
void decode_buffer_flush(char *decode_buffer, size_t *buf_ind, FILE *fp_target);

void check_files(FILE *fp1, FILE *fp2);

#endif //_RLE_TEXT_COMPRESSION_H__
