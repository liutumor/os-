#include <mips/cpu.h>
#include <asm/asm.h>
#include <pmap.h>
#include <env.h>
#include <printf.h>
#include <kclock.h>
#include <trap.h>

#define DDR_BASE_ADDR 0x80000000;

int mCONTEXT;

void interface_init()
{
    printf("\n");
    printf("\n");
    printf("\n");
printf("       db                                                                      \n");
printf("      d88b                                                                     \n");
printf("     d8\'`8b                                                                    \n");
printf("    d8\'  `8b     88       88  8b,dPPYba,   ,adPPYba,   8b,dPPYba,  ,adPPYYba,  \n");
printf("   d8YaaaaY8b    88       88  88P\'   \"Y8  a8\"     \"8a  88P\'   \"Y8  \"\"     `Y8  \n");
printf("  d8\"\"\"\"\"\"\"\"8b   88       88  88          8b       d8  88          ,adPPPPP88  \n");
printf(" d8\'        `8b  \"8a,   ,a88  88          \"8a,   ,a8\"  88          88,    ,88  \n");
printf("d8\'          `8b  `\"YbbdP\'Y8  88           `\"YbbdP\"\'   88          `\"8bbdP\"Y8  \n");
printf("\n");
    boot_music();
}
void time_setup()
{
    char * buf;
    char * h, *m, *s;
    printf("Please setup the system time(xx:xx:xx) \n");
    h = readline("hour> ");
    m = readline("minute> ");
    s = readline("sec> ");
    
    set_seven_seg_value(*s);
}
void device_init()
{
 	set_leds(0x5555); 
  	mips32_bicsr (SR_BEV);
    mips32_bissr (SR_IE | SR_HINT0 | SR_HINT1 | SR_HINT2 | SR_HINT3 | SR_HINT4);
    init_seven_seg();
    enable_all_seven_seg();
    //time_setup();
}
void sys_init()
{
    printf("\n");
	mips_tlbinvalall ();
    printf("*******Start to detect available memory space:\n");
	mips_detect_memory();
    printf("\n");
    printf("*******Start to initialize the virtual memory:\n");
	mips_vm_init();
    printf("\n");
    printf("*******Start to initialize page memory management:\n");
	page_init();
    page_check();
    printf("\n");
    printf("*******Start to initialize process management:\n");
	env_init();
    printf("\n");
//	env_check();
    printf("*******Start to load user program:\n");
    ENV_CREATE(user_idle);
    printf("\n");
    printf("*******Start to initialize traps:\n");
	trap_init();
    printf("\n");
    printf("*******Start to initialize kclock:\n");
    kclock_init();
    printf("kclock init has been completed\n"); 
    printf("\n");
    printf("*******The whole system is ready!\n");
   //  printf("kclock init has been completed\n");
    interface_init(); //initialize the interface
    asm ("ei");
    
    sched_yield();
  //  panic("!!!!!!!!!!!!!!!!!!");
	//while (1) monitor(NULL);
}


void SD_TEST()
{
    //spi_init();
    // for(uint8_t i = 0;i < 15; i++){
    //     set_leds(xchg_spi(i));
    //     for(uint32_t j = 0; j < 1000*00000; j++);
    // }
    disk_initialize(0);
}

