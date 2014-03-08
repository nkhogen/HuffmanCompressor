#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "priorityqueue.h"
#define TYPES_OF_CHARACTERS 255 // 1 character is used as marker that is 00000000

/* Node of the huffman tree */
typedef struct TheTreeNode{
    char letter;
	struct TheTreeNode* parent;
    struct TheTreeNode *left,*right;
}TreeNode, *TreeNodePtr;

typedef struct TheCharData{
	char character;
	int frequency;
	TreeNodePtr node;
} CharData;

ListHeadPtr createPriorityQueue(CharData* charData){
	ListHeadPtr listHeadPtr = NULL;
	if(charData != NULL){
		for(int i = 0; i < TYPES_OF_CHARACTERS; i++){
			if(charData[i].frequency > 0){
				TreeNodePtr treeNodePtr = (TreeNodePtr) malloc(sizeof(TreeNode));
				treeNodePtr->letter = charData[i].character;
				treeNodePtr->left = NULL;
				treeNodePtr->right = NULL;
				treeNodePtr->parent = NULL;
				charData[i].node = treeNodePtr;
				enqueue(listHeadPtr, treeNodePtr, charData[i].frequency);
			}
		}
	}
	return listHeadPtr;
}
TreeNodePtr createHuffmanCodeTree(CharData* charData){
	int nodeCount = 0;
	int totalPriority = 0;
	ListNodePtr firstNodePtr  = NULL;
	ListNodePtr secondNodePtr = NULL;
	TreeNodePtr treeNodePtr = NULL;
	ListHeadPtr listHeadPtr = createPriorityQueue(charData);
	if(listHeadPtr == NULL || listHeadPtr->size_of_list == 0){
		return NULL;
	}
	while(listHeadPtr->size_of_list > 1){
		firstNodePtr = dequeue(listHeadPtr);
		secondNodePtr = dequeue(listHeadPtr);
		totalPriority = firstNodePtr->priority + secondNodePtr->priority;
		treeNodePtr = (TreeNodePtr) malloc(sizeof(TreeNode));
		treeNodePtr->left =  (TreeNodePtr)(firstNodePtr->data);
		treeNodePtr->right = (TreeNodePtr)(secondNodePtr->data);
		treeNodePtr->letter = 0;
		treeNodePtr->parent = NULL;
		treeNodePtr->left->parent = treeNodePtr;
		treeNodePtr->right->parent = treeNodePtr;
		enqueue(listHeadPtr, treeNodePtr, totalPriority);
		free(firstNodePtr);
		free(secondNodePtr);
		nodeCount++;
	}
	if(nodeCount == 0){
		if(listHeadPtr->size_of_list > 0){
			firstNodePtr = dequeue(listHeadPtr);
			treeNodePtr = (TreeNodePtr) malloc(sizeof(TreeNode));
			treeNodePtr->left =  (TreeNodePtr)(firstNodePtr->data);
			treeNodePtr->right = NULL;
			treeNodePtr->letter = 0;
			treeNodePtr->parent = NULL;
			treeNodePtr->left->parent = treeNodePtr;
			free(firstNodePtr);
		}
		else{
			treeNodePtr = NULL;
		}
	}
	else {
		treeNodePtr = (TreeNodePtr)(listHeadPtr->next->data);
	}
	purge(listHeadPtr);
	return treeNodePtr;
}

void getCharData(FILE* inputFile, CharData* charData){
	int i = 0;
	int totalFrequency = 0;
	int fileData = 0;
	if(charData != NULL){
		for(i = 0; i < TYPES_OF_CHARACTERS; i++){
			charData[i].frequency = 0;
			charData[i].character = (char)(i+1);
			charData[i].node = NULL;
		}
		while ((fileData =fgetc(inputFile))!=EOF){
			if(fileData > 0 && fileData < TYPES_OF_CHARACTERS+1){
				charData[fileData-1].frequency++;
				totalFrequency++;
			}
		}
		if(totalFrequency > 0){
			for(i = 0; i < TYPES_OF_CHARACTERS; i++){
				if(charData[i].frequency > 0){
					charData[i].frequency = (charData[i].frequency * 255)/ totalFrequency; // can be represented by a char
					if(charData[i].frequency == 0){
						charData[i].frequency = 1;
					}
				}
			}
		}
	}
}
void getCharCode(CharData charData, char* code, int& codeLength){
	int index = 0;
	int i = 0;
	char tmpCh = 0;
	codeLength = 0;
	TreeNodePtr child = charData.node;
	if(child != NULL){
		TreeNodePtr parent = child->parent;
		while(parent != NULL){
			if(parent->left == child){
				code[index++] = 1;
			}
			else{
				code[index++] = 2;
			}
			codeLength++;
			child = parent;
			parent = parent->parent;
		}
	}
	code[index] = 0;
}
void writeHeader(CharData* charData, FILE* outputFile){
	for(int i = 0; i < TYPES_OF_CHARACTERS; i++){
		if(charData[i].frequency > 0){
			fputc(charData[i].character, outputFile);
			fputc(charData[i].frequency, outputFile);
		}
	}
	fputc(0,outputFile); // marker
}
int readHeader(CharData* charData, FILE* inputFile){
	int index = 0;
	int fileData = 0;
	int freqData = 0;
	for(index = 0; index < TYPES_OF_CHARACTERS; index++){
		charData[index].frequency = 0;
		charData[index].character = (char)(index+1);
		charData[index].node = NULL;
	}
	while (true){
		fileData = fgetc(inputFile);
		if( fileData == EOF){
			return fileData;
		}
		if(fileData == 0){
			break;
		}
		freqData = fgetc(inputFile);
		if(freqData > 0 && freqData < TYPES_OF_CHARACTERS+1){
			charData[fileData - 1].frequency = freqData;
		}
	}
	// Read the last bit count marker
	fileData = fgetc(inputFile);
	if(fileData == EOF){
		return fileData;
	}
	if((fileData & (1 << 7))){
		return fileData;
	}
	return EOF;
}
void purgeTree(TreeNodePtr treeNodePtr){
	if(treeNodePtr != NULL)
	{
		if(treeNodePtr->left != NULL){
			purgeTree(treeNodePtr->left);
		}
		if(treeNodePtr->right != NULL){
			purgeTree(treeNodePtr->right);
		}
		free(treeNodePtr);
	}
}
void compressFile(FILE* inputFile, FILE* outputFile){
	char code = 0;
	int codeLength = 0;
	int bitsLeft = 8;
	int charIndex = 0;
	int fileData = 0;
	char x = 0;
	char bit = 0;
	long markerPos = 0L;
	TreeNodePtr treeNodePtr = NULL;
	CharData charData[TYPES_OF_CHARACTERS];
	char charCode[TYPES_OF_CHARACTERS+10];
	getCharData(inputFile, charData);
	treeNodePtr = createHuffmanCodeTree(charData);
	if(treeNodePtr == NULL){
		printf("Huffman tree cannot be created!\n");
		return;
	}
	writeHeader(charData, outputFile);
	markerPos = ftell(outputFile);
	// write a dummy marker
	fputc(-1, outputFile);
	rewind(inputFile);
	while ((fileData=fgetc(inputFile))!=EOF){
		if(fileData > 0 && fileData < TYPES_OF_CHARACTERS+1){
			getCharCode(charData[fileData-1], charCode, codeLength);
			while (codeLength > 0){
				code = charCode[--codeLength];
				bit = code-1;
				x = x | bit;
				bitsLeft--;
				if(bitsLeft == 0){
					fputc(x, outputFile);
					x = 0;
					bitsLeft = 8;
				}
				x = x << 1;
			}
		}
	}
	if (bitsLeft!=8){
		x = x << (bitsLeft-1);
		fputc(x,outputFile);
	}
	fseek(outputFile,markerPos, SEEK_SET);
	// 1 is the marker for end marker
	x = ((char)(1<<7) | (char)(8-bitsLeft));
	fputc(x, outputFile);
	purgeTree(treeNodePtr);
}
/*function to decompress the input*/
void decompressFile (FILE *input, FILE *output){
    TreeNodePtr current = NULL;
	TreeNodePtr treeNodePtr = NULL;
    char c = 0;
	char bit = 0;
    char mask = 1<<7;
    int i = 0;
    int bitCount = 8;
	int fileData = 0;
	int nextFileData = 0;
	char lastNumOfBits = 0;
	CharData charData[TYPES_OF_CHARACTERS];
	fileData = readHeader(charData, input);
	if(fileData == EOF){
		return;
	}
	lastNumOfBits = (char)(fileData & 127);
	treeNodePtr = createHuffmanCodeTree(charData);
	current = treeNodePtr;
	nextFileData = fgetc(input);
    while (true)
	{
		fileData = nextFileData;
		if(fileData == EOF){
			break;
		}
		nextFileData = fgetc(input);
		if(nextFileData == EOF){
			bitCount = lastNumOfBits;
		}
		c = (char)fileData;
        for (i = 0; i < bitCount; i++){
            bit = c & mask;
            c = c<<1;
            if (bit==0){
                current = current->left;
				if (current->left == NULL && current->right == NULL){
					fputc(current->letter, output);
					current= treeNodePtr;
				}
            }

            else{
                current = current->right;
                if (current->left == NULL && current->right == NULL){
					fputc(current->letter, output);
					current= treeNodePtr;
                }
            }
        }
    }
	purgeTree(treeNodePtr);
	return;
}
int print_menu()
{
	int input;

	printf("----------Menu-----------\n");
	printf("Press 1 to Compress. \n");
	printf("Press 2 to Decompress. \n");
	printf("Press 3 to Exit the Program. \n");
	printf("----------Menu-----------\n");
	scanf("%d", &input);
	if ((input >= 0)&&(input <=3))
	{
		printf("\nThe User Option is: %d \n", input);

	}
	else{
		printf("\nPlease Try Again... \n");
		print_menu();
	}
	return input;
}

void main(){
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;
	char filename[256];
	int option = 0;
	do{
	    option = print_menu();
		switch(option)
		{
		case 1: printf("Enter the name of the text file to compress: ");
				scanf(" %s",filename);
				filename[255] = 0;
				inputFile = fopen(filename, "r");
				if(inputFile == NULL){
					printf("Unable to open the input file\n");
					break;
				}
				outputFile = fopen("compressedOut.txt","wb");
				if(outputFile == NULL){
					printf("Unable to open the output file\n");
					fclose(inputFile);
					break;
				}
				compressFile(inputFile,outputFile);
				fclose(inputFile);
				fclose(outputFile);
				printf("Successfully compressed as compressedOut.txt\n ");
			    break;
		case 2: printf("Enter the name of the text file to decompress: ");
				scanf(" %s",filename);
				filename[255] = 0;
				inputFile = fopen(filename, "rb");
				if(inputFile == NULL){
					printf("Unable to open the input file\n");
					break;
				}
				outputFile = fopen("decompressedOut.txt","w");
				if(outputFile == NULL){
					printf("Unable to open the output file\n");
					fclose(inputFile);
					break;
				}
				decompressFile(inputFile, outputFile);
				fclose(inputFile);
				fclose(outputFile);
				printf("Successfully decompressed as decompressed_output.txt!!\n ");
			    break;
		default: break;
		}
	} while (option != 3);
}
