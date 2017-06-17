#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define BUFSIZE 1024

pa_sample_spec ss ={
  .format = PA_SAMPLE_S16LE,
  .rate = 44100,
  .channels =2
};

int main(int argc,char**argv)
{
  int fd,error,ret = -1;
  pa_simple *s = NULL;
  char *device = NULL;//NULL indicates default

  if((fd = open(argv[1],O_RDONLY)) < 0){
    fprintf(stderr, __FILE__": open() failed: %s\n", strerror(errno));
    goto finish;
  }

  if(!(s = pa_simple_new(NULL,"simpe-player",PA_STREAM_PLAYBACK,device,"playback",&ss,NULL,NULL,&error))){
    fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    goto finish;
  }

  for(;;){
    uint8_t buf[BUFSIZE];
    ssize_t r;

#if 1
    pa_usec_t latency;

    if((latency = pa_simple_get_latency(s,&error)) == (pa_usec_t)-1){
      fprintf(stderr, __FILE__": pa_simple_get_latency() failed: %s\n", pa_strerror(error));
      goto finish;
    }

    fprintf(stderr, __FILE__"%0.0f usec \r", (float)latency);
#endif

    if((r = read(fd,buf,sizeof(buf))) <= 0){
      if(r == 0)
        break;

      fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
      goto finish;
    }

    if(pa_simple_write(s,buf,(size_t)r,&error) < 0){
      fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
      goto finish;
    }
  }

  if(pa_simple_drain(s,&error) < 0){
    fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
    goto finish;
  }

  ret = 0;

 finish:
  if(s)
    pa_simple_free(s);

  return ret;
}
