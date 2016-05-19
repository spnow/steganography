// /**
//  * Read and parse a wave file
//  *
//  * Source: http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
//  **/
#include <string.h>
#include <stdlib.h>
#include "wav_io.h"
#include "utils.h"
#include "lsb.h"

unsigned char buffer4[4];
unsigned char buffer2[2];


long get_size_of_each_sample(HEADER_PARSED *header_p) {
    return (header_p->channels * header_p->bits_per_sample) / 8;
}

long get_num_samples(HEADER_PARSED *header_p) {
    return (8 *header_p->data_size) / (header_p->channels * header_p->bits_per_sample);

}

int read_headers(HEADER *header, FILE *ptr) {

    int read = 0;

    /* ------------------------ RIFF Chunk Descriptor ------------------------ */

    // Read RIFF chunkID

    read = fread(header->header_p.riff, sizeof(header->header_p.riff), 1, ptr);
    memcpy(&header->header_n.riff_desc.chunkID,header->header_p.riff,sizeof(CKID));

    // Read RIFF ChunkSize

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    memcpy(&header->header_n.riff_desc.chunkSize,buffer4,sizeof(CKSIZE));

    // convert little endian to big endian 4 byte int
    header->header_p.overall_size  = little_to_big_4_bytes(buffer4); 

    // Read format ('WAVE')

    read = fread(header->header_p.wave, sizeof(header->header_p.wave), 1, ptr);
    memcpy(&header->header_n.riff_desc.format, header->header_p.wave, sizeof(CKID));

    /* ---------------------------------------------------------------------------- */
    /* ------------------------ FMT sub-chunk ------------------------ */

    // Read 'fmt '

    read = fread(header->header_p.fmt_chunk_marker, sizeof(header->header_p.fmt_chunk_marker), 1, ptr);
    memcpy(&header->header_n.fmt.chunkID, header->header_p.fmt_chunk_marker, sizeof(CKID));

    // Read subchunkSize

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    memcpy(&header->header_n.fmt.chunkSize,buffer4,sizeof(CKSIZE));

    // convert little endian to big endian 4 byte integer
    header->header_p.length_of_fmt = little_to_big_4_bytes(buffer4);

    // Read Audio Format

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    memcpy(&header->header_n.fmt.wFormatTag, buffer2, sizeof(WORD));
    header->header_p.format_type = little_to_big_2_bytes(buffer2);

    // Read Audio Channels

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    memcpy(&header->header_n.fmt.wChannels, buffer2, sizeof(WORD));
    header->header_p.channels = little_to_big_2_bytes(buffer2);

    // Read Sample Rate

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    memcpy(&header->header_n.fmt.dwSamplesPerSec, buffer4, sizeof(DWORD));

    header->header_p.sample_rate = little_to_big_4_bytes(buffer4);

    // Read Byte Rate

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    memcpy(&header->header_n.fmt.dwAvgBytesPerSec, buffer4, sizeof(DWORD));

    header->header_p.byterate  = little_to_big_4_bytes(buffer4);

    // Read Block Alignment

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    memcpy(&header->header_n.fmt.wBlockAlign, buffer2, sizeof(WORD));

    header->header_p.block_align = little_to_big_2_bytes(buffer2);

    // Read Bits Per Sample

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    memcpy(&header->header_n.fmt.wBitsPerSample, buffer2, sizeof(WORD));

    header->header_p.bits_per_sample = little_to_big_2_bytes(buffer2);

    /* ---------------------------------------------------------------------------- */
    /* ------------------------ DATA sub-chunk Descriptor ------------------------ */

    // Read Data marker

    read = fread(header->header_p.data_chunk_header, sizeof(header->header_p.data_chunk_header), 1, ptr);
    memcpy(&header->header_n.data.chunkID,header->header_p.data_chunk_header, sizeof(CKID));

    // Read Size of data

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    memcpy(&header->header_n.data.chunkSize, buffer4, sizeof(CKSIZE));

    header->header_p.data_size = little_to_big_4_bytes(buffer4);

    /* ---------------------------------------------------------------------------- */
    //Asign FILE pointer to HEADER struct

    header->header_p.ptr = ptr;

    /* ---------------------------------------------------------------------------- */

    return 0;

}

int write_headers(HEADER *header, FILE *ptr) {
    
    int write = 0;

    /* ------------------------ RIFF Chunk Descriptor ------------------------ */

    // Write RIFF chunkID

    write= fwrite(&header->header_n.riff_desc.chunkID, sizeof(CKID), 1, ptr);

    // Write RIFF ChunkSize

    write= fwrite(&header->header_n.riff_desc.chunkSize, sizeof(CKSIZE), 1, ptr);

    // Write format ('WAVE')

    write= fwrite(&header->header_n.riff_desc.format, sizeof(CKID), 1, ptr);

    /* ---------------------------------------------------------------------------- */
    /* ------------------------ FMT sub-chunk ------------------------ */

    // Write 'fmt '

    write= fwrite(&header->header_n.fmt.chunkID, sizeof(CKID), 1, ptr);

    // Write subchunkSize

    write= fwrite(&header->header_n.fmt.chunkSize, sizeof(CKSIZE), 1, ptr);

    // Write Audio Format

    write= fwrite(&header->header_n.fmt.wFormatTag, sizeof(WORD), 1, ptr);

    // Write Audio Channels

    write= fwrite(&header->header_n.fmt.wChannels, sizeof(WORD), 1, ptr);

    // Write Sample Rate

    write= fwrite(&header->header_n.fmt.dwSamplesPerSec, sizeof(DWORD), 1, ptr);

    // Write Byte Rate

    write= fwrite(&header->header_n.fmt.dwAvgBytesPerSec, sizeof(DWORD), 1, ptr);

    // Write Block Alignment

    write= fwrite(&header->header_n.fmt.wBlockAlign, sizeof(WORD), 1, ptr);

    // Write Bits Per Sample

    write= fwrite(&header->header_n.fmt.wBitsPerSample, sizeof(WORD), 1, ptr);

    /* ---------------------------------------------------------------------------- */
    /* ------------------------ DATA sub-chunk Descriptor ------------------------ */

    // Write Data marker

    write= fwrite(&header->header_n.data.chunkID, sizeof(CKID), 1, ptr);

    // Write Size of data

    write= fwrite(&header->header_n.data.chunkSize, sizeof(CKSIZE), 1, ptr);

    /* ---------------------------------------------------------------------------- */

    return 0;

}

int write_steg_sound_data(HEADER *header, FILE *ptr, char* msg, size_t msg_size, int mode) {

    char sample_size = header->header_p.bits_per_sample / 8;
    char block_byte_size = 0;

    if (mode == LSB1) {
       block_byte_size = sample_size*8; 
    } else if (mode == LSB4) {
        block_byte_size = sample_size*2;
    }

    char sample[block_byte_size];

    for (int i = 0; i < msg_size; i++) {
        fread(sample, block_byte_size, 1, header->header_p.ptr);
        lsb_encode(sample, block_byte_size, 0, sample_size, &(msg[i]), 1, mode);
        fwrite(sample, block_byte_size, 1, ptr);
    }

    int remain = header->header_p.data_size - msg_size*block_byte_size;
    char byte[1];

    while (remain-->0) {
        fread(byte, 1, 1, header->header_p.ptr);
        fwrite(byte, 1, 1, ptr);
    }
    return 0;

}

int read_steg_sound_data(HEADER * header, char* msg, size_t msg_size, int mode) {

    //TODO this method should read the msg_len also, not recieve it as parameter

    char sample_size = header->header_p.bits_per_sample / 8;
    int block_size = 0 ;

    if (mode == LSB1) {
       block_size = sample_size*msg_size*8;
    } else if (mode == LSB4) {
        block_size = sample_size*msg_size*2;
    }

    char sample[block_size];
    fread(sample, block_size, 1, header->header_p.ptr);
    lsb_decode(sample, block_size, 0, sample_size, msg, msg_size, mode);
    return 0;
}