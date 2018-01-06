#include "fc_layer.h"
#include "util.h"

void fc_forward_32(struct layer* l, float* src, float* dest, float* workspace)
{
  int N = l->output_c;
  setvcfg(0, 2, 0, 1);
  float* weights = l->weights_32;
  for (int j = 0; j < N; )
    {
      int consumed = setvlen(N - j);
      asm volatile ("vmca va0, %0" : : "r" (&dest[j]));
      asm volatile ("la t0, vfc_32_pre" : : : "t0");
      asm volatile ("lw t1, 0(t0)");
      asm volatile ("vf 0(t0)");
     
      for (int i = 0; i < l->w*l->h*l->c; i++)
        {
          if (src[i] != 0.0)
            {
              asm volatile ("vmca va1, %0" : : "r" (&weights[i*N+j]));
              asm volatile ("vmcs vs1, %0" : : "r" (src[i]));
              asm volatile ("la t0, vfc_32" : : : "t0");
              asm volatile ("lw t1, 0(t0)");
              asm volatile ("vf 0(t0)");
            }
        }
      asm volatile ("la t0, vfc_32_post" : : : "t0");
      asm volatile ("lw t1, 0(t0)");
      asm volatile ("vf 0(t0)");
      j += consumed;
    }
  asm volatile ("fence");

  printf("%.8f %.8f fc\n", dest[0], dest[999]);
}

void fc_forward_encoded_32(struct layer* l, float* src, float* dest, float* workspace)
{
  int N = l->output_c;
  setvcfg(0, 2, 0, 1);
  float* codebook = l->weights_32;
  int8_t* weights = l->encoded_indices;
  for (int j = 0; j < N; )
    {
      int consumed = setvlen(N - j);
      asm volatile ("vmca va0, %0" : : "r" (&dest[j]));
      asm volatile ("vmcs vs2, %0" : : "r" (&codebook));
      asm volatile ("la t0, vfc_encoded_32_pre" : : : "t0");
      asm volatile ("lw t1, 0(t0)");
      asm volatile ("vf 0(t0)");
     
      for (int i = 0; i < l->w*l->h*l->c; i++)
        {
          if (src[i] != 0.0)
            {
              asm volatile ("vmca va1, %0" : : "r" (&weights[i*N+j]));
              asm volatile ("vmcs vs1, %0" : : "r" (src[i]));
              asm volatile ("la t0, vfc_encoded_32" : : : "t0");
              asm volatile ("lw t1, 0(t0)");
              asm volatile ("vf 0(t0)");
            }
        }
      asm volatile ("la t0, vfc_encoded_32_post" : : : "t0");
      asm volatile ("lw t1, 0(t0)");
      asm volatile ("vf 0(t0)");
      j += consumed;
    }
  asm volatile ("fence");

  printf("%.8f %.8f fc\n", dest[0], dest[999]);
}
