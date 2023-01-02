#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    char c;
    int freq;
    struct node *left;
    struct node *right;
};
typedef struct node Node;

struct minHeap {
    int size;
    Node** array;
};
typedef struct minHeap MinHeap;

Node **createNodes(int count){
    Node **arr = (Node **)calloc(count, sizeof(Node*));
    return arr;
}

Node *createNode(char character){
    Node *node = (Node *)malloc(sizeof(Node));
    node->c = character;
    node->freq = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

int makeNodes(Node **arr, char *name) {
    printf("%s",name);
    FILE *fp;
    fp=fopen(name, "r");
    int ch, count = 0;
    while (1) {
        ch = fgetc(fp);
        if (feof(fp))
            break;
        if (arr[ch] == NULL) {
            arr[ch] = createNode((char)ch);
            count++;
        } else {
            arr[ch]->freq++;
        }
    }
    fclose(fp);
    return count;
}

void printFrequency(Node **arr) {
    printf("\nPrinting Frequency of each character\n");
    for (int i=0; i<256; i++) {
        if(arr[i] != NULL)
            printf("%c %d\n", arr[i]->c, arr[i]->freq);
    }
}

void copyNodesToHeap(Node **arr, MinHeap *minHeap) {
    int size = 0;
    for (int i=0; i<256; i++) {
        if(arr[i] != NULL)
            minHeap->array[size++] = arr[i];
    }
}

void swap(Node **a, Node **b) {
	Node *temp = *a;
	*a = *b;
	*b = temp;
}

void heapify(MinHeap *minHeap, int i) {
	int largest = i, left = 2*i + 1, right = left+1;

	if(left < minHeap->size && minHeap->array[left]->freq < minHeap->array[largest]->freq)
		largest = left;
	if(right < minHeap->size && minHeap->array[right]->freq < minHeap->array[largest]->freq)
		largest = right;
	if(largest != i) {
		swap(&minHeap->array[i], &minHeap->array[largest]);
		heapify(minHeap, largest);
	}
}

MinHeap *createMinHeap(Node **arr, int uniqueCharacters) {
    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    minHeap->array = createNodes(uniqueCharacters);
    minHeap->size = uniqueCharacters;
    copyNodesToHeap(arr, minHeap);

    for(int i = uniqueCharacters/2-1; i>=0; i--)
 		heapify(minHeap, i);

    return minHeap;
}

Node *extractMin(MinHeap *minHeap) {
    Node* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    heapify(minHeap, 0);
    return temp;
}

int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

void insertNode(MinHeap* minHeap, Node *node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    minHeap->array[i] = node;
}

Node *createHuffmanTree(MinHeap *minHeap) {
    while (!isSizeOne(minHeap)) {
        Node *smallest = extractMin(minHeap);
        Node *secondSmallest = extractMin(minHeap);
        Node *internalNode = createNode('-');
        internalNode->freq = smallest->freq + secondSmallest->freq;
        internalNode->left = smallest;
        internalNode->right = secondSmallest;
        insertNode(minHeap, internalNode);
    }
    return extractMin(minHeap);
}

int isLeaf(Node *node) {
    return node->left == NULL && node->right == NULL;
}

void storeAndPrintMeta(FILE *fp, char **charArr, int arr[], int n, char c) {
    char *prefix = calloc(3, sizeof(char)), *text = (char*)calloc(n, sizeof(char));
    prefix[0] = c;
    prefix[1] = '-';
    prefix[2] = '>';

    char* character_with_encoding;
    int len = n+3;
    printf("%c -> ", c);
    character_with_encoding = malloc(len*sizeof(char));
    strcpy(character_with_encoding, prefix);
    for (int i = 0; i < n; ++i){
        text[i] = arr[i]+'0';
        if (i < n-1)
            printf("%d ", arr[i]);
        else
            printf("%d\n", arr[i]);
    }
    strcat(character_with_encoding, text);
    charArr[(int)c] = text;
    fputs(character_with_encoding, fp);
    fputs("\n", fp);
    free(character_with_encoding);
}

void writeAndPrintMeta(FILE *fp, char **charArr, Node *root, int arr[], int index) {
    if (root->left) {
        arr[index] = 0;
        writeAndPrintMeta(fp, charArr, root->left, arr, index + 1);
    }
    if (root->right) {
        arr[index] = 1;
        writeAndPrintMeta(fp, charArr, root->right, arr, index + 1);
    }
    if (isLeaf(root)) {
        storeAndPrintMeta(fp, charArr, arr, index, root->c);
    }
}

void encodeFile(FILE *outputFile, char* inputFileName, char **charArr) {
    FILE *inputFile;
    inputFile=fopen (inputFileName, "r");
    int ch;
    while (1) {
        ch = fgetc(inputFile);
        if (feof(inputFile))
            break;
        fputs(charArr[ch], outputFile);
    }
    fputs("\n", outputFile);
    fclose(inputFile);
}


void performEncoding(char *inputFileName, char *outFileName, Node *root) {
    int valEncoding[256];
    char **charArr = (char **)calloc(256, sizeof(char*));
    FILE *outputFile;
    outputFile=fopen(outFileName, "w");
    writeAndPrintMeta(outputFile, charArr, root, valEncoding, 0);
    encodeFile(outputFile, inputFileName, charArr);
    fclose(outputFile);
}

void encode(char *inputFileName, char *outputFileName) {
    Node **arr = createNodes(256);
    int n = makeNodes(arr, inputFileName);
    printFrequency(arr);
    MinHeap *minHeap = createMinHeap(arr, n);
    Node *root = createHuffmanTree(minHeap);
    performEncoding(inputFileName, outputFileName, root);
}






struct decodeTreeNode {
    char c;
    struct decodeTreeNode *left;
    struct decodeTreeNode *right;
};
typedef struct decodeTreeNode DecodeNode;

DecodeNode *getDecodeNode() {
    DecodeNode *node = (DecodeNode*) malloc(sizeof(DecodeNode));
    node->left = node->right = NULL;
    return node;
}

void constructTree(DecodeNode *root, char *encoding, char ch) {
    int index = 0;
    while(encoding[index] != '\n') {
        if(encoding[index++] == '0') {
            if (root->left == NULL)
                root->left = getDecodeNode();
            root = root->left;
        } else {
            if (root->right == NULL)
                root->right = getDecodeNode();
            root = root->right;
        }
    }
    root->c = ch;
}

void decodeAndcreateOutFile(FILE *outputFile, DecodeNode *root, char *encoding) {
    int index = 0;
    DecodeNode *temp = root;
    while (encoding[index] != '\n') {
        if(encoding[index++] == '0') {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
        if (temp->left == NULL && temp->right == NULL) {
            fputs(&temp->c, outputFile);
            temp = root;
        }
    }
}
void readEncodedFile(char *inputFileName, char *outputFileName, DecodeNode *root) {
    FILE *inputFile;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL)
        exit(EXIT_FAILURE);
    
    while ((read = getline(&line, &len, inputFile)) != -1) {
        if (read <= 1 || line[1] != '-')
            break;
        constructTree(root, &line[3], line[0]);
    }
    FILE *outputFile;
    outputFile=fopen(outputFileName, "w");
    decodeAndcreateOutFile(outputFile, root, line);
    fclose(inputFile);
    fclose(outputFile);
    if (line)
        free(line);
}

void decode(char *inputFileName, char *outputFileName) {
    DecodeNode *root = (DecodeNode*) malloc(sizeof(DecodeNode));
    readEncodedFile(inputFileName, outputFileName, root);
}

int main() {
    char inputFileName[100], outputFileName[100];
    int mode = -1;
    printf("Enter the name of the input file\n");
    scanf("%s", inputFileName);
    printf("Enter the name of the output file\n");
    scanf("%s", outputFileName);
    printf("Enter mode (0 for compression, 1 for decompression)\n");
    scanf("%d", &mode);
    
    if(mode == 0) {
        encode(inputFileName, outputFileName);
    } else if (mode == 1) {
        decode(inputFileName, outputFileName);   
    } else {
        printf("Invalid mode value\n");
    }
	return 0;
}