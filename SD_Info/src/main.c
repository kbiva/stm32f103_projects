/*
 *  main.c
 *
 *  SD card information
 *
 *  Author: Kestutis Bivainis
 *
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>
#include "LGDP4532.h"
#include "colors.h"
#include "sd_card_func.h"
#include "stm3210e_eval_sdio_sd.h"

extern SD_CardInfo SDCardInfo;

int main (void) {

  SD_Error Status;
  uint32_t i;
  char buf[128];

  DWT_Init();

  LGDP4532_Init(0,2);
  LGDP4532_ColorMode(COLOR_18BIT);
  LGDP4532_OrientationMode(ORIENTATION_LANDSCAPE);
  LGDP4532_ClearScreen(BLACK);
  LGDP4532_SetFont(FONT_8x14);

  SD_IRQ_Conf();

  LGDP4532_PutStrCentered("SD Card Identification Data",0);
  LGDP4532_SetFont(FONT_6x8);
  Status = SD_Init();
  if(Status != SD_OK) {
    LGDP4532_PutStr("Error initializing SD Card",0,15);
    while(1){};
  }

  Status = SD_GetCardInfo(&SDCardInfo);
  if (Status == SD_OK) {

    LGDP4532_SetTextColors(ORANGE,BLACK);
    sprintf(buf,"Capacity:%llu(bytes)",SDCardInfo.CardCapacity);
    LGDP4532_PutStr(buf,0,15);

    sprintf(buf,"%llu(Kbytes)",SDCardInfo.CardCapacity/1024);
    LGDP4532_PutStr(buf,54,24);

    sprintf(buf,"%llu(Mbytes)",SDCardInfo.CardCapacity/(1024*1024));
    LGDP4532_PutStr(buf,54,33);

    sprintf(buf,"%.2f(Gbytes)",(((float)SDCardInfo.CardCapacity/(1024*1024))/1024));
    LGDP4532_PutStr(buf,54,42);

    LGDP4532_SetTextColors(WHITE,BLACK);
    sprintf(buf,"BlockSize:%d (bytes)",SDCardInfo.CardBlockSize);
    LGDP4532_PutStr(buf,0,51);

    sprintf(buf,"RCA:0x%04X",SDCardInfo.RCA);
    LGDP4532_PutStr(buf,0,60);

    LGDP4532_SetTextColors(DEEPPINK,BLACK);
    sprintf(buf,"ManufacturerID:%d",SDCardInfo.SD_cid.ManufacturerID);
    LGDP4532_PutStr(buf,0,69);
    // This is unofficial list
    switch(SDCardInfo.SD_cid.ManufacturerID){
      case 1:sprintf(buf,"(Panasonic)");break;
      case 2:sprintf(buf,"(Toshiba)");break;
      case 3:sprintf(buf,"(SanDisk)");break;
      case 24:sprintf(buf,"(Lexar)");break;
      case 27:sprintf(buf,"(Samsung)");break;
      case 57:sprintf(buf,"(Patriot)");break;
      default:sprintf(buf,"(unknown)");break;
    }
    LGDP4532_PutStr(buf,108,69);

    LGDP4532_PutStr("Type:",0,78);
    switch(SDCardInfo.CardType){
      case 0:sprintf(buf,"%d (SD Card v1.1)",SDCardInfo.CardType);break;
      case 1:sprintf(buf,"%d (SD Card v2.0)",SDCardInfo.CardType);break;
      case 2:sprintf(buf,"%d (SDHC Card)",SDCardInfo.CardType);break;
      default:sprintf(buf,"%d (unknown)",SDCardInfo.CardType);break;
    }
    LGDP4532_PutStr(buf,30,78);

    sprintf(buf,"OEM_AppliID:%c%c",SDCardInfo.SD_cid.OEM_AppliID>>8,
                                   SDCardInfo.SD_cid.OEM_AppliID&0x00FF);
    LGDP4532_PutStr(buf,0,87);

    sprintf(buf,"ProdName:%c%c%c%c%c",SDCardInfo.SD_cid.ProdName1>>24,
                                      (SDCardInfo.SD_cid.ProdName1&0x00FF0000)>>16,
                                      (SDCardInfo.SD_cid.ProdName1&0x0000FF00)>>8,
                                      SDCardInfo.SD_cid.ProdName1&0x000000FF,
                                      SDCardInfo.SD_cid.ProdName2);
    LGDP4532_PutStr(buf,0,96);

    LGDP4532_SetTextColors(WHITE,BLACK);
    sprintf(buf,"ProdRev:%d.%d",SDCardInfo.SD_cid.ProdRev>>4,
                                SDCardInfo.SD_cid.ProdRev&0x0F);
    LGDP4532_PutStr(buf,0,105);

    sprintf(buf,"ProdSN:0x%08X",SDCardInfo.SD_cid.ProdSN);
    LGDP4532_PutStr(buf,0,114);

    LGDP4532_SetTextColors(GREEN,BLACK);
    sprintf(buf,"ManufactDate:%d-%02d",(SDCardInfo.SD_cid.ManufactDate>>4)+2000,
                                       SDCardInfo.SD_cid.ManufactDate&0x000F);
    LGDP4532_PutStr(buf,0,123);

    LGDP4532_SetTextColors(WHITE,BLACK);
    sprintf(buf,"CID_CRC:0x%02X",SDCardInfo.SD_cid.CID_CRC);
    LGDP4532_PutStr(buf,0,132);

    sprintf(buf,"CSDStruct:%d",SDCardInfo.SD_csd.CSDStruct);
    LGDP4532_PutStr(buf,0,141);
    if(SDCardInfo.SD_csd.CSDStruct==0) {
      LGDP4532_PutStr("(CSD ver. 1.0)",72,141);
    } else if(SDCardInfo.SD_csd.CSDStruct==1) {
      LGDP4532_PutStr("(CSD ver. 2.0)",72,141);
    }  else {
      LGDP4532_PutStr("(unknown)",72,141);
    }

    sprintf(buf,"SysSpecVersion:%d",SDCardInfo.SD_csd.SysSpecVersion);
    LGDP4532_PutStr(buf,0,150);

    sprintf(buf,"TAAC:0x%02X (",SDCardInfo.SD_csd.TAAC);
    LGDP4532_PutStr(buf,0,159);
    switch(SDCardInfo.SD_csd.TAAC>>3){
      case 0:LGDP4532_PutStr("reserved",66,159);break;
      case 1:LGDP4532_PutStr("1.0",66,159);break;
      case 2:LGDP4532_PutStr("1.2",66,159);break;
      case 3:LGDP4532_PutStr("1.3",66,159);break;
      case 4:LGDP4532_PutStr("1.5",66,159);break;
      case 5:LGDP4532_PutStr("2.0",66,159);break;
      case 6:LGDP4532_PutStr("2.5",66,159);break;
      case 7:LGDP4532_PutStr("3.0",66,159);break;
      case 8:LGDP4532_PutStr("3.5",66,159);break;
      case 9:LGDP4532_PutStr("4.0",66,159);break;
      case 10:LGDP4532_PutStr("4.5",66,159);break;
      case 11:LGDP4532_PutStr("5.0",66,159);break;
      case 12:LGDP4532_PutStr("5.5",66,159);break;
      case 13:LGDP4532_PutStr("6.0",66,159);break;
      case 14:LGDP4532_PutStr("7.0",66,159);break;
      case 15:LGDP4532_PutStr("8.0",66,159);break;
    }
    switch(SDCardInfo.SD_csd.TAAC&0x07){
      case 0:LGDP4532_PutStr("(1ns))",90,159);break;
      case 1:LGDP4532_PutStr("(10ns))",90,159);break;
      case 2:LGDP4532_PutStr("(100ns))",90,159);break;
      case 3:LGDP4532_PutStr("(1us))",90,159);break;
      case 4:LGDP4532_PutStr("(10us))",90,159);break;
      case 5:LGDP4532_PutStr("(100us))",90,159);break;
      case 6:LGDP4532_PutStr("(1ms))",90,159);break;
      case 7:LGDP4532_PutStr("(10ms))",90,159);break;
    }

    sprintf(buf,"NSAC:%d",SDCardInfo.SD_csd.NSAC);
    LGDP4532_PutStr(buf,0,168);

    sprintf(buf,"MaxBusClkFrec:0x%02X",SDCardInfo.SD_csd.MaxBusClkFrec);
    LGDP4532_PutStr(buf,0,177);
    switch(SDCardInfo.SD_csd.MaxBusClkFrec) {
      case 0x32:LGDP4532_PutStr("(25Mhz)",114,177);break;
      case 0x5a:LGDP4532_PutStr("(50Mhz)",114,177);break;
      default:LGDP4532_PutStr("(unknown)",114,177);break;
    }

    sprintf(buf,"CardComdClasses:0x%03X",SDCardInfo.SD_csd.CardComdClasses);
    LGDP4532_PutStr(buf,0,186);

    sprintf(buf,"RdBlockLen:%d (%db)",SDCardInfo.SD_csd.RdBlockLen,
                                     1<<SDCardInfo.SD_csd.RdBlockLen);
    LGDP4532_PutStr(buf,0,195);

    sprintf(buf,"PartBlockRead:%d",SDCardInfo.SD_csd.PartBlockRead);
    LGDP4532_PutStr(buf,0,204);
    if(SDCardInfo.SD_csd.PartBlockRead==0) {
      LGDP4532_PutStr("(no)",96,204);
    } else if(SDCardInfo.SD_csd.PartBlockRead==1) {
      LGDP4532_PutStr("(yes)",96,204);
    }  else {
      LGDP4532_PutStr("(unknown)",96,204);
    }

    sprintf(buf,"WrBlockMisalign:%d",SDCardInfo.SD_csd.WrBlockMisalign);
    LGDP4532_PutStr(buf,0,213);
    if(SDCardInfo.SD_csd.WrBlockMisalign==0) {
      LGDP4532_PutStr("(no)",108,213);
    } else if(SDCardInfo.SD_csd.WrBlockMisalign==1) {
      LGDP4532_PutStr("(yes)",108,213);
    }  else {
      LGDP4532_PutStr("(unknown)",108,213);
    }

    sprintf(buf,"RdBlockMisalign:%d",SDCardInfo.SD_csd.RdBlockMisalign);
    LGDP4532_PutStr(buf,0,222);
    if(SDCardInfo.SD_csd.RdBlockMisalign==0) {
      LGDP4532_PutStr("(no)",108,222);
    } else if(SDCardInfo.SD_csd.RdBlockMisalign==1) {
      LGDP4532_PutStr("(yes)",108,222);
    }  else {
      LGDP4532_PutStr("(unknown)",108,222);
    }

    sprintf(buf,"DSRImpl:%d",SDCardInfo.SD_csd.DSRImpl);
    LGDP4532_PutStr(buf,0,231);
    if(SDCardInfo.SD_csd.DSRImpl==0) {
      LGDP4532_PutStr("(no)",60,231);
    } else if(SDCardInfo.SD_csd.DSRImpl==1) {
      LGDP4532_PutStr("(yes)",60,231);
    } else {
      LGDP4532_PutStr("(unknown)",60,231);
    }

    sprintf(buf,"DeviceSize:%d",SDCardInfo.SD_csd.DeviceSize);
    LGDP4532_PutStr(buf,160,33);
    i=strlen(buf);
    if(SDCardInfo.SD_csd.CSDStruct==0) {
      sprintf(buf,"(%dMb)",(SDCardInfo.SD_csd.DeviceSize+1)*(1<<(SDCardInfo.SD_csd.DeviceSizeMul+2))*(1<<SDCardInfo.SD_csd.RdBlockLen)/(1024*1024));
    } else if(SDCardInfo.SD_csd.CSDStruct==1) {
      sprintf(buf,"(%dMb)",(SDCardInfo.SD_csd.DeviceSize+1)*512/1024);
    } else {
      sprintf(buf,"(unknown)");
    }
    LGDP4532_PutStr(buf,160+i*6,33);

    if(SDCardInfo.SD_csd.CSDStruct==0) {
      // defined only in CSD Version 1.0
      sprintf(buf,"MaxRdCurrentVDDMin:%d",SDCardInfo.SD_csd.MaxRdCurrentVDDMin);
      LGDP4532_PutStr(buf,160,42);
      switch(SDCardInfo.SD_csd.MaxRdCurrentVDDMin){
        case 0:LGDP4532_PutStr("(0.5mA)",280,42);break;
        case 1:LGDP4532_PutStr("(1mA)",280,42);break;
        case 2:LGDP4532_PutStr("(5mA)",280,42);break;
        case 3:LGDP4532_PutStr("(10mA)",280,42);break;
        case 4:LGDP4532_PutStr("(25mA)",280,42);break;
        case 5:LGDP4532_PutStr("(35mA)",280,42);break;
        case 6:LGDP4532_PutStr("(60mA)",280,42);break;
        case 7:LGDP4532_PutStr("(100mA)",280,42);break;
        default:LGDP4532_PutStr("(unknown)",280,42);break;
      }

      sprintf(buf,"MaxRdCurrentVDDMax:%d",SDCardInfo.SD_csd.MaxRdCurrentVDDMax);
      LGDP4532_PutStr(buf,160,51);
      switch(SDCardInfo.SD_csd.MaxRdCurrentVDDMax){
        case 0:LGDP4532_PutStr("(1mA)",280,51);break;
        case 1:LGDP4532_PutStr("(5mA)",280,51);break;
        case 2:LGDP4532_PutStr("(10mA)",280,51);break;
        case 3:LGDP4532_PutStr("(25mA)",280,51);break;
        case 4:LGDP4532_PutStr("(35mA)",280,51);break;
        case 5:LGDP4532_PutStr("(45mA)",280,51);break;
        case 6:LGDP4532_PutStr("(80mA)",280,51);break;
        case 7:LGDP4532_PutStr("(200mA)",280,51);break;
        default:LGDP4532_PutStr("(unknown)",280,51);break;
      }

      sprintf(buf,"MaxWrCurrentVDDMin:%d",SDCardInfo.SD_csd.MaxWrCurrentVDDMin);
      LGDP4532_PutStr(buf,160,60);
      switch(SDCardInfo.SD_csd.MaxWrCurrentVDDMin){
        case 0:LGDP4532_PutStr("(0.5mA)",280,60);break;
        case 1:LGDP4532_PutStr("(1mA)",280,60);break;
        case 2:LGDP4532_PutStr("(5mA)",280,60);break;
        case 3:LGDP4532_PutStr("(10mA)",280,60);break;
        case 4:LGDP4532_PutStr("(25mA)",280,60);break;
        case 5:LGDP4532_PutStr("(35mA)",280,60);break;
        case 6:LGDP4532_PutStr("(60mA)",280,60);break;
        case 7:LGDP4532_PutStr("(100mA)",280,60);break;
        default:LGDP4532_PutStr("(unknown)",280,60);break;
      }

      sprintf(buf,"MaxWrCurrentVDDMax:%d",SDCardInfo.SD_csd.MaxWrCurrentVDDMax);
      LGDP4532_PutStr(buf,160,69);
      switch(SDCardInfo.SD_csd.MaxWrCurrentVDDMax){
        case 0:LGDP4532_PutStr("(1mA)",280,69);break;
        case 1:LGDP4532_PutStr("(5mA)",280,69);break;
        case 2:LGDP4532_PutStr("(10mA)",280,69);break;
        case 3:LGDP4532_PutStr("(25mA)",280,69);break;
        case 4:LGDP4532_PutStr("(35mA)",280,69);break;
        case 5:LGDP4532_PutStr("(45mA)",280,69);break;
        case 6:LGDP4532_PutStr("(80mA)",280,69);break;
        case 7:LGDP4532_PutStr("(200mA)",280,69);break;
        default:LGDP4532_PutStr("(unknown)",280,69);break;
      }
    }
    sprintf(buf,"DeviceSizeMul:%d",SDCardInfo.SD_csd.DeviceSizeMul);
    LGDP4532_PutStr(buf,160,78);
    switch(SDCardInfo.SD_csd.DeviceSizeMul) {
      case 0:LGDP4532_PutStr("(x4)",256,78);break;
      case 1:LGDP4532_PutStr("(x8)",256,78);break;
      case 2:LGDP4532_PutStr("(x16)",256,78);break;
      case 3:LGDP4532_PutStr("(x32)",256,78);break;
      case 4:LGDP4532_PutStr("(x64)",256,78);break;
      case 5:LGDP4532_PutStr("(x128)",256,78);break;
      case 6:LGDP4532_PutStr("(x256)",256,78);break;
      case 7:LGDP4532_PutStr("(x512)",256,78);break;
      default:LGDP4532_PutStr("(unknown)",256,78);break;
    }

    sprintf(buf,"EraseGrSize:%d",SDCardInfo.SD_csd.EraseGrSize);
    LGDP4532_PutStr(buf,160,87);

    sprintf(buf,"EraseGrMul:%d",SDCardInfo.SD_csd.EraseGrMul);
    LGDP4532_PutStr(buf,160,96);

    sprintf(buf,"WrProtectGrSize:%d",SDCardInfo.SD_csd.WrProtectGrSize);
    LGDP4532_PutStr(buf,160,105);

    sprintf(buf,"WrProtectGrEnable:%d",SDCardInfo.SD_csd.WrProtectGrEnable);
    LGDP4532_PutStr(buf,160,114);
    if(SDCardInfo.SD_csd.WrProtectGrEnable==0) {
      LGDP4532_PutStr("(no)",280,114);
    } else if(SDCardInfo.SD_csd.WrProtectGrEnable==1) {
      LGDP4532_PutStr("(yes)",280,114);
    }  else {
      LGDP4532_PutStr("(unknown)",280,114);
    }

    sprintf(buf,"ManDeflECC:%d",SDCardInfo.SD_csd.ManDeflECC);
    LGDP4532_PutStr(buf,160,123);

    sprintf(buf,"WrSpeedFact:%d (x%d)",SDCardInfo.SD_csd.WrSpeedFact,1<<SDCardInfo.SD_csd.WrSpeedFact);
    LGDP4532_PutStr(buf,160,132);

    sprintf(buf,"MaxWrBlockLen:%d (%db)",SDCardInfo.SD_csd.MaxWrBlockLen,1<<SDCardInfo.SD_csd.MaxWrBlockLen);
    LGDP4532_PutStr(buf,160,141);

    sprintf(buf,"WriteBlockPartial:%d",SDCardInfo.SD_csd.WriteBlockPaPartial);
    LGDP4532_PutStr(buf,160,150);
    if(SDCardInfo.SD_csd.WriteBlockPaPartial==0) {
      LGDP4532_PutStr("(no)",280,150);
    } else if(SDCardInfo.SD_csd.WriteBlockPaPartial==1) {
      LGDP4532_PutStr("(yes)",280,150);
    }  else {
      LGDP4532_PutStr("(unknown)",280,150);
    }

    sprintf(buf,"ContentProtectAppli:%d",SDCardInfo.SD_csd.ContentProtectAppli);
    LGDP4532_PutStr(buf,160,159);

    sprintf(buf,"FileFormatGroup:%d",SDCardInfo.SD_csd.FileFormatGrouop);
    LGDP4532_PutStr(buf,160,168);

    sprintf(buf,"CopyFlag:%d",SDCardInfo.SD_csd.CopyFlag);
    LGDP4532_PutStr(buf,160,177);
    if(SDCardInfo.SD_csd.CopyFlag==0) {
      LGDP4532_PutStr("(original)",226,177);
    } else if(SDCardInfo.SD_csd.CopyFlag==1) {
      LGDP4532_PutStr("(copied)",226,177);
    }  else {
      LGDP4532_PutStr("(unknown)",226,177);
    }

    sprintf(buf,"PermWrProtect:%d",SDCardInfo.SD_csd.PermWrProtect);
    LGDP4532_PutStr(buf,160,186);
    if(SDCardInfo.SD_csd.PermWrProtect==0) {
      LGDP4532_PutStr("(no)",256,186);
    } else if(SDCardInfo.SD_csd.PermWrProtect==1) {
      LGDP4532_PutStr("(yes)",256,186);
    }  else {
      LGDP4532_PutStr("(unknown)",256,186);
    }

    sprintf(buf,"TempWrProtect:%d",SDCardInfo.SD_csd.TempWrProtect);
    LGDP4532_PutStr(buf,160,195);
    if(SDCardInfo.SD_csd.TempWrProtect==0) {
      LGDP4532_PutStr("(no)",256,195);
    } else if(SDCardInfo.SD_csd.TempWrProtect==1) {
      LGDP4532_PutStr("(yes)",256,195);
    }  else {
      LGDP4532_PutStr("(unknown)",256,195);
    }

    sprintf(buf,"FileFormat:%d",SDCardInfo.SD_csd.FileFormat);
    LGDP4532_PutStr(buf,160,204);
    switch(SDCardInfo.SD_csd.FileFormatGrouop){
      case 0:
        switch(SDCardInfo.SD_csd.FileFormat) {
          case 0:LGDP4532_PutStr("(Hard disk-like file syst.)",160,213);break;
          case 1:LGDP4532_PutStr("(Floppy-like file system)",160,213);break;
          case 2:LGDP4532_PutStr("(Universal File Format)",160,213);break;
          case 3:LGDP4532_PutStr("(Others/Unknown)",160,213);break;
        };break;
      case 1:LGDP4532_PutStr("(reserved)",160,213);
    }

    sprintf(buf,"ECC:%d",SDCardInfo.SD_csd.ECC);
    LGDP4532_PutStr(buf,160,222);

    sprintf(buf,"CSD_CRC:%02X",SDCardInfo.SD_csd.CSD_CRC);
    LGDP4532_PutStr(buf,160,231);
  } else {
    LGDP4532_PutStr("Error getting SD Card info",0,15);
  }

  while(1){};
}
