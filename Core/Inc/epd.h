#ifndef __EPD_H_
#define __EPD_H_



#if defined(EPD_2IN9B_V3)
  #include "EPD_2in9b_V3.h"
#elif defined(EPD_2IN13D)
  #include "EPD_2in13d.h"
#elif defined(EPD_WF0213T1PCZ04)
  #include "EPD_WF0213T1PCZ04.h"
#elif defined(EPD_HINK)
  #include "EPD_HINK.h"
#elif defined(EPD_SSD1680)
  #include "EPD_SSD1680.h"
#else

#endif
#endif
