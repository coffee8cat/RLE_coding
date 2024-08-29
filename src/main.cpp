#include <stdio.h>
#include <stdlib.h>

#include <RLE.h>

int main()
{

    FILE *fp_source = fopen("data\\data1.hex", "rb");
    FILE *fp_target = fopen("coded_data\\data_encoded.hex", "wb");

    RLE_encode(fp_source, fp_target);

    fclose(fp_source);
    fclose(fp_target);

    fp_source = fopen("coded_data\\data_encoded.hex", "rb");
    fp_target = fopen("data\\data2.hex", "wb");

    RLE_decode(fp_source, fp_target);


    fclose(fp_source);
    fclose(fp_target);

    return EXIT_SUCCESS;
}
