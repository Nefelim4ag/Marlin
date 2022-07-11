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

#include "../inc/MarlinConfig.h"

#if ENABLED(SDSUPPORT)

//#define DEBUG_CARDREADER

#include "cardreader.h"

#include "../MarlinCore.h"
#include "../lcd/marlinui.h"

#if ENABLED(DWIN_CREALITY_LCD)
#include "../lcd/e3v2/creality/dwin.h"
#elif ENABLED(DWIN_CREALITY_LCD_ENHANCED)
#include "../lcd/e3v2/enhanced/dwin.h"
#endif

#include "../module/planner.h" // for synchronize
#include "../module/printcounter.h"
#include "../gcode/queue.h"
#include "../module/settings.h"
#include "../module/stepper/indirection.h"
#include "../gcode/parser.h"

#if ENABLED(EMERGENCY_PARSER)
#include "../feature/e_parser.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../feature/powerloss.h"
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
#include "../feature/pause.h"
#endif

#define DEBUG_OUT EITHER(DEBUG_CARDREADER, MARLIN_DEV_MODE)
#include "../core/debug_out.h"
#include "../libs/hex_print.h"

// extern
extern GCodeParser parser;

PGMSTR(M21_STR, "M21");
PGMSTR(M23_STR, "M23 %s");
PGMSTR(M24_STR, "M24");

// public:

card_flags_t CardReader::flag;
// char                  CardReader::filename[FF_SFN_BUF], CardReader::longFilename[FF_LFN_BUF];
uint8_t CardReader::activefileitems[MAX_DIR_DEPTH];
dir_active_items_t CardReader::active_dir_items;
FIL CardReader::curfile;
FILINFO CardReader::curfilinfo;

IF_DISABLED(NO_SD_AUTOSTART, uint8_t CardReader::autofile_index); // = 0

#if ENABLED(BINARY_FILE_TRANSFER)
serial_index_t IF_DISABLED(HAS_MULTI_SERIAL, constexpr) CardReader::transfer_port_index;
#endif

// private:

#if HAS_MEDIA_SUBCALLS
uint8_t CardReader::file_subcall_ctr;
uint32_t CardReader::filespos[SD_PROCEDURE_DEPTH];
char CardReader::proc_filenames[SD_PROCEDURE_DEPTH][MAXPATHNAMELENGTH];
#endif

CardReader::CardReader()
{
    flag.sdprinting = flag.sdprintdone = flag.saving = flag.logging = false;
    FATFS_sd.fs_type = 0;
    curfile.obj.fs = 0;
    curfilinfo.fname[0] = 0;
    curfilinfo.altname[0] = 0;
    for (uint16_t i = 0; i < MAX_DIR_DEPTH; i++)
        active_dir_items.items[i] = 0;
    active_dir_items.current = 0;

    TERN_(HAS_MEDIA_SUBCALLS, file_subcall_ctr = 0);

    IF_DISABLED(NO_SD_AUTOSTART, autofile_cancel());

#if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)
    SET_INPUT_PULLUP(SD_DETECT_PIN);
#endif

#if PIN_EXISTS(SDPOWER)
    OUT_WRITE(SDPOWER_PIN, HIGH); // Power the SD reader
#endif
}

//
// Get a DOS 8.3 filename in its useful form
//
#ifndef FF_DEBUG
char *createFilename(char *const buffer, const dir_t &p)
{
    char *pos = buffer;
    LOOP_L_N(i, 11)
    {
        if (p.name[i] == ' ')
            continue;
        if (i == 8)
            *pos++ = '.';
        *pos++ = p.name[i];
    }
    *pos++ = 0;
    return buffer;
}

//
// Return 'true' if the item is a folder or G-code file
//
bool CardReader::is_dir_or_gcode(const dir_t &p)
{
    // uint8_t pn0 = p.name[0];

    if ((p.attributes & DIR_ATT_HIDDEN) // Hidden by attribute
                                        // When readDir() > 0 these must be false:
                                        //|| pn0 == DIR_NAME_FREE || pn0 == DIR_NAME_DELETED  // Clear or Deleted entry
                                        //|| pn0 == '.' || longFilename[0] == '.'             // Hidden file
                                        //|| !DIR_IS_FILE_OR_SUBDIR(&p)                       // Not a File or Directory
    )
        return false;

    flag.filenameIsDir = DIR_IS_SUBDIR(&p); // We know it's a File or Folder

    return (
        flag.filenameIsDir                                             // All Directories are ok
        || (p.name[8] == 'G' && p.name[9] != '~')                      // Non-backup *.G* files are accepted
        || (p.name[8] == 'C' && p.name[9] == 'F' && p.name[10] == 'G') // *.CFG* files are accepted
    );
}

//
// Get the number of (compliant) items in the folder
//
int CardReader::countItems(SdFile dir)
{
    dir_t p;
    int c = 0;
    while (dir.readDir(&p, longFilename) > 0)
        c += is_dir_or_gcode(p);

#if ALL(SDCARD_SORT_ALPHA, SDSORT_USES_RAM, SDSORT_CACHE_NAMES)
    nrFiles = c;
#endif

    return c;
}

//
// Get file/folder info for an item by index
//
void CardReader::selectByIndex(SdFile dir, const uint8_t index)
{
    dir_t p;
    for (uint8_t cnt = 0; dir.readDir(&p, longFilename) > 0;)
    {
        if (is_dir_or_gcode(p))
        {
            if (cnt == index)
            {
                createFilename(filename, p);
                return; // 0 based index
            }
            cnt++;
        }
    }
}

//
// Get file/folder info for an item by name
//
void CardReader::selectByName(SdFile dir, const char *const match)
{
    dir_t p;
    for (uint8_t cnt = 0; dir.readDir(&p, longFilename) > 0; cnt++)
    {
        if (is_dir_or_gcode(p))
        {
            createFilename(filename, p);
            if (strcasecmp(match, filename) == 0)
                return;
        }
    }
}

#endif // FF_DEBUG

/**
 * Open a G-code file and set Marlin to start processing it.
 * Enqueues M23 and M24 commands to initiate a media print.
 */
void CardReader::openAndPrintFile(const char *name)
{
    char cmd[4 + strlen(name) + 1 + 3 + 1]; // Room for "M23 ", filename, "\n", "M24", and null
    sprintf_P(cmd, M23_STR, name);
    for (char *c = &cmd[4]; *c; c++)
        *c = tolower(*c);
    strcat_P(cmd, PSTR("\nM24"));
    queue.inject(cmd);
}

#ifndef FF_DEBUG

//
// Get the root-relative DOS path of the selected file
//
void CardReader::getAbsFilenameInCWD(char *dst)
{
    *dst++ = '/';
    uint8_t cnt = 1;

    auto appendAtom = [&](SdFile &file)
    {
        file.getDosName(dst);
        while (*dst && cnt < MAXPATHNAMELENGTH)
        {
            dst++;
            cnt++;
        }
        if (cnt < MAXPATHNAMELENGTH)
        {
            *dst = '/';
            dst++;
            cnt++;
        }
    };

    LOOP_L_N(i, workDirDepth) // Loop down to current work dir
    appendAtom(workDirParents[i]);

    if (cnt < MAXPATHNAMELENGTH - (FILENAME_LENGTH)-1)
    { // Leave room for filename and nul
        appendAtom(file);
        --dst;
    }
    *dst = '\0';
}

#endif // FF_DEBUG

void openFailed(const char *const fname)
{
    SERIAL_ECHOLNPGM(STR_SD_OPEN_FILE_FAIL, fname, ".");
}

void announceOpen(const uint8_t doing, const char *const path)
{
    if (doing)
    {
        PORT_REDIRECT(SerialMask::All);
        SERIAL_ECHO_START();
        SERIAL_ECHOPGM("Now ");
        SERIAL_ECHOF(doing == 1 ? F("doing") : F("fresh"));
        SERIAL_ECHOLNPGM(" file: ", path);
    }
}

//
// Open a file by DOS path for read
// The 'subcall_type' flag indicates...
//   - 0 : Standard open from host or user interface.
//   - 1 : (file open) Opening a new sub-procedure.
//   - 1 : (no file open) Opening a macro (M98).
//   - 2 : Resuming from a sub-procedure
//   - 9 : Just open file
//
void CardReader::openFileRead(const char *const path, const uint8_t subcall_type /*=0*/)
{
    if (!isMounted() || path == 0)
        return;

    switch (subcall_type)
    {
    case 0: // Starting a new print. "Now fresh file: ..."
        announceOpen(2, path);
        TERN_(HAS_MEDIA_SUBCALLS, file_subcall_ctr = 0);
        break;

#if HAS_MEDIA_SUBCALLS

    case 1: // Starting a sub-procedure
    {

        // With no file is open it's a simple macro. "Now doing file: ..."
        if (!isFileOpen())
        {
            announceOpen(1, path);
            break;
        }

        // Too deep? The firmware has to bail.
        if (file_subcall_ctr > SD_PROCEDURE_DEPTH - 1)
        {
            SERIAL_ERROR_MSG("Exceeded max SUBROUTINE depth:", SD_PROCEDURE_DEPTH);
            kill(GET_TEXT_F(MSG_KILL_SUBCALL_OVERFLOW));
            return;
        }

        // Store current filename (based on workDirParents) and position
        f_getcwd(proc_filenames[file_subcall_ctr], MAXPATHNAMELENGTH);
        uint16_t slen = strlen(proc_filenames[file_subcall_ctr]);
        proc_filenames[file_subcall_ctr][slen] = '/';
        slen++;
        proc_filenames[file_subcall_ctr][slen] = 0;
        strncat(proc_filenames[file_subcall_ctr], curfilinfo.fname, (MAXPATHNAMELENGTH - slen));
        filespos[file_subcall_ctr] = getIndex();

        // For sub-procedures say 'SUBROUTINE CALL target: "..." parent: "..." pos12345'
        SERIAL_ECHO_MSG("SUBROUTINE CALL target:\"", path, "\" parent:\"", proc_filenames[file_subcall_ctr], "\" pos", getIndex());
        file_subcall_ctr++;
    }
    break;

    case 2: // Resuming previous file after sub-procedure
        SERIAL_ECHO_MSG("END SUBROUTINE");
        break;

#endif
    }

    abortFilePrintNow();

    if (f_stat(path, &curfilinfo) == FR_OK && f_open(&curfile, path, FA_READ) == FR_OK)
    {
        { // Don't remove this block, as the PORT_REDIRECT is a RAII
            PORT_REDIRECT(SerialMask::All);
            SERIAL_ECHOLNPGM(STR_SD_FILE_OPENED, curfilinfo.fname, STR_SD_SIZE, curfilinfo.fsize);
            SERIAL_ECHOLNPGM(STR_SD_FILE_SELECTED);
        }

        if (subcall_type < 9)
        {
            //      selectFileByName(fname);
            ui.set_status(curfilinfo.fname);
        }
    }
    else
    {
        openFailed(curfilinfo.fname);
        memset(&curfilinfo, 0, sizeof(curfilinfo));
    }
}

#ifndef FF_DEBUG
inline void echo_write_to_file(const char *const fname)
{
    SERIAL_ECHOLNPGM(STR_SD_WRITE_TO_FILE, fname);
}

//
// Open a file by DOS path for write
//
void CardReader::openFileWrite(const char *const path)
{
    if (!isMounted())
        return;

    announceOpen(2, path);
    TERN_(HAS_MEDIA_SUBCALLS, file_subcall_ctr = 0);

    abortFilePrintNow();

    SdFile *diveDir;
    const char *const fname = diveToFile(false, diveDir, path);
    if (!fname)
        return;

#if ENABLED(SDCARD_READONLY)
    openFailed(fname);
#else
    if (file.open(diveDir, fname, O_CREAT | O_APPEND | O_WRITE | O_TRUNC))
    {
        flag.saving = true;
        selectFileByName(fname);
        TERN_(EMERGENCY_PARSER, emergency_parser.disable());
        echo_write_to_file(fname);
        ui.set_status(fname);
    }
    else
        openFailed(fname);
#endif
}

//
// Check if a file exists by absolute or workDir-relative path
// If the file exists, the long name can also be fetched.
//
bool CardReader::fileExists(const char *const path)
{
    if (!isMounted())
        return false;

    DEBUG_ECHOLNPGM("fileExists: ", path);

    // Dive to the file's directory and get the base name
    SdFile *diveDir = nullptr;
    const char *const fname = diveToFile(false, diveDir, path);
    if (!fname)
        return false;

    // Get the longname of the checked file
    // diveDir->rewind();
    // selectByName(*diveDir, fname);
    // diveDir->close();

    // Try to open the file and return the result
    SdFile tmpFile;
    const bool success = tmpFile.open(diveDir, fname, O_READ);
    if (success)
        tmpFile.close();
    return success;
}

#endif // FF_DEBUG

#if ENABLED(LONG_FILENAME_HOST_SUPPORT)

//
// Get a long pretty path based on a DOS 8.3 path
//
void CardReader::printLongPath(char *const path)
{

    serial_index_t port = queue.ring_buffer.command_port();

    char full_path[256];
    int i, pathLen = strlen(path);
    char *fname_ptr = path;
    FILINFO finfo;

    // Find file name and go to directory
    for (i = pathLen - 1; i >= 0; i--)
    {
        if (path[i] == '/')
            break;
    }
    if (path[i] == '/')
    {
        path[i] = 0;
        i++;
        if (f_chdir(path) != FR_OK)
        {
            SERIAL_EOL();
            SERIAL_ECHO_START();
            SERIAL_ECHOPGM(STR_SD_CANT_OPEN_SUBDIR, path);
            return;
        }
    }
    fname_ptr = path + i;

    // Get full current directory name
    f_getcwd(full_path, 256);
    pathLen = strlen(full_path);
    if (pathLen < 255)
    {
        strcat(full_path, "/");
        pathLen++;
    }

    // Get full file name
    if (f_stat(fname_ptr, &finfo) != FR_OK)
    {
        SERIAL_EOL();
        SERIAL_ECHO_START();
        SERIAL_ECHOPGM(STR_SD_OPEN_FILE_FAIL, fname_ptr);
        return;
    }

    // Print full directory and file name to serial output
    if (port.index == MKS_WIFI_SERIAL_NUM)
    {
        mks_wifi_out_add((uint8_t *)path, strlen(path));
        mks_wifi_out_add((uint8_t *)fname_ptr, strlen(fname_ptr));
    }
    else
    {
        // Print /LongNamePart to serial output
        SERIAL_ECHO(path);
        SERIAL_ECHO(fname_ptr);
        SERIAL_EOL();
    }
}

#endif // LONG_FILENAME_HOST_SUPPORT

//
// Delete a file by name in the working directory
//
void CardReader::removeFile(const char *const name)
{
    if (!isMounted())
        return;

        // abortFilePrintNow();

#if ENABLED(SDCARD_READONLY)
    SERIAL_ECHOLNPGM("Deletion failed (read-only), File: ", fname, ".");
#else
    if (f_unlink(name) == FR_OK)
    {
        SERIAL_ECHOLNPGM("File deleted:", name);
    }
    else
        SERIAL_ECHOLNPGM("Deletion failed, File: ", name, ".");
#endif
}

void CardReader::report_status()
{
    if (isPrinting())
    {
        SERIAL_ECHOPGM(STR_SD_PRINTING_BYTE, curfile.fptr);
        SERIAL_CHAR('/');
        SERIAL_ECHOLN(curfilinfo.fsize);
    }
    else
        SERIAL_ECHOLNPGM(STR_SD_NOT_PRINTING);
}

//
// List all files on the SD card
//
void CardReader::ls(bool includeLongNames /*=true*/)
{
    if (isMounted())
    {
#if ENABLED(MKS_WIFI)
        serial_index_t port = queue.ring_buffer.command_port();
        if (port.index == MKS_WIFI_SERIAL_NUM)
            printListing(includeLongNames);
        else
            printListing(includeLongNames);
#else
        printListing(root OPTARG(LONG_FILENAME_HOST_SUPPORT, includeLongNames));
#endif
    }
}

/**
 * Recursive method to print all files within a folder in flat
 * DOS 8.3 format. This style of listing is the most compatible
 * with legacy hosts.
 *
 * This method recurses to unlimited depth and lists all G-code
 * files within the given parent. If the hierarchy is very deep
 * this can blow up the stack, so a 'depth' parameter would be a
 * good addition.
 */
void CardReader::printListing(const bool includeLongNames /*=true*/
                              ,
                              const char *const prepend /*=nullptr*/
)
{

    TCHAR curpath[256];
    DIR dir;
    FILINFO finfo;
    static uint8_t depth = 0;

    // if is root then prepare dir
    if (prepend == NULL)
    {
        curpath[0] = '/';
        curpath[1] = 0;
    }
    else
    {
        strncpy(curpath, prepend, 256);
    }
    if (f_chdir(curpath) != FR_OK)
        return;

    serial_index_t port = queue.ring_buffer.command_port();

    if (port.index == MKS_WIFI_SERIAL_NUM)
    {
        // search for path
        if (prepend == NULL && parser.string_arg != 0)
        {
            mks_wifi_out_add((uint8_t *)"STRING_ARG: ", strlen("STRING_ARG: "));
            mks_wifi_out_add((uint8_t *)parser.string_arg, strlen(parser.string_arg));
            mks_wifi_out_add((uint8_t *)"\n", 1);
            uint32_t ind = strlen(parser.string_arg);
            if (ind > 0)
            {
                ind--;
                uint32_t slen = ind;
                while (ind > 0 && parser.string_arg[ind] != '/')
                    ind--;
                if (slen != ind && parser.string_arg[ind] == '/')
                {
                    strncpy(curpath, parser.string_arg + ind + 1, sizeof(curpath) - 1);
                    mks_wifi_out_add((uint8_t *)"PATHN: ", strlen("PATHN: "));
                    mks_wifi_out_add((uint8_t *)curpath, strlen(curpath));
                    mks_wifi_out_add((uint8_t *)"\n", 1);
                }
            }
        }

        if (f_opendir(&dir, curpath) != FR_OK)
        {
            mks_wifi_out_add((uint8_t *)"!!! Error opening ", strlen("!!! Error opening "));
            mks_wifi_out_add((uint8_t *)curpath, strlen(curpath));
            mks_wifi_out_add((uint8_t *)"!\n", 1);
            return;
        }

        DEBUG("Begin file list in path: %s", curpath);
        while (f_readdir(&dir, &finfo) == FR_OK)
        {
            if (finfo.fname[0] == 0)
                break;
            if (isFileMustShow(&finfo) || isDirMustShow(&finfo))
            {
                DEBUG("FILE: %s", finfo.fname);
                mks_wifi_out_add((uint8_t *)finfo.fname, strlen(finfo.fname));
                // printLongPath(createFilename(finfo.fname, p));
                if (isDirMustShow(&finfo))
                {
                    DEBUG("FILE: %s is DIR", finfo.fname);
                    mks_wifi_out_add((uint8_t *)".DIR", 4);
                }
                mks_wifi_out_add((uint8_t *)"\n", 1);
            }
        }
    }
    else // Not WiFi
    {
        while (f_readdir(&dir, &finfo) == FR_OK)
        {
            if (finfo.fname[0] == 0)
                break;
            if (isDirMustShow(&finfo))
            {
                depth++;
                printListing(includeLongNames, finfo.fname);
            }
            else if (isFileMustShow(&finfo))
            {
                SERIAL_ECHO(curpath);
                SERIAL_CHAR('/');
                SERIAL_ECHO(finfo.fname);
                SERIAL_CHAR(' ');
                SERIAL_ECHOLN(finfo.fsize);
            }
        }
    }
    if (depth > 0)
    {
        depth--;
        f_chdir("..");
    }
}

void CardReader::write_command(char *const buf)
{
    if (!isFileOpen())
        SERIAL_ERROR_MSG(STR_SD_ERR_WRITE_TO_FILE);

    char *begin = buf,
         *npos = nullptr,
         *end = buf + strlen(buf) - 1;

    FRESULT fres = FR_OK;
    UINT writed = 0;
    uint32_t towrite;

    if ((npos = strchr(buf, 'N')))
    {
        begin = strchr(npos, ' ') + 1;
        end = strchr(npos, '*') - 1;
    }
    end[1] = '\r';
    end[2] = '\n';
    end[3] = '\0';

    towrite = strlen(begin);
    fres = f_write(&curfile, begin, towrite, &writed);

    if (fres != FR_OK || writed != towrite)
        SERIAL_ERROR_MSG(STR_SD_ERR_WRITE_TO_FILE);
}

#if DISABLED(NO_SD_AUTOSTART)
/**
 * Run all the auto#.g files. Called:
 * - On boot after successful card init.
 * - From the LCD command to Run Auto Files
 */
void CardReader::autofile_begin()
{
    autofile_index = 1;
    (void)autofile_check();
}

/**
 * Run the next auto#.g file. Called:
 *   - On boot after successful card init
 *   - After finishing the previous auto#.g file
 *   - From the LCD command to begin the auto#.g files
 *
 * Return 'true' if an auto file was started
 */
bool CardReader::autofile_check()
{
    if (!autofile_index)
        return false;

    if (!isMounted())
        mount();
    else if (ENABLED(SDCARD_EEPROM_EMULATION))
        settings.first_load();

#ifndef FF_DEBUG
    // Don't run auto#.g when a PLR file exists
    if (isMounted() && TERN1(POWER_LOSS_RECOVERY, !recovery.valid()))
    {
        char autoname[10];
        sprintf_P(autoname, PSTR("/auto%c.g"), '0' + autofile_index - 1);
        if (fileExists(autoname))
        {
            cdroot();
            openAndPrintFile(autoname);
            autofile_index++;
            return true;
        }
    }
    autofile_cancel();
#endif // FF_DEBUG
    return false;
}
#endif

void CardReader::closefile(const bool store_location /*=false*/)
{
    f_close(&curfile);
    curfile.obj.fs = 0;

    flag.saving = flag.logging = false;
    TERN_(EMERGENCY_PARSER, emergency_parser.enable());

    if (store_location)
    {
        // future: store printer state, filename and position for continuing a stopped print
        //  so one can unplug the printer and continue printing the next day.
    }
}

#ifndef FF_DEBUG
//
// Get info for a file in the working directory by DOS name
//
void CardReader::selectFileByName(const char *const match)
{
#if ENABLED(SDSORT_CACHE_NAMES)
    for (uint16_t nr = 0; nr < sort_count; nr++)
        if (strcasecmp(match, sortshort[nr]) == 0)
        {
            strcpy(filename, sortshort[nr]);
            strcpy(longFilename, sortnames[nr]);
            flag.filenameIsDir = IS_DIR(nr);
            return;
        }
#endif
    workDir.rewind();
    selectByName(workDir, match);
}

uint16_t CardReader::countFilesInWorkDir()
{
    workDir.rewind();
    return countItems(workDir);
}

/**
 * Dive to the given DOS 8.3 file path, with optional echo of the dive paths.
 *
 * On entry:
 *  - The workDir points to the last-set navigation target by cd, cdup, cdroot, or diveToFile(true, ...)
 *
 * On exit:
 *  - Your curDir pointer contains an SdFile reference to the file's directory.
 *  - If update_cwd was 'true' the workDir now points to the file's directory.
 *
 * Returns a pointer to the last segment (filename) of the given DOS 8.3 path.
 * On exit, inDirPtr contains an SdFile reference to the file's directory.
 *
 * A nullptr result indicates an unrecoverable error.
 *
 * NOTE: End the path with a slash to dive to a folder. In this case the
 *       returned filename will be blank (points to the end of the path).
 */
const char *CardReader::diveToFile(const bool update_cwd, SdFile *&inDirPtr, const char *const path, const bool echo /*=false*/)
{
    DEBUG_SECTION(est, "diveToFile", true);

    // Track both parent and subfolder
    static SdFile newDir1, newDir2;
    SdFile *sub = &newDir1, *startDirPtr;

    // Parsing the path string
    const char *atom_ptr = path;

    DEBUG_ECHOLNPGM(" path = '", path, "'");

    if (path[0] == '/')
    { // Starting at the root directory?
        inDirPtr = &root;
        atom_ptr++;
        DEBUG_ECHOLNPGM(" CWD to root: ", hex_address((void *)inDirPtr));
        if (update_cwd)
            workDirDepth = 0; // The cwd can be updated for the benefit of sub-programs
    }
    else
        inDirPtr = &workDir; // Dive from workDir (as set by the UI)

    startDirPtr = inDirPtr;

    DEBUG_ECHOLNPGM(" startDirPtr = ", hex_address((void *)startDirPtr));

    while (atom_ptr)
    {
        // Find next subdirectory delimiter
        const char *const name_end = strchr(atom_ptr, '/');

        // Last atom in the path? Item found.
        if (name_end <= atom_ptr)
            break;

        // Isolate the next subitem name
        const uint8_t len = name_end - atom_ptr;
        char dosSubdirname[len + 1];
        strncpy(dosSubdirname, atom_ptr, len);
        dosSubdirname[len] = 0;

        if (echo)
            SERIAL_ECHOLN(dosSubdirname);

        DEBUG_ECHOLNPGM(" sub = ", hex_address((void *)sub));

        // Open inDirPtr (closing first)
        sub->close();
        if (!sub->open(inDirPtr, dosSubdirname, O_READ))
        {
            openFailed(dosSubdirname);
            atom_ptr = nullptr;
            break;
        }

        // Close inDirPtr if not at starting-point
        if (inDirPtr != startDirPtr)
        {
            DEBUG_ECHOLNPGM(" closing inDirPtr: ", hex_address((void *)inDirPtr));
            inDirPtr->close();
        }

        // inDirPtr now subDir
        inDirPtr = sub;
        DEBUG_ECHOLNPGM(" inDirPtr = sub: ", hex_address((void *)inDirPtr));

        // Update workDirParents and workDirDepth
        if (update_cwd)
        {
            DEBUG_ECHOLNPGM(" update_cwd");
            if (workDirDepth < MAX_DIR_DEPTH)
                workDirParents[workDirDepth++] = *inDirPtr;
        }

        // Point sub at the other scratch object
        sub = (inDirPtr != &newDir1) ? &newDir1 : &newDir2;
        DEBUG_ECHOLNPGM(" swapping sub = ", hex_address((void *)sub));

        // Next path atom address
        atom_ptr = name_end + 1;
    }

    if (update_cwd)
    {
        workDir = *inDirPtr;
        DEBUG_ECHOLNPGM(" final workDir = ", hex_address((void *)inDirPtr));
        flag.workDirIsRoot = (workDirDepth == 0);
        TERN_(SDCARD_SORT_ALPHA, presort());
    }

    DEBUG_ECHOLNPGM(" returning string ", atom_ptr ?: "nullptr");
    return atom_ptr;
}

void CardReader::cdroot()
{
    workDir = root;
    flag.workDirIsRoot = true;
    workDirDepth = 0;
}

#endif // FF_DEBUG

int8_t CardReader::cdup()
{
    if (isRootDir())
        return 0;

    if (f_chdir("..") == FR_OK)
    {
        TERN_(SDCARD_SORT_ALPHA, presort());
        return 1;
    }
    else
        SERIAL_ECHO_MSG(STR_SD_CANT_ENTER_SUBDIR, "..");
    return 0;
}

void CardReader::cd(const char *relpath)
{
    if (f_chdir(relpath) == FR_OK)
    {
        TERN_(SDCARD_SORT_ALPHA, presort());
    }
    else
        SERIAL_ECHO_MSG(STR_SD_CANT_ENTER_SUBDIR, relpath);
}

uint16_t CardReader::get_num_Files()
{
    if (!isMounted())
        return 0;
    uint16_t item_count = 0;
    TCHAR curpath[256];
    DIR dir;
    FILINFO finfo;

    if (f_getcwd(curpath, sizeof(curpath)) != FR_OK)
        return 0;
    if (f_opendir(&dir, curpath) != FR_OK)
        return 0;
    while (1)
    {
        if (f_readdir(&dir, &finfo) != FR_OK)
            break;
        if (finfo.fname[0] == 0)
            break;
        if ((finfo.fattrib & AM_DIR && !(finfo.fattrib & AM_HID)) || isFileMustShow(&finfo))
            item_count++;
    }

    f_closedir(&dir);
    return item_count;
}

bool CardReader::isRootDir()
{
    if (!isMounted())
        return true;
    TCHAR curpath[256];

    if (f_getcwd(curpath, sizeof(curpath)) != FR_OK)
        return true;

    if (curpath[0] == '/')
    {
        if (curpath[1] == 0)
            return true;
    }

    return false;
}

//
// Get info for a file in the working directory by index
//
void CardReader::selectFileByIndex(const uint16_t nr)
{
    uint16_t item_count = 0;
    TCHAR curpath[256];
    DIR dir;
    FILINFO finfo;

    if (f_getcwd(curpath, sizeof(curpath)) != FR_OK)
        return;
    if (f_opendir(&dir, curpath) != FR_OK)
        return;

    // Files first ...
    do
    {
        if (f_readdir(&dir, &finfo) != FR_OK)
            break;
        if (finfo.fname[0] == 0)
            break;
        if (isFileMustShow(&finfo))
            item_count++;
    } while (item_count < nr + 1);

    if (item_count < nr + 1)
    {
        // ... then directories
        f_readdir(&dir, NULL); // rewind firectory
        do
        {
            if (f_readdir(&dir, &finfo) != FR_OK)
                break;
            if (finfo.fname[0] == 0)
                break;
            if (isDirMustShow(&finfo))
                item_count++;
        } while (item_count < nr + 1);
    }

    if (item_count == nr + 1)
    {
        memcpy(&curfilinfo, &finfo, sizeof(finfo));
        card.flag.filenameIsDir = (finfo.fattrib & AM_DIR);
    }
    f_closedir(&dir);
}

bool CardReader::isFileMustShow(FILINFO *finfo)
{
    char *fext = FATFS_GetFileExtensionUTF(finfo->fname);
    if ((strcmp(fext, "gcode") == 0) && !(finfo->fattrib & AM_HID))
        return true;

    return false;
}

bool CardReader::isDirMustShow(FILINFO *finfo)
{
    return ((finfo->fattrib) & AM_DIR && !(finfo->fattrib & AM_HID));
}

int16_t CardReader::get()
{
    int16_t val = 0;
    UINT readed = 0;

    if (!isFileOpen())
        return -1;

    if (f_read(&curfile, &val, 1, &readed) != FR_OK)
        return -1;

    return val;
}

//
// Return from procedure or close out the Print Job
//
void CardReader::fileHasFinished()
{
    f_close(&curfile);
    curfile.obj.fs = 0;

#ifndef FF_DEBUG
#if HAS_MEDIA_SUBCALLS
    if (file_subcall_ctr > 0)
    { // Resume calling file after closing procedure
        file_subcall_ctr--;
        openFileRead(proc_filenames[file_subcall_ctr], 2); // 2 = Returning from sub-procedure
        setIndex(filespos[file_subcall_ctr]);
        startOrResumeFilePrinting();
        return;
    }
#endif
#endif
    endFilePrintNow(TERN_(SD_RESORT, true));

    flag.sdprintdone = true;       // Stop getting bytes from the SD card
    marlin_state = MF_SD_COMPLETE; // Tell Marlin to enqueue M1001 soon
}

#ifndef FF_DEBUG
#if ENABLED(AUTO_REPORT_SD_STATUS)
AutoReporter<CardReader::AutoReportSD> CardReader::auto_reporter;
#endif

#if ENABLED(POWER_LOSS_RECOVERY)

bool CardReader::jobRecoverFileExists()
{
    const bool exists = recovery.file.open(&root, recovery.filename, O_READ);
    if (exists)
        recovery.file.close();
    return exists;
}

void CardReader::openJobRecoveryFile(const bool read)
{
    if (!isMounted())
        return;
    if (recovery.file.isOpen())
        return;
    if (!recovery.file.open(&root, recovery.filename, read ? O_READ : O_CREAT | O_WRITE | O_TRUNC | O_SYNC))
        openFailed(recovery.filename);
    else if (!read)
        echo_write_to_file(recovery.filename);
}

// Removing the job recovery file currently requires closing
// the file being printed, so during SD printing the file should
// be zeroed and written instead of deleted.
void CardReader::removeJobRecoveryFile()
{
    if (jobRecoverFileExists())
    {
        recovery.init();
        removeFile(recovery.filename);
#if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
        SERIAL_ECHOPGM("Power-loss file delete");
        SERIAL_ECHOF(jobRecoverFileExists() ? F(" failed.\n") : F("d.\n"));
#endif
    }
}

#endif // POWER_LOSS_RECOVERY

void CardReader::openLogFile(const char *const path)
{
    flag.logging = DISABLED(SDCARD_READONLY);
    IF_DISABLED(SDCARD_READONLY, openFileWrite(path));
}

#endif // FF_DEBUG

void CardReader::mount(bool wifi)
{

    if (f_mount(&FATFS_sd, DISK_SD, 1) != FR_OK)
    {
        if (wifi)
            SERIAL_ECHO_MSG("WIFI: " STR_SD_INIT_FAIL);
        else
            SERIAL_ECHO_MSG(STR_SD_INIT_FAIL);
    }
    else
    {
        SERIAL_ECHO_MSG(STR_SD_CARD_OK);
    }

#if PIN_EXISTS(SD_DETECT)
    if (!isMounted() && marlin_state != MF_INITIALIZING)
        ui.set_status(GET_TEXT_F(MSG_MEDIA_INIT_FAIL), -1);
#endif

    ui.refresh();
}

/**
 * Handle SD card events
 */
#if MB(FYSETC_CHEETAH, FYSETC_AIO_II)
#include "../module/stepper.h"
#endif

void CardReader::manage_media()
{
    static uint8_t prev_stat = 2; // First call, no prior state
    uint8_t stat = uint8_t(IS_SD_INSERTED());
    if (stat == prev_stat)
        return;

    DEBUG_ECHOLNPGM("SD: Status changed from ", prev_stat, " to ", stat);

    if (ui.detected())
    {

        uint8_t old_stat = prev_stat;
        prev_stat = stat; // Change now to prevent re-entry

        if (stat)
        {                    // Media Inserted
            safe_delay(500); // Some boards need a delay to get settled
            if (TERN1(SD_IGNORE_AT_STARTUP, old_stat != 2))
                mount(); // Try to mount the media
            if (!isMounted())
                stat = 0; // Not mounted?
        }
        else
        {
#if PIN_EXISTS(SD_DETECT)
            release(); // Card is released
#endif
        }

        ui.media_changed(old_stat, stat); // Update the UI

        if (stat)
        {
            TERN_(SDCARD_EEPROM_EMULATION, settings.first_load());
            if (old_stat == 2)
            { // First mount?
                DEBUG_ECHOLNPGM("First mount.");
#if ENABLED(POWER_LOSS_RECOVERY)
                recovery.check(); // Check for PLR file. (If not there then call autofile_begin)
#elif DISABLED(NO_SD_AUTOSTART)
#ifndef FF_DEBUG
                autofile_begin(); // Look for auto0.g on the next loop
#endif
#endif
            }
        }
    }
    else
        DEBUG_ECHOLNPGM("SD: No UI Detected.");
}

/**
 * "Release" the media by clearing the 'mounted' flag.
 * Used by M22, "Release Media", manage_media.
 */
void CardReader::release()
{
    // Card removed while printing? Abort!
    if (IS_SD_PRINTING())
        abortFilePrintSoon();
    else
        endFilePrintNow();

    f_unmount(DISK_SD);
}

void CardReader::abortFilePrintNow()
{
    flag.sdprinting = flag.sdprintdone = false;
    endFilePrintNow(TERN_(SD_RESORT, re_sort));
}

//
// Run tasks upon finishing or aborting a file print.
//
void CardReader::endFilePrintNow()
{
    TERN_(ADVANCED_PAUSE_FEATURE, did_pause_print = 0);
    flag.abort_sd_printing = false;
    if (isFileOpen())
        f_close(&curfile);
}

//
// Echo the DOS 8.3 filename (and long filename, if any)
//
void CardReader::printSelectedFilename()
{
    if (isFileOpen() == true)
    {
        SERIAL_ECHO(curfilinfo.altname);
#if ENABLED(LONG_FILENAME_HOST_SUPPORT)
        if (curfilinfo.fname[0])
        {
            SERIAL_CHAR(' ');
            SERIAL_ECHO(curfilinfo.fname);
        }
#endif
    }
    else
        SERIAL_ECHOPGM("(no file)");

    SERIAL_EOL();
}

/**
 * Start or resume a media print by setting the sdprinting flag.
 * The file browser pre-sort is also purged to free up memory,
 * since you cannot browse files during active printing.
 * Used by M24 and anywhere Start / Resume applies.
 */
void CardReader::startOrResumeFilePrinting()
{
    if (isMounted())
    {
        flag.sdprinting = true;
        flag.sdprintdone = false;
        TERN_(SD_RESORT, flush_presort());
    }
}

#endif // SDSUPPORT
