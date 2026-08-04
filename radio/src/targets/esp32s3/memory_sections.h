#pragma once
// ESP32-S3: no CCM; place "fast" sections in normal DRAM / IRAM via IDF attrs when needed.
#ifndef __CCMRAM
#define __CCMRAM
#endif
#ifndef __SDRAM
#define __SDRAM
#endif
#ifndef __NOINIT
#define __NOINIT
#endif
