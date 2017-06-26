/*
 * A WAVE audio player using PulseAudio asynchronous APIs
 *
 * Copyright (C) 2016 Ahmed S. Darwish <darwish.07@gmail.com>
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, version 2 or later.
 *
 * This is an as minimal as it can get implementation for the
 * purpose of studying PulseAudio asynchronous APIs.
 */

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <pulse/sample.h>

#include "common.h"

struct riff_header{
    char     id[4];         /* "RIFF" */
    uint32_t size;       	/* ... */
    char     type[4];       /* "WAVE" */
} __attribute__((packed));

struct fmt_header{
    char     id[4];         /* "fmt" */
    uint32_t size;       	
    uint16_t tag;            
    uint16_t channels;          
    uint32_t samples_per_sec ;  
    uint32_t avgbytes_per_sec ;
    uint16_t block_align;         
    uint16_t bits_per_sample;    
    uint16_t extra         /*maybe not exist*/ 
} __attribute__((packed));

struct data_header{
    char     id[4];         /* "data" */
    uint32_t size;       	/* ... */
} __attribute__((packed));

static pa_sample_format_t bits_per_sample_to_pa_spec_format(int bits) {
    switch (bits) {
    case  8: return PA_SAMPLE_U8;
    case 16: return PA_SAMPLE_S16LE;
    case 32: return PA_SAMPLE_S16BE;
    default: return PA_SAMPLE_INVALID;
    }
}

struct audio_file *audio_file_new(char *filepath) {
    struct audio_file *file = NULL;
    struct stat filestat = { 0, };
    struct riff_header *riff_header = NULL;
    struct fmt_header *fmt_header = NULL;
    struct data_header *data_header = NULL;
    char *header = NULL,*tmp=NULL;
    pa_sample_format_t sample_format;
    size_t header_size, audio_size;
    int fd = -1, ret;

    file = malloc(sizeof(struct audio_file));
    if (!file)
        goto fail;

    memset(file, 0, sizeof(*file));

    fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        errorp("open('%s')", filepath);
        goto fail;
    }

    ret = fstat(fd, &filestat);
    if (ret < 0) {
        errorp("fstat('%s')", filepath);
        goto fail;
    }

    header_size = sizeof(struct riff_header)+sizeof(struct fmt_header)+sizeof(struct data_header);
    if (filestat.st_size < header_size) {
        error("Too small file size < WAV header %lu bytes", header_size);
        goto fail;
    }

    header = mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (header == MAP_FAILED) {
        errorp("mmap('%s')", filepath);
        goto fail;
    }

    riff_header = (struct riff_header*)header;
    fmt_header = (struct fmt_header*)(header+sizeof(struct riff_header));

    if (strncmp(riff_header->id, "RIFF", 4)) {
        error("File '%s' is not a WAV file", filepath);
        goto fail;
    }

    if (fmt_header->tag != 1) {
        error("Cannot play audio file '%s'", filepath);
        error("Audio data is not in raw, uncompressed, PCM format");
        goto fail;
    }

    tmp = (char*)(fmt_header+1);
    if(fmt_header->size == 16)tmp-=2;

    //skip unknown chunk
    while(1){
	char name[5]={0};
	strncpy(name,tmp,4);
	printf("meet %s\n",name);
    	if(strncmp(tmp,"data",4)==0)
		break;
	 int size = *(int*)(tmp+4);
	 tmp+=8+size;
    }

    data_header = (struct data_header*)tmp;

    sample_format = bits_per_sample_to_pa_spec_format(fmt_header->bits_per_sample);
    if (sample_format == PA_SAMPLE_INVALID) {
        error("Unrecognized WAV file format with %u bits per sample!",
              fmt_header->bits_per_sample);
        goto fail;
    }

    /* Guard against corrupted WAV files where the reported audio
     * data size is much larger than what's really in the file. */
    audio_size = min((size_t)data_header->size,
                     (filestat.st_size - ((char*)(data_header+1)-header)));

    file->buf = (void *)(data_header + 1);
    file->size = audio_size;
    file->readi = 0;
    file->spec.format = sample_format;
    file->spec.rate = fmt_header->samples_per_sec;
    file->spec.channels = fmt_header->channels;

    return file;

fail:
    if (header && header != MAP_FAILED) {
        assert(filestat.st_size > 0);
        munmap(header, filestat.st_size);
    }

    if (fd != -1)
        close(fd);

    if (file)
        free(file);

    return NULL;
}
