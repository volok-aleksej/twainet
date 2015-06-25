#ifndef CPU_ID_H
#define CPU_ID_H

#ifdef _WIN32

#include <intrin.h>

unsigned short getCpuHash()          
{        
   int cpuinfo[4] = { 0, 0, 0, 0 };                  
   __cpuid(cpuinfo, 0 );          
   unsigned short hash = 0;          
   unsigned short* ptr = (unsigned short*)(&cpuinfo[0]); 
   for (unsigned int i = 0; i < 8; i++ )
      hash += ptr[i];     

   return hash;           
} 

#elif DARWIN

#include <mach-o/arch.h>    

unsigned short getCpuHash()            
{         
	const NXArchInfo* info = NXGetLocalArchInfo();    
	unsigned short val = 0;            
	val += (unsigned short)info->cputype;               
	val += (unsigned short)info->cpusubtype;            
	return val;             
}

#else

static void getCpuid( unsigned int* p, unsigned int ax )       
{         
	__asm __volatile         
	(   "movl %%ebx, %%esi\n\t"               
		"cpuid\n\t"          
		"xchgl %%ebx, %%esi" 
		: "=a" (p[0]), "=S" (p[1]),           
		"=c" (p[2]), "=d" (p[3])            
		: "0" (ax)           
		);     
}         

unsigned short getCpuHash()            
{         
	unsigned int cpuinfo[4] = { 0, 0, 0, 0 };          
	getCpuid( cpuinfo, 0 );  
	unsigned short hash = 0;            
	unsigned int* ptr = (&cpuinfo[0]);                 
	for ( unsigned int i = 0; i < 4; i++ )             
		hash += (ptr[i] & 0xFFFF) + ( ptr[i] >> 16 );   

	return hash;             
}         

#endif/*_WIN32*/

#endif/*CPU_ID_H*/