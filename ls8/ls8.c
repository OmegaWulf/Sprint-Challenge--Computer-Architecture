#include <stdio.h>
#include "cpu.h"




/**
 * Main
 */

int main(int argc, char *argv[])
{
    
    
    if (argc != 2) {
        fprintf(stderr, "usage: comp filename\n");
        return 1;
    }
    
    
    char *load_program = argv[1] ; 
    
    
  struct cpu cpu;
  cpu_init(&cpu);
  cpu_load(&cpu, load_program);
  cpu_run(&cpu);

    
  return 0;
}
