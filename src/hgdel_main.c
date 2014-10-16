/*----------------------------------------------------------------------------
    "hgdel_main.c"
        Hangame easy game avatar cache cleaner

    by ななこっち★
    (Microsoft C 32bit Compiler ver15.00/Visual Studio 2010)
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <windows.h>
#include <winreg.h>
#include <string.h>
#include <tchar.h>
#include <direct.h>
#include <shellapi.h>
#include <shlwapi.h>
#pragma	comment(lib,"shlwapi.lib")
#pragma warning(disable : 4996)

#define	HKCU	HKEY_CURRENT_USER
#define	HGJP	_T("Software\\HanGame.Com\\JAPANESE")
#define	AVA		_T("Avatar")
#define	HGHOME	_T("Home")
#define	appname	_T("ハンゲームアバターキャッシュクリーナー")
#define	STRLEN	1000

int	readhgdir(LPSTR str) {
	HKEY	hkey;
	DWORD DWordValue,Type,Size;
	int iResult;

	iResult=RegOpenKeyEx(HKCU, HGJP, 0, KEY_READ, &hkey);
	if(iResult != ERROR_SUCCESS)
		return 1;

	Size=sizeof(DWordValue);
	RegQueryValueEx(hkey, HGHOME, NULL, &Type, NULL, &Size);

	iResult=RegQueryValueEx(hkey, HGHOME, NULL, &Type, (LPBYTE)(LPCTSTR)str, &Size);
	if(iResult != ERROR_SUCCESS) {
		RegCloseKey(hkey);
		return 1;
	}

	RegCloseKey(hkey);
	return 0;
}

int findListSubdir( LPTSTR lpPath, int mode )
{
    WIN32_FIND_DATA fdFile;
    HANDLE          hFind;
    LPTSTR          lpWild;
	int				files=0;

    lpWild = PathAddBackslash( lpPath );
    lstrcpy( lpWild, TEXT("*.*") );

    if ( (hFind = FindFirstFile(lpPath,&fdFile)) != INVALID_HANDLE_VALUE ){
        do {
            if ( fdFile.cFileName[0] != TEXT('.') ){
                if ( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
                    lstrcpy( lpWild, fdFile.cFileName );
                    files+=findListSubdir( lpPath, mode);
                    /*
                    fdFile.cFileNameにフォルダ名
                    lpPathにフォルダのフルパス名
                    */
					if(!mode) {
						_rmdir(lpPath);
						files++;
					}
				} else{
                    lstrcpy( lpWild, fdFile.cFileName );
                    /*
                    fdFile.cFileNameにファイル名
                    lpPathにファイルのフルパス名
                    */
					if(mode) {
						unlink(lpPath);
						files++;
					}
				}
            }
        } while ( FindNextFile(hFind,&fdFile) );
        
        FindClose( hFind );
    }
    lpWild[ -1 ] = '\0';

	return files;
}


int APIENTRY WinMain(
HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	char	hginstallpath[STRLEN];
	char	hgavapath[STRLEN];
	char	hgtemppath[STRLEN];
	char	*hgmsg;

	int		_dirs, _files;

	if(strcmp(lpszCmdLine, "/?") == 0) {
		MessageBox(NULL, _T("ハンゲームアバターキャッシュクリーナー\n/?\tこのヘルプ\n/silent\t何も表示しない"), appname, MB_ICONINFORMATION | MB_OK);
		return 0;
	}

	if(readhgdir(hginstallpath) == 1) {
		if(strcmp(lpszCmdLine, "/silent") == 0) {
			return 1;
		}
		MessageBox(NULL, _T("ハンゲームかんたんゲームがインストールされていません"), appname, MB_ICONINFORMATION | MB_OK);
		return 1;
	}

	if(*hginstallpath == '\0') {
		return 1;
	}

	strcpy(hgavapath, hginstallpath);
	strcat(hgavapath, TEXT("\\"));
	strcat(hgavapath, AVA);
//	strcpy(hgtemppath,hgavapath);
//	strcat(hgtemppath,TEXT("_"));
//	strcat(hgavapath, TEXT("\\"));
//	strcat(hgtemppath, TEXT("\\"));

//	files = findListSubdir(hgtemppath, 1);
//	dirs = findListSubdir(hgtemppath, 0);

//	rename(hgavapath, hgtemppath);
//	_mkdir(hgavapath);
//	files = findListSubdir(hgtemppath, 1);
//	dirs = findListSubdir(hgtemppath, 0);

	_files = findListSubdir(hgavapath, 1);
	_dirs = findListSubdir(hgavapath, 0);
	_rmdir(hgavapath);
	_mkdir(hgavapath);

	if(strcmp(lpszCmdLine, "/silent") != 0) {
		hgmsg = malloc(STRLEN);
		sprintf(hgmsg, "%s より\n%d フォルダ %d ファイルを削除しました。", hgavapath, _dirs, _files);

		MessageBox(NULL, hgmsg, appname, MB_ICONINFORMATION | MB_OK);

		free(hgmsg);
	}
	return 0;
}
