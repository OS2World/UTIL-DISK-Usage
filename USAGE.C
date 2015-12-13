/* print out the disk usage of all subdirectories */

#define INCL_DOS
#define INCL_DOSERRORS

#include <os2.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

long asize;
unsigned depth = 0;
unsigned max = -1;

long prsize(char *);
void cdecl main(int, char **);

void cdecl main(argc,argv) int argc; char **argv; {
SHORT n,drive = 0;
char root[255],temp[255];
FSALLOCATE space;

cputs("\r\nDirectory usage tabulator   (c) 1990 ALF Products Inc.\r\n\n");
temp[0]='\0';
for (n=1;n < argc;n++) {
   if (!strcmp(argv[n],"?")) {
      cputs("Invocation:   USAGE [path] [/n]\r\n\n");
      cputs("path:   The pathname of the directory that will have its\r\n");
      cputs("        file usage tabulated.\r\n\n");
      cputs("n:      The depth of subdirectories that will be printed.\r\n");
      cputs("        '/0' will not print any subdirectories.\r\n");
      exit(0); }
   else if (argv[n][0] == '/') max=atoi(argv[n]+1);
   else if (argv[n][1] == ':') {
      strcpy(temp,argv[n]+2);
      drive=toupper(argv[n][0])-'A'+1; }
   else strcpy(temp,argv[n]);
   }
strcpy(root,drive ? "@:\\" : "\\"); root[0]+=(char)drive;

n=120;
if (DosQCurDir(drive,(PBYTE)(root+strlen(root)),&n))
	{ cputs("Invalid drive.\r\n"); exit(1); }
DosQFSInfo(drive,1,(PBYTE)(&space),sizeof(space));
asize=space.cbSector*space.cSectorUnit;
if (temp[0]=='\\') strcpy(root+2*(root[1]==':'),temp);
else {
   if (root[strlen(root)-1] != '\\') strcat(root,"\\");
   strcat(root,temp); }
if (root[strlen(root)-1] != '\\') strcat(root,"\\");
prsize(root);
}

long prsize(instr) char *instr; {
long total = 0;
long ftotal = 0;
long hdisk = 0;
long disk = 0;
long size;
SHORT dummy;
char tstr[255];
unsigned err;
HDIR hdir;
FILEFINDBUF files;

strcpy(tstr,instr); strcat(tstr,"*.*");
hdir = HDIR_CREATE;
dummy = 1;
err=DosFindFirst(tstr,&hdir,FILE_NORMAL|FILE_HIDDEN|FILE_SYSTEM|FILE_DIRECTORY,
	&files,sizeof(files),&dummy,0L);
while (!err) {
   if (files.attrFile & FILE_DIRECTORY) {
      if (strcmp(files.achName,".") && strcmp(files.achName,"..")) {
         strcpy(tstr,instr); strcat(tstr,files.achName);
         depth++; total+=prsize(strcat(tstr,"\\")); depth--;
      }
	}
   else {
      disk+=files.cbFileAlloc+1023&0xfffffc00;
      hdisk+=files.cbFileAlloc+511&0xfffffe00;
      size=((files.cbFileAlloc+asize-1)/asize)*asize;
      total+=size; ftotal+=size;
	}
	dummy = 1;
   err=DosFindNext(hdir,&files,sizeof(files),&dummy);
}
DosFindClose(hdir);
if (err != ERROR_NO_MORE_FILES) { cputs("Invalid pathname.\r\n"); exit(1); }
strcpy(tstr,instr); if (tstr[1]) tstr[strlen(tstr)-1]='\0';
if (depth <= max) {
   if (disk <= 362496l) printf("*");
   else if (disk <= 1213952l) printf("+");
   else printf(" ");
   printf("%5ld Kbytes in files, %5ld Kbytes total in %s\r\n",
      ftotal>>10,total>>10,tstr);
   }
return(total);
}
