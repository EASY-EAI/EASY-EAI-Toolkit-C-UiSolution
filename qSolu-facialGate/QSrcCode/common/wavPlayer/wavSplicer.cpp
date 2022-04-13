#include "common/system.h"
#include "wavSplicer.h"

#pragma pack(1)	//取消字节对齐
typedef struct{
	char       chunk_ID[4];		//一般是RIFF
	__uint32_t chunk_Size;		//数据域的大小（不包含ChunkID和ChunkSize，所以比实际文件大小少了8个Byte)
	char       chunk_Format[4];	//WAV 文件, 一般是WAVE
	char       subChunk1_ID[4];	//WAV 文件的 SUB_CHUNK, 一般是fmt
	__uint32_t subChunk1_Size;	//一个“fmt” 的大小,若采用 PCM 编码, 则该长度为 16 字节
	__uint16_t audioFormat;		//编码格式：1-pcm
	__uint16_t numChannels;		//声道数量：1-单声道，2-双声道
	__uint32_t sampleRate;		//采样率，小端方式，大小为 0x0000AC44 = 44100 Hz。
	__uint32_t byteRate;		//比特速率，小端方式，大小为 0x0002B110 = 44100 * 16 * 2 = 1411200 bps。
	__uint16_t blockAlign;		//对齐，小端方式，0x0004 表示 4 字节对齐。
	__uint16_t bitsPerSample;	//采样位深，即每个采样点需要的位数，小端方式，0x0010 = 16 bit。
	char       subChunk2_ID[4];	//WAV 文件的 SUB_CHUNK, 一般为 “data”
	__uint32_t subChunk2_Size;	//数据内容的大小
}wavHead_t;
#pragma pack()

static void readDataFromLittle(__uint32_t *pVale, char **pOpt, int byte_num)
{
	if(byte_num <= 0)
		return;
	
	for(int i = 0; i < byte_num; i++){
		*pVale |= (((**pOpt)&0xff)<<(i*8));
		(*pOpt)++;
	}
}
static void writeDataFromLittle(char **pOpt, __uint32_t *pVale, int byte_num)
{
	if(byte_num <= 0)
		return;
	
	for(int i = 0; i < byte_num; i++){
		**pOpt = (((*pVale)>>(i*8))&0xff);
		(*pOpt)++;
	}
}
static void readDataFromBig(__uint32_t *pVale, char **pOpt, int byte_num)
{
	if(byte_num <= 0)
		return;
	
	for(int i = (byte_num-1); i >= 0; i--){
		*pVale |= (((**pOpt)&0xff)<<(i*8));
		(*pOpt)++;
	}
}

static int readHeadInfo(int fd, wavHead_t *pHeadInfo)
{
	char *pBuffOpt = NULL;
		
	char *pInfoBuff = NULL;
    pInfoBuff = (char *)malloc(sizeof(wavHead_t));
	if(pInfoBuff){
		lseek(fd, 0, SEEK_SET);
		read(fd, pInfoBuff, sizeof(wavHead_t));	
		pBuffOpt = pInfoBuff;
		
		/*get chunk ID*/
		pHeadInfo->chunk_ID[0] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->chunk_ID[1] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->chunk_ID[2] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->chunk_ID[3] = *pBuffOpt;
		pBuffOpt++;
		/*get chunk Size*/
		readDataFromLittle(&pHeadInfo->chunk_Size, &pBuffOpt, sizeof(pHeadInfo->chunk_Size));
		/*get chunk Format*/
		pHeadInfo->chunk_Format[0] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->chunk_Format[1] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->chunk_Format[2] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->chunk_Format[3] = *pBuffOpt;
		pBuffOpt++;
		/*get subChunk1 ID*/
		pHeadInfo->subChunk1_ID[0] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->subChunk1_ID[1] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->subChunk1_ID[2] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->subChunk1_ID[3] = *pBuffOpt;
		pBuffOpt++;
		/*get subChunk1 Size*/
		readDataFromLittle(&pHeadInfo->subChunk1_Size, &pBuffOpt, sizeof(pHeadInfo->subChunk1_Size));
		/*get audioFormat*/
		readDataFromLittle((__uint32_t *)&pHeadInfo->audioFormat, &pBuffOpt, sizeof(pHeadInfo->audioFormat));
		/*get numChannels*/
		readDataFromLittle((__uint32_t *)&pHeadInfo->numChannels, &pBuffOpt, sizeof(pHeadInfo->numChannels));
		/*get sampleRate*/
		readDataFromLittle(&pHeadInfo->sampleRate, &pBuffOpt, sizeof(pHeadInfo->sampleRate));
		/*get byteRate*/
		readDataFromLittle(&pHeadInfo->byteRate, &pBuffOpt, sizeof(pHeadInfo->byteRate));
		/*get blockAlign*/
		readDataFromLittle((__uint32_t *)&pHeadInfo->blockAlign, &pBuffOpt, sizeof(pHeadInfo->blockAlign));
		/*get bitsPerSample*/
		readDataFromLittle((__uint32_t *)&pHeadInfo->bitsPerSample, &pBuffOpt, sizeof(pHeadInfo->bitsPerSample));
		/*get subChunk2 ID*/
		pHeadInfo->subChunk2_ID[0] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->subChunk2_ID[1] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->subChunk2_ID[2] = *pBuffOpt;
		pBuffOpt++;
		pHeadInfo->subChunk2_ID[3] = *pBuffOpt;
		pBuffOpt++;
		/*get subChunk2_Size*/
		readDataFromLittle(&pHeadInfo->subChunk2_Size, &pBuffOpt, sizeof(pHeadInfo->subChunk2_Size));
#if 0
		/*prinf head info*/
		printf("========== [chunk_ID]:       %c%c%c%c\n", pHeadInfo->chunk_ID[0], pHeadInfo->chunk_ID[1], pHeadInfo->chunk_ID[2], pHeadInfo->chunk_ID[3]);
		printf("========== [chunk_Size]:     %u\n", pHeadInfo->chunk_Size);
		printf("========== [chunk_Format]:   %c%c%c%c\n", pHeadInfo->chunk_Format[0], pHeadInfo->chunk_Format[1], pHeadInfo->chunk_Format[2], pHeadInfo->chunk_Format[3]);
		printf("========== [subChunk1_ID]:   %c%c%c%c\n", pHeadInfo->subChunk1_ID[0], pHeadInfo->subChunk1_ID[1], pHeadInfo->subChunk1_ID[2], pHeadInfo->subChunk1_ID[3]);
		printf("========== [subChunk1_Size]: %u\n", pHeadInfo->subChunk1_Size);
		printf("========== [audioFormat]:    %u\n", pHeadInfo->audioFormat);
		printf("========== [numChannels]:    %u\n", pHeadInfo->numChannels);
		printf("========== [sampleRate]:     %u\n", pHeadInfo->sampleRate);
		printf("========== [byteRate]:       %u\n", pHeadInfo->byteRate);
		printf("========== [blockAlign]:     %u\n", pHeadInfo->blockAlign);
		printf("========== [bitsPerSample]:  %u\n", pHeadInfo->bitsPerSample);
		printf("========== [subChunk2_ID]:   %s\n", pHeadInfo->subChunk2_ID);
		printf("========== [subChunk2_Size]: %u\n", pHeadInfo->subChunk2_Size);
#endif
		free(pInfoBuff);
		return 0;
	}else{
		return -1;
	}	
}

static int writeHeadInfo(int fd, wavHead_t *pHeadInfo)
{
	char *pBuffOpt = NULL;
		
	char *pInfoBuff = NULL;
    pInfoBuff = (char *)malloc(sizeof(wavHead_t));
	if(pInfoBuff){
		pBuffOpt = pInfoBuff;
			
		/*get chunk ID*/
		*pBuffOpt = pHeadInfo->chunk_ID[0];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->chunk_ID[1];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->chunk_ID[2];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->chunk_ID[3];
		pBuffOpt++;
		/*get chunk Size*/
		writeDataFromLittle(&pBuffOpt, &pHeadInfo->chunk_Size, sizeof(pHeadInfo->chunk_Size));
		/*get chunk Format*/
		*pBuffOpt = pHeadInfo->chunk_Format[0];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->chunk_Format[1];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->chunk_Format[2];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->chunk_Format[3];
		pBuffOpt++;
		/*get subChunk1 ID*/
		*pBuffOpt = pHeadInfo->subChunk1_ID[0];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->subChunk1_ID[1];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->subChunk1_ID[2];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->subChunk1_ID[3];
		pBuffOpt++;
		/*get subChunk1 Size*/
		writeDataFromLittle(&pBuffOpt, &pHeadInfo->subChunk1_Size, sizeof(pHeadInfo->subChunk1_Size));
		/*get audioFormat*/
		writeDataFromLittle(&pBuffOpt, (__uint32_t *)&pHeadInfo->audioFormat, sizeof(pHeadInfo->audioFormat));
		/*get numChannels*/
		writeDataFromLittle(&pBuffOpt, (__uint32_t *)&pHeadInfo->numChannels, sizeof(pHeadInfo->numChannels));
		/*get sampleRate*/
		writeDataFromLittle(&pBuffOpt, &pHeadInfo->sampleRate, sizeof(pHeadInfo->sampleRate));
		/*get byteRate*/
		writeDataFromLittle(&pBuffOpt, &pHeadInfo->byteRate, sizeof(pHeadInfo->byteRate));
		/*get blockAlign*/
		writeDataFromLittle(&pBuffOpt, (__uint32_t *)&pHeadInfo->blockAlign, sizeof(pHeadInfo->blockAlign));
		/*get bitsPerSample*/
		writeDataFromLittle(&pBuffOpt, (__uint32_t *)&pHeadInfo->bitsPerSample, sizeof(pHeadInfo->bitsPerSample));
		/*get subChunk2 ID*/
		*pBuffOpt = pHeadInfo->subChunk2_ID[0];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->subChunk2_ID[1];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->subChunk2_ID[2];
		pBuffOpt++;
		*pBuffOpt = pHeadInfo->subChunk2_ID[3];
		pBuffOpt++;
		/*get subChunk2_Size*/
		writeDataFromLittle(&pBuffOpt, &pHeadInfo->subChunk2_Size, sizeof(pHeadInfo->subChunk2_Size));
#if 0
		/*prinf head info*/
		printf("========== [chunk_ID]:       %c%c%c%c\n", pHeadInfo->chunk_ID[0], pHeadInfo->chunk_ID[1], pHeadInfo->chunk_ID[2], pHeadInfo->chunk_ID[3]);
		printf("========== [chunk_Size]:     %u\n", pHeadInfo->chunk_Size);
		printf("========== [chunk_Format]:   %s\n", pHeadInfo->chunk_Format);
		printf("========== [subChunk1_ID]:   %s\n", pHeadInfo->subChunk1_ID);
		printf("========== [subChunk1_Size]: %u\n", pHeadInfo->subChunk1_Size);
		printf("========== [audioFormat]:    %u\n", pHeadInfo->audioFormat);
		printf("========== [numChannels]:    %u\n", pHeadInfo->numChannels);
		printf("========== [sampleRate]:     %u\n", pHeadInfo->sampleRate);
		printf("========== [byteRate]:       %u\n", pHeadInfo->byteRate);
		printf("========== [blockAlign]:     %u\n", pHeadInfo->blockAlign);
		printf("========== [bitsPerSample]:  %u\n", pHeadInfo->bitsPerSample);
		printf("========== [subChunk2_ID]:   %s\n", pHeadInfo->subChunk2_ID);
		printf("========== [subChunk2_Size]: %u\n", pHeadInfo->subChunk2_Size);
#endif
		lseek(fd, 0, SEEK_SET);
		write(fd, pInfoBuff, sizeof(wavHead_t));
		
		free(pInfoBuff);
		return 0;
	}else{
		return -1;
	}	
}

static int readWavData(int fd, char *pData, int dataLen, int offset)
{
	char *pOpt = pData;
	int readLenSum = 0;
	int tempReadLen = 0;
	
	if(dataLen <= 0)
		return 0;
	
	lseek(fd, 0, SEEK_SET);
	lseek(fd, offset, SEEK_SET);
	while(1){
		tempReadLen = read(fd, pOpt, 1024);
		if(tempReadLen >= 0){
			//printf("readLenSum = %d, tempReadLen = %d\n",readLenSum,tempReadLen);
			readLenSum += tempReadLen;
			pOpt += tempReadLen;
		}else{
			return -1;
		}
		
		if(readLenSum >= dataLen)
			break;
		
		//usleep(10*1000);
	}
	return 0;
}

int wavSplicing(const char *targetFile, std::queue<std::string> srcFileList)
{
	int targetWavFd = -1;
    wavHead_t targetHeadInfo;
    memset(&targetHeadInfo,0,sizeof(wavHead_t));

	int tempWavFd = -1;
    wavHead_t tempHeadInfo;
    memset(&tempHeadInfo,0,sizeof(wavHead_t));

    std::queue<int> tempDataLenList;
    std::queue<char *> tempDataList;
    std::string srcFilePath;
    int tempDataLen = 0;
    char *tempData = NULL;

    if(srcFileList.empty()){
        printf("src Files List is empty\n");
        return -1;
    }
    srcFilePath = srcFileList.front();
    srcFileList.pop();
	
	targetWavFd = open(targetFile, O_WRONLY|O_CREAT, 0777);
	if(targetWavFd > 0) {
		printf("targetWav open succ ...\n");
		
        tempWavFd = open(srcFilePath.c_str(), O_RDONLY);
		if(tempWavFd <= 0)
            return -1;

        //1, 获取头部初始数据(以首个文件作为基准)
		readHeadInfo(tempWavFd, &tempHeadInfo);
        memcpy(&targetHeadInfo, &tempHeadInfo, sizeof(wavHead_t));
		//读取音频数据
		if(tempHeadInfo.subChunk2_Size){
            tempDataLen = tempHeadInfo.subChunk2_Size;
            tempData = (char *)malloc(tempDataLen);
		}
        if(tempData)
            readWavData(tempWavFd, tempData, tempDataLen, sizeof(wavHead_t));
		close(tempWavFd);
        tempWavFd = -1;
        //把读出的音频数据送入队列
        if(tempData){
            tempDataLenList.push(tempDataLen);
            tempDataLen = 0;
            tempDataList.push(tempData);
            tempData = NULL;
        }

        //2, 如果有两个文件以上则把它们也拼接上
        while(1)
		{
            //已经没有更多需要拼接的文件
            if(srcFileList.empty())
                break;

            srcFilePath = srcFileList.front();
            srcFileList.pop();
            tempWavFd = open(srcFilePath.c_str(), O_RDONLY);
			if(tempWavFd <= 0){
                //打开失败，不合并该文件，下一个
                continue;
			}

			//获取头部初始数据
			memset(&tempHeadInfo, 0, sizeof(wavHead_t));
			readHeadInfo(tempWavFd, &tempHeadInfo);
#if 0   //尚未开发此功能
            //检查头信息是否一致，不一致则丢弃
            if(0 != checkHeadInfo(targetHeadInfo, tempHeadInfo)){
                close(tempWavFd);
                continue;
            }
#endif
			targetHeadInfo.subChunk2_Size += tempHeadInfo.subChunk2_Size;

			//读取音频数据
			if(tempHeadInfo.subChunk2_Size){
                tempDataLen = tempHeadInfo.subChunk2_Size;
                tempData = (char *)malloc(tempDataLen);
			}
            if(tempData){
                readWavData(tempWavFd, tempData, tempDataLen, sizeof(wavHead_t));
            }
            //把读出的音频数据送入队列
            if(tempData){
                tempDataLenList.push(tempDataLen);
                tempDataLen = 0;
                tempDataList.push(tempData);
                tempData = NULL;
            }
			
			close(tempWavFd);
        }
		
        //3, 构造拼接文件
		//写入最终头部信息
		targetHeadInfo.chunk_Size = targetHeadInfo.subChunk2_Size + (sizeof(wavHead_t) - 8);
        writeHeadInfo(targetWavFd, &targetHeadInfo);
		/*写入真实数据*/ /*此处用for*/
        while(1){
            if(tempDataLenList.empty()){
                if(tempDataList.empty())
                    break;
                else{
                    tempData = tempDataList.front();
                    free(tempData);
                    tempDataList.pop();
                    continue;
                }
            }
            if(tempDataList.empty()){
                if(tempDataLenList.empty())
                    break;
                else{
                    tempDataLenList.pop();
                    continue;
                }
            }

            tempDataLen = tempDataLenList.front();
            tempData = tempDataList.front();

            write(targetWavFd, tempData, tempDataLen);
            free(tempData);

            tempDataLenList.pop();
            tempDataList.pop();
        }

        close(targetWavFd);
        return 0;
	} else {
		printf("targetWav open faild ...\n");
		return -1;
	}
}
