//vls-begin// script files
#include "stdafx.h"
#include "tintin.h"

char DLLEXPORT szPROFILESCRIPT[MAX_PATH];

void use_command(char *arg)
{
    char file[BUFFER_SIZE];
    char msg[BUFFER_SIZE];
//* en
//    arg = get_arg_in_braces(arg, file, STOP_SPACES);
    arg = get_arg_in_braces(arg, file, WITH_SPACES);
//* /en
    if ( !*file ) {
        tintin_puts2(rs::rs(1233));
        EnterCriticalSection(&secScriptFiles);
        SCRIPTFILE_INDEX ind = ScriptFileList.begin();
        while (ind != ScriptFileList.end()) {
            CScriptFile *pScr = *ind;
            sprintf(msg, rs::rs(1234), pScr->m_strName.data());
            tintin_puts2(msg);
            ind++;
        }
        LeaveCriticalSection(&secScriptFiles);
        return;
    }

    switch (FindScriptFile(file)) {
    case -2: case -3:
        strcpy(msg, rs::rs(1231));
        break;
    case -1:
        AddScriptFile(file);
        if (WaitForSingleObject(eventReadingConfig, 0) == WAIT_OBJECT_0)
            SetEvent(eventReadingHasUse);
        else
            PostMessage(hwndMAIN, WM_USER+300, 0, 0);
        sprintf(msg, rs::rs(1235), file);
        break;
    default:
        sprintf(msg, rs::rs(1232), file);
    }
    if (mesvar[MSG_SF])
        tintin_puts2(msg);
}

void unuse_command(char *arg)
{
    char file[BUFFER_SIZE];
    char msg[BUFFER_SIZE];
//* en
//    arg = get_arg_in_braces(arg, file, STOP_SPACES);
    arg = get_arg_in_braces(arg, file, WITH_SPACES);
//* /en
    if ( !*file ) {
        use_command("\0");
        return;
    }

    if (FindScriptFile(file) < 0) {
        if (mesvar[MSG_SF])
            tintin_puts2(rs::rs(1236));
        return;
    }

    RemoveScriptFile(file);
    if (mesvar[MSG_SF]) {
        sprintf(msg, rs::rs(1237), file);
        tintin_puts2(msg);
    }
}

PCScriptFile DLLEXPORT GetScriptFileList(int* size)
{
    EnterCriticalSection(&secScriptFiles);
    if (size)
        *size = ScriptFileList.size();
    CScriptFile *pScr = ScriptFileList.front();
    LeaveCriticalSection(&secScriptFiles);
    return pScr;
}

PCScriptFile DLLEXPORT GetScriptFile(int pos)
{
    EnterCriticalSection(&secScriptFiles);
    SCRIPTFILE_INDEX ind = ScriptFileList.begin();
    while (ind != ScriptFileList.end()) {
        CScriptFile *pScr = *ind;
        if (pos == 0) {
            LeaveCriticalSection(&secScriptFiles);
            return pScr;
        }
        ind++;
        pos--;
    }
    LeaveCriticalSection(&secScriptFiles);
    return NULL;
}

int DLLEXPORT FindScriptFile(char *filename)
{
    if (!filename) return -1;

    if (!_stricmp("commonlib.scr", filename)) return -2;
    if (!_stricmp(szPROFILESCRIPT, filename)) return -3;

    EnterCriticalSection(&secScriptFiles);
    int pos = 0;
    SCRIPTFILE_INDEX ind = ScriptFileList.begin();
    while (ind != ScriptFileList.end()) {
        CScriptFile *pScr = *ind;
        if (!_stricmp(pScr->m_strName.data(), filename)) {
            LeaveCriticalSection(&secScriptFiles);
            return pos;
        }
        pos++;
        ind++;
    }
    LeaveCriticalSection(&secScriptFiles);
    return -1;
}

PCScriptFile AddScriptFile(char* filename)
{
    if (!filename) return NULL;

    CScriptFile *pScr;

    if (!_stricmp("commonlib.scr", filename)) return NULL;
    if (!_stricmp(szPROFILESCRIPT, filename)) return NULL;

    EnterCriticalSection(&secScriptFiles);
    SCRIPTFILE_INDEX ind = ScriptFileList.begin();
    while (ind != ScriptFileList.end()) {
        pScr = *ind;
        if (!_stricmp(pScr->m_strName.data(), filename)) {
            LeaveCriticalSection(&secScriptFiles);
            return pScr;
        }
        ind++;
    }

    pScr = new CScriptFile;
    pScr->m_strName = filename;
    ScriptFileList.push_back(pScr);
    LeaveCriticalSection(&secScriptFiles);
    return pScr;
}

void UpScriptFile(char* filename)
{
    if (!filename) return;

    EnterCriticalSection(&secScriptFiles);
    SCRIPTFILE_INDEX ind = ScriptFileList.begin();
    while (ind != ScriptFileList.end()) {
        CScriptFile *pScr = *ind;
        if (!_stricmp(pScr->m_strName.data(), filename)) {
            ind = ScriptFileList.erase(ind);
            if (ind != ScriptFileList.begin())
                ind--;
            ScriptFileList.insert(ind, pScr);
            LeaveCriticalSection(&secScriptFiles);
            return;
        }
        ind++;
    }
    LeaveCriticalSection(&secScriptFiles);
}

void DownScriptFile(char* filename)
{
    if (!filename) return;

    EnterCriticalSection(&secScriptFiles);
    SCRIPTFILE_INDEX ind = ScriptFileList.begin();
    while (ind != ScriptFileList.end()) {
        CScriptFile *pScr = *ind;
        if (!_stricmp(pScr->m_strName.data(), filename)) {
            ind = ScriptFileList.erase(ind);
            if (ind != ScriptFileList.end())
                ind++;
            ScriptFileList.insert(ind, pScr);
            LeaveCriticalSection(&secScriptFiles);
            return;
        }
        ind++;
    }
    LeaveCriticalSection(&secScriptFiles);
}

void RemoveScriptFile(char* filename)
{
    if (!filename) return;

    EnterCriticalSection(&secScriptFiles);
    SCRIPTFILE_INDEX ind = ScriptFileList.begin();
    while (ind != ScriptFileList.end()) {
        CScriptFile *pScr = *ind;
        if (!_stricmp(pScr->m_strName.data(), filename)) {
            delete pScr;
            ScriptFileList.erase(ind);
            LeaveCriticalSection(&secScriptFiles);
            return;
        }
        ind++;
    }
    LeaveCriticalSection(&secScriptFiles);
}

//vls-end//
