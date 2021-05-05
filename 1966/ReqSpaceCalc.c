#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "main.h"
#include "system.h"
#include "ReqSpaceCalc.h"

unsigned int CalculateRequiredSpace(const struct FileCopyTarget *FileCopyList, unsigned int NumFiles, unsigned int NumDirectories){
	unsigned int i, TotalRequiredSpace;

	DEBUG_PRINTF("File list:\n");

	for(TotalRequiredSpace=0,i=0; i<NumFiles+NumDirectories; i++){
		DEBUG_PRINTF("%c%c%c%c%c%c%c%c%c%c %u %s\n", FIO_S_ISDIR(FileCopyList[i].mode)?'d':'-', FileCopyList[i].mode&FIO_S_IRUSR?'r':'-', FileCopyList[i].mode&FIO_S_IWUSR?'w':'-', FileCopyList[i].mode&FIO_S_IXUSR?'x':'-', FileCopyList[i].mode&FIO_S_IRGRP?'r':'-', FileCopyList[i].mode&FIO_S_IWGRP?'w':'-', FileCopyList[i].mode&FIO_S_IXGRP?'x':'-', FileCopyList[i].mode&FIO_S_IROTH?'r':'-', FileCopyList[i].mode&FIO_S_IWOTH?'w':'-', FileCopyList[i].mode&FIO_S_IXOTH?'x':'-', FileCopyList[i].size, FileCopyList[i].target);

		if(!FIO_S_ISDIR(FileCopyList[i].mode)){
			TotalRequiredSpace+=(FileCopyList[i].size+0x3FF)&~0x3FF;
		}
	}

	return TotalRequiredSpace;
}
