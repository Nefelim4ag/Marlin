/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "../inc/MarlinConfig.h"
#include "../libs/fatfs/fatfs_shared.h"

#if ENABLED(SDSUPPORT)

extern const char M23_STR[], M24_STR[];

#if ENABLED(SDCARD_RATHERRECENTFIRST) && DISABLED(SDCARD_SORT_ALPHA)
  #define SD_ORDER(N,C) ((C) - 1 - (N))
#else
  #define SD_ORDER(N,C) N
#endif

#define MAX_DIR_DEPTH     6       // Maximum folder depth
#define MAXDIRNAMELENGTH  128       // DOS folder name size
#define MAXPATHNAMELENGTH  (1 + (MAXDIRNAMELENGTH + 1) * (MAX_DIR_DEPTH) + 1 + FF_SFN_BUF) // "/" + N * ("ADIRNAME/") + "filename.ext"

typedef struct {
  bool saving:1,
       logging:1,
       sdprinting:1,
       sdprintdone:1,
       filenameIsDir:1,
       abort_sd_printing:1
    ;
} card_flags_t;

typedef struct
{
  uint8_t   items[MAX_DIR_DEPTH];
  uint8_t   current;
} dir_active_items_t;

#if ENABLED(AUTO_REPORT_SD_STATUS)
  #include "../libs/autoreport.h"
#endif

class CardReader {
public:
  static card_flags_t flag;                         // Flags (above)
  // static char filename[FF_SFN_BUF],            // DOS 8.3 filename of the selected item
  //             longFilename[FF_LFN_BUF];   // Long name of the selected item


  // // // Methods // // //

  CardReader();


  static void mount(bool wifi = false);
  static void release();
  static bool isMounted() { return FATFS_sd.fs_type != 0; }
  static bool isRootDir();

  // Handle media insert/remove
  static void manage_media();

  // SD Card Logging
  /*
  static void openLogFile(const char * const path);
*/

  static void write_command(char * const buf);

  #if DISABLED(NO_SD_AUTOSTART)     // Auto-Start auto#.g file handling
    static uint8_t autofile_index;  // Next auto#.g index to run, plus one. Ignored by autofile_check when zero.
    static void autofile_begin();   // Begin check. Called automatically after boot-up.
    static bool autofile_check();   // Check for the next auto-start file and run it.
    static void autofile_cancel() { autofile_index = 0; }
  #endif

  // Basic file ops
  static void openFileRead(const char * const path, const uint8_t subcall=0);
/*
  static void openFileWrite(const char * const path);
  static bool fileExists(const char * const name);
*/
  static void removeFile(const char * const name);
  static void closefile(const bool store_location=false);
  static char* longest_filename() { return curfilinfo.fname[0] ? curfilinfo.fname : curfilinfo.altname; }

  #if ENABLED(LONG_FILENAME_HOST_SUPPORT)
    static void printLongPath(char * const path);   // Used by M33
  #endif

  // Working Directory for SD card menu
  static void cd(const char *relpath);
  static int8_t cdup();

/*
  static uint16_t countFilesInWorkDir();
*/

  // Select a file
  static void selectFileByIndex(const uint16_t nr);
/*
  static void selectFileByName(const char * const match);  // (working directory only)

  // Print job
  static void getAbsFilenameInCWD(char *dst);
  static void cdroot();
*/
  static void openAndPrintFile(const char *name);   // (working directory or full path)
  static void report_status();

  static void fileHasFinished();
  static uint16_t get_num_Files();
  static void endFilePrintNow();
  static void abortFilePrintSoon() { flag.abort_sd_printing = isFileOpen(); }
  static void printSelectedFilename();
  static void startOrResumeFilePrinting();
  static void pauseSDPrint()       { flag.sdprinting = false; }
  static void abortFilePrintNow();

  static bool isPrinting()         { return flag.sdprinting; }
  static bool isPaused()           { return isFileOpen() && !isPrinting(); }

  #if HAS_PRINT_PROGRESS_PERMYRIAD
    static uint16_t permyriadDone() {
      if (flag.sdprintdone) return 10000;
      if (isFileOpen() && filesize) return sdpos / ((filesize + 9999) / 10000);
      return 0;
    }
  #endif
  static uint8_t percentDone() {
    if (flag.sdprintdone) return 100;
    if (isFileOpen() && curfilinfo.fsize) return curfile.fptr / ((curfilinfo.fsize + 99) / 100);
    return 0;
  }

/*
  //  * Dive down to a relative or absolute path.
  //  * Relative paths apply to the workDir.
  //  *
  //  * update_cwd: Pass 'true' to update the workDir on success.
  //  *   inDirPtr: On exit your pointer points to the target SdFile.
  //  *             A nullptr indicates failure.
  //  *       path: Start with '/' for abs path. End with '/' to get a folder ref.
  //  *       echo: Set 'true' to print the path throughout the loop.

  static const char* diveToFile(const bool update_cwd, SdFile* &inDirPtr, const char * const path, const bool echo=false);
*/

  FORCE_INLINE static void getfilename_sorted(const uint16_t nr) { selectFileByIndex(nr); }
  FORCE_INLINE static bool isFileMustShow(FILINFO *finfo);
  FORCE_INLINE static bool isDirMustShow(FILINFO *finfo);

  static void ls(bool includeLongNames = true);

/*
  #if ENABLED(POWER_LOSS_RECOVERY)
    static bool jobRecoverFileExists();
    static void openJobRecoveryFile(const bool read);
    static void removeJobRecoveryFile();
  #endif

  // Current Working Dir - Set by cd, cdup, cdroot, and diveToFile(true, ...)
  static char* getWorkDirName()  { workDir.getDosName(filename); return filename; }
  static SdFile& getWorkDir()    { return workDir.isOpen() ? workDir : root; }
*/

  // Print File stats
  static uint32_t getFileSize()  { if (isFileOpen()) return curfilinfo.fsize; else return 0; }
  static uint32_t getIndex()     { if (isFileOpen()) return curfile.fptr; else return 0; }
  static bool isFileOpen()       { return isMounted() && curfile.obj.fs != 0; }
  static bool eof()              { return getIndex() >= getFileSize(); }

  // File data operations
  static int16_t get();
  static void setIndex(const uint32_t index)      { if (isFileOpen()) f_lseek(&curfile, index); }

  #if ENABLED(AUTO_REPORT_SD_STATUS)
    //
    // SD Auto Reporting
    //
    struct AutoReportSD { static void report() { report_status(); } };
    static AutoReporter<AutoReportSD> auto_reporter;
  #endif

/*
  static int16_t read(void *buf, uint16_t nbyte)  { return file.isOpen() ? file.read(buf, nbyte) : -1; }
  static int16_t write(void *buf, uint16_t nbyte) { return file.isOpen() ? file.write(buf, nbyte) : -1; }

private:
*/
  static FIL curfile;
  static FILINFO curfilinfo;
  static uint8_t  activefileitems[MAX_DIR_DEPTH];
  static dir_active_items_t  active_dir_items;

  //
  // Procedure calls to other files
  //
  #if HAS_MEDIA_SUBCALLS
    static uint8_t file_subcall_ctr;
    static uint32_t filespos[SD_PROCEDURE_DEPTH];
    static char proc_filenames[SD_PROCEDURE_DEPTH][MAXPATHNAMELENGTH];
  #endif

/*
  //
  // Directory items
  //
  static bool is_dir_or_gcode(const dir_t &p);
  static int countItems(SdFile dir);
  static void selectByIndex(SdFile dir, const uint8_t index);
  static void selectByName(SdFile dir, const char * const match);
*/

  static void printListing(
              const bool includeLongNames = true
            , const char * const prepend = nullptr
          );

};

#if PIN_EXISTS(SD_DETECT)
  #define IS_SD_INSERTED() (READ(SD_DETECT_PIN) == SD_DETECT_STATE)
#else
  // No card detect line? Assume the card is inserted.
  #define IS_SD_INSERTED() true
#endif

#define IS_SD_PRINTING()  (card.flag.sdprinting && !card.flag.abort_sd_printing)
#define IS_SD_FETCHING()  (!card.flag.sdprintdone && IS_SD_PRINTING())
#define IS_SD_PAUSED()    card.isPaused()
#define IS_SD_FILE_OPEN() card.isFileOpen()

extern CardReader card;

#else // !SDSUPPORT

#define IS_SD_PRINTING()  false
#define IS_SD_FETCHING()  false
#define IS_SD_PAUSED()    false
#define IS_SD_FILE_OPEN() false

#define LONG_FILENAME_LENGTH 0

#endif // !SDSUPPORT
