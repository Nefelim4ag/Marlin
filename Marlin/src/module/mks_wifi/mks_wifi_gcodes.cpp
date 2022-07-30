#include "mks_wifi_gcodes.h"
#include "../../module/settings.h"
#include "../../lcd/marlinui.h"


const uint8_t pak[5]={0xA5,0x07,0x00,0x00,0xFC};

const char EOL_STR[] =				"\r\n";
const char OK_STR[] =					"ok";

const char M997_IDLE[] =			"M997 IDLE\r\n";
const char M997_PRINTING[] =	"M997 PRINTING\r\n";
const char M997_PAUSE[] =			"M997 PAUSE\r\n";

const char M115_FIRMWARE[] =	"FIRMWARE_NAME: MarlinRB ";





void mks_m20(bool longnames)
{
	if(IS_SD_PRINTING())
		return;

	mks_wifi_out_add ((uint8_t*)STR_BEGIN_FILE_LIST, strlen(STR_BEGIN_FILE_LIST));
	mks_wifi_out_add ((uint8_t*)EOL_STR, sizeof(EOL_STR));
	if (card.isMounted())
	{
		card.ls(TERN_(LONG_FILENAME_HOST_SUPPORT, longnames));
	}
	mks_wifi_out_add ((uint8_t*)STR_END_FILE_LIST, strlen(STR_END_FILE_LIST));
	mks_wifi_out_add ((uint8_t*)EOL_STR, sizeof(EOL_STR));
}



void mks_m23(char *filename)
{
	card.openFileRead(filename);

	mks_wifi_out_add((uint8_t*)OK_STR, sizeof(EOL_STR));
	mks_wifi_out_add ((uint8_t*)EOL_STR, sizeof(EOL_STR));
}


// Stop printing
void mks_m26()
{
	if (CardReader::isPrinting())
	{
		ui.abort_print();
	}
	mks_wifi_out_add((uint8_t*)OK_STR, sizeof(EOL_STR));
	mks_wifi_out_add ((uint8_t*)EOL_STR, sizeof(EOL_STR));
}



void mks_m27(void)
{
	char tempBuf[16];
	if (CardReader::isPrinting())
	{
		uint8_t prog = ui.get_progress_percent();
		sprintf((char *)tempBuf,"M27 %d\r\n", prog);
		mks_wifi_out_add((uint8_t*)tempBuf, strlen(tempBuf));
	}
}



void mks_m30(char *filename)
{
	
	filename[0]='0';
	sd_delete_file(filename);

	mks_wifi_out_add((uint8_t*)OK_STR, sizeof(EOL_STR));
	mks_wifi_out_add ((uint8_t*)EOL_STR, sizeof(EOL_STR));
}



void mks_m105(void)
{
	char tempBuf[100];
	const int8_t target_extruder = GcodeSuite::get_target_extruder_from_command();
	if (target_extruder < 0) return;

	memset(tempBuf,0,100);

	sprintf((char *)tempBuf,"T:%d /%d B:%d /%d T0:%d /%d T1:0 /0 @:45 B@:1\r\n", 
	(int)Temperature::degHotend(target_extruder), (int)Temperature::degTargetHotend(target_extruder),
	(int)Temperature::degBed(), (int)Temperature::degTargetBed(),
	(int)Temperature::degHotend(target_extruder), (int)Temperature::degTargetHotend(target_extruder));
	mks_wifi_out_add((uint8_t *)tempBuf,strlen(tempBuf));

}



void mks_m115(void)
{
	mks_wifi_out_add((uint8_t *)M115_FIRMWARE, sizeof(M115_FIRMWARE));
	mks_wifi_out_add((uint8_t *)SHORT_BUILD_VERSION, sizeof(SHORT_BUILD_VERSION));

/* 	sprintf(version, "EPROM datasize: %u bytes", settings.datasize());
	strcat(version, "\r\n");
	mks_wifi_out_add((uint8_t *)version,strlen(version));
 */

	mks_wifi_out_add ((uint8_t*)EOL_STR, sizeof(EOL_STR));
}



void mks_m991(void)
{
	char tempBuf[100];

  const int8_t target_extruder = GcodeSuite::get_target_extruder_from_command();
  if (target_extruder < 0)
		return;

	sprintf((char *)tempBuf,"T:%d /%d B:%d /%d T0:%d /%d T1:0 /0 @:85 B@:1\r\n", 
	(int)Temperature::degHotend(target_extruder), (int)Temperature::degTargetHotend(target_extruder),
	(int)Temperature::degBed(), (int)Temperature::degTargetBed(),
	(int)Temperature::degHotend(target_extruder), (int)Temperature::degTargetHotend(target_extruder));
	mks_wifi_out_add((uint8_t *)tempBuf,strlen(tempBuf));

}



void mks_m992(void)
{
  char buffer[30];

  if(CardReader::isPrinting())
	{
		uint32_t duration_sec = print_job_timer.duration();
		uint16_t hours = duration_sec / 3600;
		uint16_t minutes = (duration_sec - hours*3600) / 60;
		uint16_t seconds = (duration_sec - hours*3600 - minutes*60);

    sprintf((char *)buffer, "M992 %02d:%02d:%02d\r\n", hours, minutes, seconds);
    mks_wifi_out_add((uint8_t *)buffer,strlen(buffer));
  };
}



void mks_m994(void)
{
  char buffer[256];

	buffer[0] = 0;
  strncat(buffer, "M994 ", 8);
	uint32_t l = strlen(buffer);
  if(card.isFileOpen())
	{
		card.getCurrentDir(buffer+l, sizeof(buffer)-l-1);
		l = strlen(buffer);
		if (buffer[l-1] != '/')
		{
			buffer[l++] = '/';
			buffer[l] = 0;
		}
    strncat(buffer+l, card.longest_filename(), sizeof(buffer) - l - 1);
		l = strlen(buffer);
		buffer[l++] = ';';
		buffer[l] = 0;
    sprintf(buffer+l, "%lu\r\n", card.getFileSize()*1000);
		l = strlen(buffer);

    mks_wifi_out_add((uint8_t *)buffer,strlen(buffer));
  }
  
}



void mks_m997(void){
	if(CardReader::isPrinting()){
		mks_wifi_out_add((uint8_t *)M997_PRINTING,strlen(M997_PRINTING));
	}else if(CardReader::isPaused()){
		mks_wifi_out_add((uint8_t *)M997_PAUSE,strlen(M997_PAUSE));
	}else{
		mks_wifi_out_add((uint8_t *)M997_IDLE,strlen(M997_IDLE));
	}
}

