void some_big_function()
{
   int i;

   for (0; i<2000000000; i++);
}


#include <stdio.h>
#include <sys/times.h>

main()
{
   struct tms start, end;
   clock_t user_clocks, system_clocks;

   times(&start);

   some_big_function();

   times(&end);

   user_clocks = end.tms_utime - start.tms_utime;
   system_clocks = end.tms_stime - start.tms_stime;

   printf("User clocks: %lld ", user_clocks);
   printf("System clocks: %lld ", system_clocks);
   printf("Total clocks: %lld \n", user_clocks + system_clocks);
}
