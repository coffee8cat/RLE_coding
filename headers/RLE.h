#ifndef _RLE_TEXT_COMPRESSION_H__
#define _RLE_TEXT_COMPRESSION_H__


void buf_flush(char seq_length, char *out_buf, FILE *fp_target)
void seq_flush(char seq_length, char ch, FILE *fp_target)

void RLE_encoder(FILE *fp_source, FILE *fp_target);
void RLE_decoder(FILE *fp_source, FILE *fp_target);

#endif //_RLE_TEXT_COMPRESSION_H__
