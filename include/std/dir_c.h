
#ifndef _DIR_C_H_
#define _DIR_C_H_

#include <sys/stat.h>
#include <sys/types.h>
#include "stddef_c.h"
#include "inttypes_c.h"
//#include "stdc.h"
#include "path_c.h"
#include <io.h>
#include <direct.h>


#ifndef MAX_PATH
#define MAX_PATH 260
#endif // MAX_PATH
typedef struct fileinfo_t {
  unsigned attrib; // _A_SUBDIR
  time_t ctime;//_create;    /* -1 for FAT file systems */
  time_t atime;//access;    /* -1 for FAT file systems */
  time_t wtime;//write;
  uint64 size;
  char name[MAX_PATH];
} fileinfo_t;
typedef struct findfile_t {
  void* dir;
  int count;
} findfile_t;
#define FF_FULLNAME  (1<<0)
#define FF_SUBDIR    (1<<1)
#define FF_NODIR     (1<<3)
#define FF_FILTERS   (1<<4)


#define filefind_is_directory(s)  (FILE_ATTRIBUTE_DIRECTORY & s->info.dwFileAttributes)
#define filefind_filename(s)      (s->info->name)
#define filefind_filesize(info)   (s->info.nFileSizeLow)
// finename
typedef struct dirlist_t {
  fileinfo_t* v;
  int n;
} dirlist_t;

CC_INLINE int dirlist_free(dirlist_t* s)
{
  FREE(s->v);
  s->v = NULL;
  s->n = 0;
  return 0;
}

typedef enum {
  AS_EXIST = 0, AS_EXECUTE = 1, AS_WRITE = 2, AS_READ = 4, AS_SUBDIR = 8, AS_RDONLY = 16, AS_HIDDEN, AS_WRITE_READ = AS_WRITE | AS_READ
} FILE_ACCESS_MODE;
typedef enum {
  FA_READONLY = 0x00000001,
  FA_HIDDEN = 0x00000002,
  FA_SYSTEM = 0x00000004,
  FA_DIRECTORY = 0x00000010,
  FA_ARCHIVE = 0x00000020,
  FA_DEVICE = 0x00000040,
  FA_NORMAL = 0x00000080,
  FA_TEMPORARY = 0x00000100,
  FA_SPARSE_FILE = 0x00000200,
  FA_REPARSE_POINT = 0x00000400,
  FA_COMPRESSED = 0x00000800,
  FA_OFFLINE = 0x00001000,
  FA_NOT_CONTENT_INDEXED = 0x00002000,
  FA_ENCRYPTED = 0x00004000,
} FILE_ATTRIBUTE;
#undef S_ISDIR
#define S_ISDIR(x)  ((x)&AS_SUBDIR)


#ifdef _WIN32
#define S_IFMT         _S_IFMT           /* file type mask */
#define S_IFDIR        _S_IFDIR          /* directory */
#define S_IFCHR        _S_IFCHR          /* character special */
#define S_IFIFO        _S_IFIFO          /* pipe */
#define S_IFREG        _S_IFREG          /* regular */
#define S_IREAD        _S_IREAD          /* read permission, owner */
#define S_IWRITE       _S_IWRITE         /* write permission, owner */
#define S_IEXEC        _S_IEXEC          /* execute/search permission, owner */
static int win32_attrib_cvt(int attrib) {
  int attrib2 = 0;
#define WIN32_ATTRIB_CVT(a, b) if (attrib&(a))  attrib2|=(b);
  WIN32_ATTRIB_CVT(_A_RDONLY, AS_RDONLY);
  WIN32_ATTRIB_CVT(_A_SUBDIR, AS_SUBDIR);
  WIN32_ATTRIB_CVT(_A_HIDDEN, AS_HIDDEN);
  return attrib2;
}
int sys_find_close(findfile_t* s) {
  if (s && s->dir) {
    _findclose((intptr_t)s->dir);
    s->dir = 0;
  }
  return 0;
}

int sys_find_next_file(findfile_t* s, const char* path, const char* filters, fileinfo_t* f, int flag) {
  struct _finddata_t info[1];
  char* name = f->name;
  int maxnamelen = countof(f->name);
  if (NULL == s->dir) {
    char buf[256];
    _snprintf(buf, 256, "%s/%s", path, filters ? filters : "*");
    s->dir = (void*)_findfirst(buf, info);
    s->count++;
    f->size = info->size;
    f->ctime = info->time_create;
    f->atime = info->time_access;
    f->wtime = info->time_write;
    f->attrib = win32_attrib_cvt(info->attrib);
    if (flag & (FF_FULLNAME | FF_SUBDIR)) {
      _snprintf(name, maxnamelen, "%s/%s", path, info->name);
    }
    else {
      strncpy(name, info->name, maxnamelen);
    }
    return ((void*)-1 != s->dir && NULL != s->dir);
  }
  else {
    if (_findnext((intptr_t)s->dir, info) == 0) {
      s->count++;
      f->size = info->size;
      f->ctime = info->time_create;
      f->atime = info->time_access;
      f->wtime = info->time_write;
      f->attrib = win32_attrib_cvt(info->attrib);
      if (flag & (FF_FULLNAME | FF_SUBDIR)) {
        _snprintf(name, MAX_PATH, "%s/%s", path, info->name);
      }
      else {
        strncpy(name, info->name, maxnamelen);
      }
      return TRUE;
    }
    else {
      _findclose((intptr_t)s->dir);
      s->dir = 0;
    }
  }
  return FALSE;
}
#else
// linux

static int linux_attrib_cvt(int attrib) {
  int attrib2=0;
#define LINUX_ATTRIB_CVT(a, b) if (a(attrib))  attrib2|=(b);
  LINUX_ATTRIB_CVT(S_ISDIR, AS_SUBDIR);
  return attrib2;
}

int sys_find_close(findfile_t* s) {
  if (s && s->dir) {
    closedir((DIR*)s->dir);
    s->dir = NULL;
  }
  return 0;
}
int sys_find_next_file(findfile_t* s, const char* path, const char* filters, fileinfo_t* f, int flag) {
  struct dirent* d_ent = NULL;
  filters = filters;
  if (NULL==s->dir) {
    if (!(s->dir = opendir(path))) {
      return 0;
    }
  }
  if ((d_ent = readdir((DIR*)s->dir)) != NULL) {
    struct stat file_stat;
    if (flag & (FF_FULLNAME|FF_SUBDIR)) {
      snprintf(f->name, MAX_PATH, "%s/%s", path, d_ent->d_name);
    } else {
      strncpy(f->name, d_ent->d_name, MAX_PATH);
    }
    if (lstat(f->name, &file_stat) < 0) {
      //assert(FALSE);
      return 0;
    }
    f->size = file_stat.st_size;
    f->ctime = file_stat.st_ctime;
    f->atime = file_stat.st_atime;
    f->wtime = file_stat.st_mtime;
    f->attrib = linux_attrib_cvt(file_stat.st_mode);
    return 1;
  }
  closedir((DIR*)s->dir);
  s->dir = NULL;
  return 0;
}
#endif

static int is_directory(const char* path) {
  struct stat info = { 0 };
  int ret = stat(path, &info);
  int IsDirPath = 0;
  if (ret >= 0) {
    IsDirPath = (S_IFDIR == (S_IFDIR & info.st_mode));
  }
  return IsDirPath;
}
// flag - FF_FULLNAME | FF_SUBDIR
static int dirlist(dirlist_t* s, const char* path, const char* filters, int flag)
{
  findfile_t finfo[1] = { 0 };
  fileinfo_t info[1] = { 0 };
  for (; sys_find_next_file(finfo, path, "*", info, flag);) {
    BOOL isok = true;
    //printf("%s %d\n", info->name, info->attrib);
    if (info->attrib&AS_SUBDIR) {
      char* name = GetFileNameExt(info->name);
      if (!FN_IS_DOTS(name)) {
        if ((flag & FF_SUBDIR) && filters) {
          dirlist(s, info->name, filters, flag);
        }
        isok = !(flag & FF_NODIR);
      }
      else {
        isok = false;
      }
    }
    {
      char* ext = GetFileExt(info->name);
      if (filters && *filters) {
        isok = false;
        if (ext && *ext) {
          isok = endwith(filters, ext, 0);
        }
      }
      if (isok) {
        int n = s->n++;
        MYREALLOC(s->v, s->n);
        s->v[n] = *info;
      }
    }
  }
  return 0;
}
#include "str_c.h"
static int dirvstr(vstr_t* sv, const char* path, const char* filters, int flag) {
  dirlist_t dl[1] = { 0 };
  int i;
  dirlist(dl, path, filters, flag);
  vstr_setsize(sv, dl->n);
  for (i = 0; i<dl->n; ++i) {
    vstr_set_str(sv, i, STR1(dl->v[i].name));
  }
  dirlist_free(dl);
  return 0;
}
#if 0
static int dirlist_c(char*** plist, const char* path, const char* filters, int flag) {
  dirlist_t s[1] = { 0 };
  int i;
  dirlist(s, path, filters, flag);
  vcstr_setsize(plist, s->n);
  for (i = 0; i<s->n; ++i) {
    cstr_redup((*plist) + i, s->v[i].name);
  }
  dirlist_free(s);
  return 0;
}
static int dirlistW(dirlist_t* s, const char* path, const char* filters, int flag)
{
  findfile_t finfo[1] = { 0 };
  fileinfo_t info[1] = { 0 };
  for (; sys_find_next_file(finfo, path, "*", info, flag);) {
    BOOL isok = true;
    if (S_ISDIR(info->attrib)) {
      char* name = GetFileNameExt(info->name);
      isok = !FN_IS_DOTS(name);
      if (isok && (flag & FF_SUBDIR) && filters) {
        dirlist(s, info->name, filters, flag);
      }
    }
    if (isok) {
      char* ext = strrchr(info->name, '.');
      isok = ext ? (cstr_find(filters, -1, ext, -1, 1, 0)>0) : 0;
    }
    if (isok) {
      int n = s->n++;
      MYREALLOC(s->v, s->n);
      s->v[n] = *info;
    }
  }
  return 0;
}
#endif


//////////////////////////////////////////////////

static int sys_rmfile(const char *pathname) {
#ifdef _WIN32
  return DeleteFileA(pathname);
#else
  return unlink(pathname);
#endif
}
//recursively delete all the file in the directory.
int sys_rmdir(const char* dir_full_path)
{
#ifdef _WIN32
  return RemoveDirectoryA(dir_full_path);
#else
  DIR* dirp = opendir(dir_full_path);
  if (!dirp) {
    return -1;
  }
  struct dirent *dir;
  struct stat st;
  while ((dir = readdir(dirp)) != NULL)
  {
    if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
      continue;
    }
    char sub_path[256];
    _snprintf(sub_path, countof(sub_path), "%s/%s", dir_full_path, dir->d_name);
    if (lstat(sub_path, &st) == -1) {
      printf("sys_rmdir:lstat %s error", sub_path);
      continue;
    }
    if (S_ISDIR(st.st_mode)) { // 如果是目录文件，递归删除
      if (sys_rmdir(sub_path) == -1) {
        closedir(dirp);
        return -1;
      }
      rmdir(sub_path);
    }
    else if (S_ISREG(st.st_mode)) {
      unlink(sub_path);     // 如果是普通文件，则unlink
    }
    else {
      printf("sys_rmdir:st_mode %s error", sub_path);
      continue;
    }
  }
  if (rmdir(dir_full_path) == -1)//delete dir itself.
  {
    closedir(dirp);
    return -1;
  }
  closedir(dirp);
#endif
  return 0;
}
/////////////////////////

enum SYS_MODE {
  AS_IFMT = 0170000,  /* file type mask */
  AS_IFDIR = 0040000,  /* directory */
  AS_IFCHR = 0020000,  /* character special */
  AS_IFIFO = 0010000,  /* pipe */
  AS_IFREG = 0100000,  /* regular */
  AS_IREAD = 0000400,  /* read permission, owner */
  AS_IWRITE = 0000200,  /* write permission, owner */
  AS_IEXEC = 0000100,  /* execute/search permission, owner */

  AS_ISUID = 04000, //文件的 (set user-id on execution)位
  AS_ISGID = 02000, //文件的 (set group-id on execution)位
  AS_ISVTX = 01000, //文件的sticky 位
  AS_IRUSR = 00400, //文件所有者具可读取权限
  AS_IWUSR = 00200, //文件所有者具可写入权限
  AS_IXUSR = 00100, //文件所有者具可执行权限
  AS_IRGRP = 00040, //用户组具可读取权限
  AS_IWGRP = 00020, //用户组具可写入权限
  AS_IXGRP = 00010, //用户组具可执行权限
  AS_IROTH = 00004, //其他用户具可读取权限
  AS_IWOTH = 00002, //其他用户具可写入权限
  AS_IXOTH = 00001, //其他用户具可执行权限
};

typedef struct {
  uint dev;      //文件的设备编号
  uint ino; //节点
  uint mode; //文件的类型和存取的权限
  int nlink;  //连到该文件的硬连接数目，刚建立的文件值为1
  int uid;//用户ID
  int gid;//组ID
  uint rdev; //(设备类型)若此文件为设备文件，则为其设备编号
  uint64 size;//文件字节数(文件大小)
  uint blksize;   //块大小(文件系统的I/O 缓冲区大小)
  uint blocks;    //块数
  time_t atime; //最后一次访问时间
  time_t mtime;//最后一次修改时间
  time_t ctime;//最后一次改变时间(指属性)
  unsigned attrib; // _A_SUBDIR
} sys_stat;

//////////////////////////
int sys_filestat(const char* file, sys_stat* s) {
#ifdef _WIN32
  struct _stat st;
  int ret = _stat(file, &st);
  if (s) {
#define COPYMEMBER(_X)  s->_X = st.st_##_X
    COPYMEMBER(dev);
    COPYMEMBER(ino);
    COPYMEMBER(mode);
    COPYMEMBER(nlink);
    COPYMEMBER(uid);
    COPYMEMBER(gid);
    COPYMEMBER(rdev);
    COPYMEMBER(size);
    //COPYMEMBER(blksize);
    //COPYMEMBER(blocks);
    COPYMEMBER(atime);
    COPYMEMBER(mtime);
    COPYMEMBER(ctime);
#undef COPYMEMBER
}
#else
  struct stat st;
  int ret = stat(file, &st);
#define COPYMEMBER(_X)  s->_X = st.st_##_X
  COPYMEMBER(dev);
  COPYMEMBER(ino);
  COPYMEMBER(mode);
  COPYMEMBER(nlink);
  COPYMEMBER(uid);
  COPYMEMBER(gid);
  COPYMEMBER(rdev);
  COPYMEMBER(size);
  COPYMEMBER(blksize);
  COPYMEMBER(blocks);
  COPYMEMBER(atime);
  COPYMEMBER(mtime);
  COPYMEMBER(ctime);
#undef COPYMEMBER
#endif
  return ret;
}
int sys_mkdir(const char* fname) {
#ifdef _WIN32
  return _mkdir(fname);
#else
  return mkdir(fname, 0755);
#endif
}
//////////////////////////
static int rmdirs(const char* path) {
  if (is_directory(path)) {
    findfile_t finfo[1] = { 0 };
    fileinfo_t info[1] = { 0 };
    int flag = FF_FULLNAME;
    const char* filters = "";
    for (; sys_find_next_file(finfo, path, "*", info, flag);) {
      BOOL isok = true;
      //printf("%s %d\n", info->name, info->attrib);
      if (info->attrib&AS_SUBDIR) {
        char* name = GetFileNameExt(info->name);
        isok = !FN_IS_DOTS(name);
        if (isok) {
          rmdirs(info->name);
        }
      }
      else {
        sys_rmfile(info->name);
      }
    }
    sys_rmdir(path);
  }
  else {
    sys_rmfile(path);
  }
  return 0;
}
static int mkdirs(const char* filename)
{
  char path[MAX_PATH];
  char* p;
  int pos;
  sys_stat st;
  strcpy(path, filename);
  p = path;
  for (; ;) {
    int len = strlen(p);
    pos = strcspn(p, ("/\\"));
    if (pos != 0) {
      p[pos] = '\0';
      if (p[pos - 1] != (':')) {
        if (sys_filestat(path, &st) != 0) {
          if (sys_mkdir(path) != 0) {
            return 0;
          }
        }
      }
    }
    if (pos == len) {
      break;
    }
    p[pos] = '/';
    p += pos + 1;
  }
  return 1;
}
/////////////////////////////////////////////////////
#endif // _DIR_C_H_
