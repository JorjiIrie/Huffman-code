#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>

// Defining Paddings
#define LP 5 //left part (for output designing)
#define TP 3 //top part (for output designing)
#define RP 100 //right part (for output designing)

// struct for HuffManTree
typedef struct node{
    unsigned char ch;
    int freq;
    int leafNode;
    struct node *left;
    struct node *right;
}huffNode;

// struct for priority Queue
typedef struct queue{
    huffNode *pqNode;
    struct queue *next;
}priorityQueue;

priorityQueue *front;


// Global variables
int charFreq[256], code[256];
int charCount, size = 0, ind = -1, notFound = 1, bitPos = 0, asciiBitCount, bitShift = 7, asciiBitRem, bitSpace = 8, fNameLen;
int shiftLeft = 0, totalCharFrequency, scanned = 0, readBinary;
char charCode[256], tempry[50],nameOfFile[50],fileName[50], option = '\0';
float initialsize, finalsize;
unsigned char bitStorage = '\0', asciiBitStorage = '\0',  tempBitStorage = '\0', temp;;
long int compressedFileLen;

// Function declaration
void headerBar(char *title, char *institution, char *section); // function for navigation bar
void borderContent(void); 	 // function for content borders
void makeNull();          	 // function to set front pointer to NULL
void initializeArray();   	 // function to fill array with characters' ascii
void enqueue(priorityQueue* newNode); // function to enqueue node in priority queue
void dequeue(priorityQueue* node);    // function to dequeue node in priority queue
void buildTree (priorityQueue* l, priorityQueue* r); // function to create tree
void encode (FILE* fp_in, huffNode* node);   // function for encoding tree
void traverse(huffNode* node);  						 // function for traversing tree
void saveTree(huffNode* node, FILE* tree);   // function for saving tree
void look(huffNode* node, unsigned char c);  // function to check for leaf nodes
void rebuildTree(huffNode*  node, FILE* fp); // function for recreating tree (decompression)
void deleteFiles();       // function to delete files from compression
void gotoxy(int x,int y); // gotoxy function
void SetColor(int ForgC);  // function to change foreground color
void bye();               // function to display outro message
int menu();               // function for menu


priorityQueue *createNode(int i, int j, huffNode *l, huffNode *r){           // function to create new node
    priorityQueue *newNode = (priorityQueue*)malloc(sizeof(priorityQueue));  // allocate memory address for new node
    newNode -> pqNode = (huffNode*)malloc(sizeof(huffNode));                 // allocate memory address for huffNode
    newNode -> pqNode -> left = l;   // left child set to l; NULL/pqNode
    newNode -> pqNode -> right = r;  // right child set to r; NULL/pqnode
    newNode -> next = NULL;          // next node set to NULL
    if(j){                           // for characters with frequency
        newNode -> pqNode -> ch = i; // set ch to i (character's ascii)
        newNode -> pqNode -> freq = charFreq[i]; // set freq of respective characters
    }
    else{ // for parent nodes
        newNode -> pqNode -> ch = '\0'; // character would be null
        newNode -> pqNode -> freq = i;  // set frequency to i (0)
    }
    return newNode; //return new node containing the character and its frequency
}

int main(){
  while(1){ // while true / loops until break is encountered
   switch(menu()){ // switch case for menu
    case 1: // for file compression
        initializeArray(); // fill the array with characters' ascii
        makeNull(); // call function to set front pointer to null
        system("cls");  // clears window console
        SetColor(6);
        headerBar(" COMPRESS FILE ", "TECHNOLOGICAL UNIVERSITY OF THE PHILIPPINES", "JAJEMIJOJHARI Final Project"); // for navigation bar
        SetColor(11);
        borderContent(); // for border
        char *subtitle = malloc(sizeof(char) * 50); // initialization
        subtitle = " File Compression and Decompression"; // words to be printed
        gotoxy((((LP + RP) / 2) - (strlen(subtitle) / 2)), 26); //for output designing
        printf("%s", subtitle); // printing


        char nameOfFile[50]; //variable declaration
        priorityQueue *newNode; //set newNode as a pointer of priorityQueue
        huffNode *huffManTree;  //set huffManTree as a pointer of huffnode
        FILE *fp, *fp_out, *fp_freq; // set file pointers
        size=0;

        gotoxy(LP + 4, TP + 8); //for output designing
        printf("Enter Text file(.txt): "); //prompts user to enter text file
        SetColor(6);
        scanf(" %s",nameOfFile);  // get the file name
        fp = fopen(nameOfFile,"r"); //open file in read mode
        if(fp == NULL){  //if file is null
           gotoxy(LP + 34, TP + 25); //for output designing
             SetColor(12);
            printf("<ERROR: File can't be found!>"); printf("\n\n"); //print error message
            exit(0); //exits program
        }else{ //if the file is found
            system("cls");  //clear windows console
            headerBar("  Contents of the File  ", "TECHNOLOGICAL UNIVERSITY OF THE PHILIPPINES", "JAJEMIJOJHARI Final Project"); // for navigation bar
            gotoxy(LP + 3, TP + 6);    //for output designing
            while(!feof(fp)){   // while end of file is not encountered--
                charCount = fgetc(fp);  // get every character
                charFreq[charCount]++;  // increment the frequency of the character
                SetColor(15);
                putchar(charCount);     // outputs the characters
                size++;    // increment the size (refers to bytesize of file)
            }
            initialsize=size+1;  // set the initial size to the size of text after reading
        }
        fclose(fp);    // close the file
        if(size > 1){  // if size of the file is more than 1 byte
            for(int i = 0; i < 256; i++){  // loop for every character
                if(charFreq[i] > 0){  // if the character repeats (frequency more than 0)
                    newNode = createNode(i,1,NULL,NULL); //  create a node for that character---
                    enqueue(newNode);  // and enqueue in a priority queue
                }
            }

            buildTree(front, front -> next); // create the tree passing the first two nodes as parameters
            huffManTree = front -> pqNode;  // set the pqNode as the huffman tree
            free(front); // deallocate front pointer
            SetColor(11);
            printf("\n\n\t\t\t\tChar\t\t Freq\t\t Code"); //outputs text
            traverse(huffManTree); //call function to traverse tree
        }
        else{ // if string length of text in file is less than 1
              gotoxy(LP + 4, TP + 25); //for output designing
            printf("string length must be greater than 1."); // outputs text
            exit(0); // exit program
        }


        fNameLen = strlen(nameOfFile); // get the string length of nameOfFile and store it fNameLen
        fNameLen -= 4; // decrement stringsize to 4 to remove [.text]
        strncpy(tempry, nameOfFile,fNameLen); //string copy nameOfFIleto tempry with size of FnameLen
        fp = fopen(nameOfFile,"r"); //open name of file
        encode(fp, huffManTree); // call function to encode node tree in file
        fclose(fp); // close file

        memset(tempry, '\0', sizeof(tempry)); // fill size of tempry starting from tempry up to null
        strncpy(tempry, nameOfFile,fNameLen); // string copy nameOfFile to tempry with size of fNameLen
        strcat(tempry,".tree"); // join two strings

        fp = fopen(tempry,"wb"); // open file in write binary mode
        bitStorage = '\0'; // set the bitsatorage to null
        saveTree(huffManTree,fp); // call function to save huffman tree
        fputc(bitStorage,fp); // writes bitStorage to file
        fclose(fp); // close file

        memset(tempry, '\0', sizeof(tempry)); // set memory of tempry
        strncpy(tempry, nameOfFile,fNameLen); // string copy nameOfFile to tempry with size of fNameLen
        strcat(tempry,".freq");  // join two strings

        fp_freq = fopen(tempry,"w"); // open file in write mode
        fprintf(fp,"%d",huffManTree -> freq); //prints the frequencies of huffman in file
        fclose(fp_freq); // close file
        getch(); // wait for a key press
        break;  // terminate case 1

    case 2: // for file decompression
        system("cls");		//clears screen
        huffNode *DhuffManTree = (huffNode*)malloc(sizeof(huffNode));		//creates new node for huffman tree
        huffNode *DnewNode; //pointer variable declaration
        system("cls"); //clears screen
        headerBar(" DECOMPRESS FILE ", "TECHNOLOGICAL UNIVERSITY OF THE PHILIPPINES", "JAJEMIJOJHARI Final Project"); // for navigation bar
        gotoxy(LP + 4, TP + 7); //for output designing
        printf("Enter compressed file(.compressed): "); //displays output
        SetColor(15);
        scanf(" %s",fileName);		//scans for input
        fNameLen = strlen(fileName); //counts the string length of filename
        fNameLen -= 11; 		//deducts the .compressed from file name
        strncpy(tempry, fileName, fNameLen);		//copies string from 1 variable to another
        strcat(tempry,".tree");		// joins the 2 different strings
        FILE *fp_compressed; 			//declaration of a file
        fp = fopen(tempry,"rb"); //opens text file
        if(fp == NULL){			//if file is empty
            SetColor(12);
            printf("\n\n\t\t\t\t<ERROR: The (.)tree file is not found.>\n\n"); //displays output
            exit(0); //exits
        }
        else{
            memset(tempry, '\0', sizeof(tempry)); //sets memory
            strncpy(tempry, fileName, fNameLen); 		//copies strings
            strcat(tempry,".freq");		//joins 2 different strings
            fp_freq = fopen(tempry,"r");	//opens file
            if(fp_freq == NULL){		//if file is empty
                gotoxy(LP + 4, TP + 25);		//for output placing
                printf("<ERROR: The (.)freq file is not found.>\n\n");	//displays output
                exit(0); //exits program
            }
            else
                fscanf(fp_freq,"%d",&totalCharFrequency); //scans file

            fclose(fp_freq);	//closes file
            readBinary = fgetc(fp);	//gets all the char in file fp
            bitStorage = readBinary;	//transfers memory
            rebuildTree(DhuffManTree, fp); //rebuilds the tree
        }
        fclose(fp);  //closes file

        fp_compressed = fopen(fileName,"rb");	//opens file
        memset(tempry, '\0', sizeof(tempry));	//sets memory
        fNameLen = strlen(fileName);		//counts the length of file name
        fNameLen -= 11;		//negates the .compressed from the file name
        strncpy(tempry, fileName,fNameLen);	//copies strings
        strcat(tempry,".txt");	//joins strings

        fp_out = fopen(tempry,"w");	//opens file


        if(fp_compressed == NULL){ 	//if file is empty
            gotoxy(LP + 4, TP + 25);	//for output placing
            SetColor(12);
            printf("<ERROR: The (.)compressed file is not found.>\n\n"); 	//displays output
            exit(0);		//exits
        }
        else{
            SetColor(11);
            for(int i = LP; i < 100; i++){
                gotoxy(i, 12); //for output designing
                printf("%c", 196);  // printing bottom
            }
            gotoxy(LP + 41, TP + 9); //for output designing
            SetColor(6);
            printf(" DECODED TEXT "); //displays output
            SetColor(15);
            printf("\n\n\t"); //prints new line
            DnewNode = DhuffManTree;
            while(!feof(fp_compressed)){
                readBinary = fgetc(fp_compressed);	//gets all characters
                bitStorage = readBinary;	//saves to bitStorage
                shiftLeft = 0;	//for tree traversal

                while(shiftLeft < 8){
                    temp = bitStorage; //saves to temp
                    temp <<= shiftLeft;
                    temp >>= 7;
                    shiftLeft++;

                    if(temp == 0)
                        DnewNode = DnewNode -> left; //Traverse to left

                    else if(temp == 1)
                        DnewNode = DnewNode -> right;	//traverse to right

                    if(DnewNode -> leafNode == 1){ 	//if  leaf node is right
                        putchar(DnewNode ->ch); //replaces char
                        fputc(DnewNode -> ch, fp_out); //replaces char inside of file
                        DnewNode = DhuffManTree;
                        scanned++; //increments scanned
                    }

                    if(scanned == totalCharFrequency)	//if scanned is equal to the total frequency of character
                        break;	//breaks from loop
                }
                if(scanned == totalCharFrequency) //if scanned is equal to the total frequency of character
                    break;	//breaks from loop
            }
            printf("\n\n\t");  //prints new line
            system("pause"); //waits for any input

        }
        fclose(fp_compressed);  //closes file
        fclose(fp_out);		//closes file
        SetColor(11);
        printf("\n\n\tDecoded message is saved in %s",tempry);
        printf("\n\tDo you want to delete %s? [Y/N]: ",fileName);
        while(option != 'y' && option != 'Y' && option != 'n' && option != 'N'){
            option = getchar();
            if(option == 'y' || option == 'Y'){
                deleteFiles();
                break;
            }
        }
        printf("\n\n\t\t\tEXITING THE PROGRAM. . .\n\n"); //displays output
        bye();	//call bye function
        free(huffManTree); //deletes the tree
        break;	//breaks from loop
        }
    }
}

int menu(){ //main menu UI
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    int mainMenuChoice, y=TP+9, choice=1; //int variable for user input
    char ch; //char variable

    system("cls");
    system("COLOR 0B");        // change the background color into black and font color into cyan
    SetColor(6);
    headerBar("  M A I N   M E N U  ", "TECHNOLOGICAL UNIVERSITY OF THE PHILIPPINES", "JAJEMIJOJHARI Final Project"); // for navigation bar
    SetColor(11);
    borderContent(); // for content borders

    char *subtitle = malloc(sizeof(char) * 50); // initialization
    subtitle = " File Compression and Decompression "; // words to be printed
    gotoxy((((LP + RP) / 2) - (strlen(subtitle) / 2)), 26); //for output designing
    printf("%s", subtitle); // printing

    gotoxy (LP + 36, TP + 9);  // set initial location for cursor
    SetColor(15);
    printf("\x10");            //print the pointer for menu options

    SetColor(3);
    gotoxy(LP + 41, TP + 9); //for output designing
    printf("COMPRESS FILE"); // printing
    SetColor(11);
    gotoxy(LP + 40, TP + 12); //for output designing
    printf("DECOMPRESS FILE"); // printing

    while(1){


        gotoxy(LP + 20, TP + 25); //for output designing
        SetColor(6);
        printf("Use arrow keys (\x18 \x19) to navigate. Press ENTER to select."); //print characters to location
        printf("\e[?25l"); //hide cursor

        ch=_getch(); //get character

        if(ch==80 && y != TP + 12) {                //if down button pressed and not equal to y location of last option
            SetConsoleTextAttribute(console, 11);   //set back to aqua
            gotoxy(LP + 41, TP + 9);                //for output designing
            printf("COMPRESS FILE");                // printing
            gotoxy(LP + 35,y); printf("    ");      //clear previous pointer
            y=y+3;                                  //increment y location to move down the arrow
            gotoxy(LP + 35,y); printf("\x10");      //print the arrow
            gotoxy(LP + 41, TP + 9);                //for output designing
            SetConsoleTextAttribute(console, 3);    //setting color to cyan
            gotoxy(LP + 40, TP + 12);               //for output designing
            printf("DECOMPRESS FILE");              // printing
            SetConsoleTextAttribute(console, 11);   //set back to aqua
            choice++;                               //increment the choice
            continue;
        } else if(ch==72 && y != TP + 9) {          //if up button pressed and not equal to y location of the first option
            SetConsoleTextAttribute(console, 11);   //set back to aqua
            gotoxy(LP + 40, TP + 12);               //for output designing
            printf("DECOMPRESS FILE");              // printing
            gotoxy(LP + 35,y); printf("    ");      //clear previous pointer
            y=y-3;                                  //increment to move down the arrow
            gotoxy(LP + 35,y); printf("\x10");      //print the arrow
            SetConsoleTextAttribute(console, 3);    //setting color to cyan
            gotoxy(LP + 41, TP + 9);                //for output designing
            printf("COMPRESS FILE");                // printing
            SetConsoleTextAttribute(console, 11);   //set back to aqua
            choice--;                               //decrement the choice
            continue;                               //continue
        } else if(ch==13) {                         //Enter key pressed
            printf("\e[?25h");                      //put back the cursor
            system("COLOR 0B");                     //set console to aqua
            return choice;                          //return main menu choice (1 or 2)
        }
    }
}

void makeNull(){    // make null function
    front = NULL;   // set front pointer to null
}

void enqueue(priorityQueue* newNode){ // enqueue function for priority queue
    priorityQueue *p, *q;  // set pointers for priorityQueue
    p = q = front; // set pointers equal to front [null]

    if(front == NULL){ // if front NULL / starting node
        newNode -> next = front; // set next node as the front node
        front = newNode; // set the memory address of new node to the front node
    }
    else{ //for the top priority (front node)
        if(newNode -> pqNode -> freq < front -> pqNode -> freq ){ // if the freq of n is less than that of the  node
            newNode -> next = front; // set next as the head node
            front = newNode; // set the memory address of new node (n) to the front node
        }
        else{ //sorting in queue
            while(p != NULL){ // traverse the list
                q = p;          // set q to p to copy address of previous link
                p = p -> next;  // set p to next to proceed to next link
                if(p != NULL){  // traverse the queue
                    if(newNode -> pqNode -> freq < p -> pqNode -> freq){ // if the frequency of new node is less than that of p (place it as a priority)
                        newNode -> next = p; // set next of new node to p
                        q -> next = newNode; // set next of q to new node
                        break; // terminate loop
                    }
                }
                else{   // else--
                    q -> next = newNode; //if not a priority--
                    newNode -> next = p; //link new node to previous node
                }
            }
        }
    }
}

void dequeue(priorityQueue* node){ // function to dequeue nodes
    if(node -> next != NULL) // if next node (node adjacent to front node)  is not nul
        free(node -> next); //  dequeue node
    free(node); // dequeue (front node)
}

void buildTree(priorityQueue* l, priorityQueue* r){  // function to create a tree
    priorityQueue *newNode, *temp = front;  //set the pointers equal to front
    newNode = createNode((l -> pqNode -> freq) + (r -> pqNode -> freq), 0, l -> pqNode, r -> pqNode); // create parent node for the leaf nodes (right and left)
    if(r -> next != NULL)   // r - > next is not the last queue
        front = r -> next;  // set the front pointer to the next queue
    else
        front = newNode;   // if the next node is NULL then, set front node as the new node
    dequeue(temp);         // call dequeue function to remove the first two node on queue
    enqueue(newNode);      // enqueue back the created tree
    if(front -> next != NULL) //   repeat process until the last node
        buildTree(front, front -> next); // call funtion
}

void traverse(huffNode* node){
    if (node -> left == NULL && node -> right == NULL){    //if the right and left child are null--
        SetColor(15);
        printf("\n\t\t\t\t %c\t\t  %d\t\t ",node->ch, node -> freq);   //print the character and its frequency
        SetColor(6);
        for(int i = 0; i <= ind; i++) // loop until i exceeds the value of index
            printf("%d",code[i]);  // print the code
        return;
    }
    ind++; //increment index
    code[ind] = 0; // set index value equal to 0 when traversing left
    traverse(node->left);
    code[ind] = 1; // set index value to 1 when traversing right
    traverse(node->right);
    ind--; // decrement index
}

void initializeArray(){ // function to populate array
    for(int i = 0; i < 256; i++)  // loop until i < 256
        charFreq[i] = 0; // populate array
}

void look(huffNode* node, unsigned char c){
    if (node -> left == NULL && node -> right == NULL ||node -> ch == c){ // if left and right node is null and ch equals c
        if(node -> ch == c) // nested if to check if ch ==c
            notFound = 0;  // set notFound to 0
        return; // go back to encode function
    }
    ind++; // increment index
    charCode[ind] = 0; // set the value of charCode in ind to 0
    look(node->left, c); // check for left node
    if(notFound){  // if not found
        charCode[ind] = 1; // set the value of charCode in ind to 1
        look(node->right, c); // check for right node
    }
    if(notFound) // if it is true for notFound (notFound==1)
        ind--; // decrement index
}


void encode(FILE* fp_in, huffNode* node){ // function to encode
    FILE* encodedFile; // set variable to file
    strcat(tempry,".compressed"); // combine two strings
    encodedFile = fopen(tempry,"wb"); // open file in write binary mode
    while(!feof(fp_in)){ // while end of file is not encountered
        ind = -1; // set index to -1
        notFound = 1; // set notFound to 1
        charCount = fgetc(fp_in); // get every characters in file and set is to charCount
        look(node, charCount); // call look function
        for(int i = 0; i <= ind; i++){ // loop i until it exceeds or equals the index
            bitPos++; // increment bitPoss
            bitStorage = bitStorage | charCode[i]; // set the bit storage
            if(bitPos < 8) // if bit position is less than 8
                bitStorage = bitStorage << 1; //shift 1 bit storage to the left
            if(bitPos == 8){ // if bit position equals 8
                fputc(bitStorage, encodedFile); // prints the characters from encoded file to bit storage
                bitPos = 0; // set bit position to 0
                bitStorage = '\0'; // null terminate bit storage

            }
        }
    }

    while(bitPos != 7){ // while bit position is not equal to 7--
        bitStorage = bitStorage << 1; // shift 1bit storage to the left
        bitPos++; // increment bit position
    }
    fputc(bitStorage, encodedFile); // prints the characters from encoded file to bit storage
    finalsize = compressedFileLen = ftell(encodedFile); // get the total size of file
    SetColor(11);
    printf("\n\n\n\t\t\t    Initial size of %s in  bytes: %.0f",tempry,initialsize); // prints initial size of file
    printf("\n\t\t\t     File size in bytes after compression: %ld",compressedFileLen); // prints compressed file size
    printf("\n\t\t\t\t     Compression Ratio: %.2f%%",((finalsize/initialsize)*100)); // prints compression ratio
    fclose(encodedFile); // close the file
}

void saveTree(huffNode *node, FILE *tree){
    unsigned char temp; //declaration of unsigned char variable
    if(node -> left == NULL && node -> right == NULL){	//if left and right is null
        temp = 1;		//sets temp to 1
        temp <<= bitShift;	//shifts the bits
        bitStorage |= temp;	//bitwise operator
        bitSpace--;		//decrements
        asciiBitCount = 8 - bitShift;	//sets value for asciibitcount
        asciiBitRem = asciiBitCount;	//sets value for asciibitrem
        if(bitSpace > 0){ //if bitspace is greater than 0
            temp = node ->ch;	//sets the value of temp to node->ch
            temp >>= asciiBitCount; //bitwise operation
            bitStorage |= temp;	//bitwise operation
        }
        fputc(bitStorage,tree); //replace char in file
        bitStorage = '\0';	//nulls bit storage
        bitShift = 7;	//bitwise operation
        bitSpace = 8;	//bitwise operation
        if(asciiBitRem > 0){	//if asciibitrem is greater than 0
            temp = node -> ch;	//sets value of temp to node->left
            temp <<= 8 - asciiBitRem;	//bitwise operation
            bitStorage |= temp;	//bitwise operation
            bitSpace = 8 - asciiBitRem; //sets value of bitspace
            bitShift = bitSpace - 1;	//sets value of bitshift
            if(asciiBitRem == 8){	//if ascii bit rem = 8
                fputc(bitStorage,tree);	//replaces char in file
                bitStorage = '\0'; //nulls bitstorage
                bitShift = 7;	//sets value of bitshift
                bitSpace = 8;	//sets value of bitspace
            }
        }
        asciiBitCount = 8;	//sets value of asciibitcount
        asciiBitRem = 8;	//sets value of asciibitrem
        return;	//returns
    }
    if(node -> left -> ch == '\0'){ //if node char in node left is null
        temp = 0; //sets value of temp
        temp <<= bitShift;		//bitwise operation
        bitStorage |= temp;		//bitwise operation
        bitShift--;		//decrements bitshift
        bitSpace--;		//decrements bitspace
        if(bitSpace == 0){
            fputc(bitStorage,tree);	//replaces char in file
            bitStorage = '\0';	//sets value of bitstorage
            bitShift = 7;	//sets value of bitshift
            bitSpace = 8;	//sets value of bitspace
        }
    }
    saveTree(node -> left,tree); //calls function

    if(node -> right -> ch == '\0'){ //if right is null
        temp = 0; // set temp to 0
        temp <<= bitShift; // temp is left shifted by value of bitshift
        bitStorage |= temp; // set bit storage equal to bit Storage or temp
        bitShift--; // decrement bitShift
        bitSpace--; // decrement bitSpace
        if(bitSpace == 0){ //of bitSpace equals 0
            fputc(bitStorage,tree); // put every characters in tree to bitStorage
            bitStorage = '\0'; // null terminate bitStorage
            bitShift = 7; // set bitShift to 7
            bitSpace = 8; // set bitShift to 8
        }
    }
    saveTree(node -> right,tree); // callsavetree function passing the right leaf and tree as parameters
}



void deleteFiles(){ // function to delete files
    int del = remove(fileName);  // remove filename
    memset(tempry, '\0', sizeof(tempry)); // set memory of tempry
    fNameLen = strlen(fileName); // set the fNameeLen to the stringlenth of fileName
    fNameLen -= 11; // decrement the file name length by 11
    strncpy(tempry, fileName,fNameLen); // string copy file name to tempry with thr size of fNameLen
    strcat(tempry,".tree"); //combine two strings
    del = remove(tempry); // remove tempry
    memset(tempry, '\0', sizeof(tempry)); //sets memory of tempry
    strncpy(tempry, fileName,fNameLen);	//copies strings
    strcat(tempry,".freq");	//joins the two strings
    del = remove(tempry);	//deletes tempry
    printf("Files successfully deleted.\n");	//displays output
}

void rebuildTree(huffNode *node, FILE* fp){ // function to rebuild tree
    huffNode *newNode; // set pointer to huffnode
    if(temp == 1){ // if temp equals 1
        if(shiftLeft < 8){ // nested if to check if shiftleft is less than 8
            asciiBitStorage |= bitStorage << shiftLeft; // bitwise operation
            asciiBitRem = shiftLeft; // set shiftLeft to asciiBitRem
        }
        else{
            readBinary = fgetc(fp);	//gets char from file and saves to read binary
            asciiBitStorage = readBinary;	//sets value of asciibitstorage to readbinary
            asciiBitRem = 0;	//sets value of asciibitrem
        }

        if(asciiBitRem > 0){ // if asciibitrem is greater than 0
            readBinary = fgetc(fp); //gets char from file
            bitStorage = readBinary;	//sets value of bitstorage to readbinary
            tempBitStorage = bitStorage;	//sets value of tempbitstorage to bitstorage
            tempBitStorage >>= 8-shiftLeft; //bitwise operation
            asciiBitStorage |=  tempBitStorage;	//bitwise operation
        }
        node -> ch = asciiBitStorage; // set the asciiBitStorage to ch node
        node -> leafNode = 1; //set leaf node to 1
        asciiBitStorage = '\0'; // null terminate asciiBitStorage
        return; //return to function
    }


    if(shiftLeft < 8) // check if shiftLeft is less than 8
        temp = bitStorage;  //sets value of temp to bitstorage
    else{
        readBinary = fgetc(fp); //reads file and saves to readbinary
        bitStorage = readBinary;	//sets value of bitstorage to readbionary
        temp = bitStorage; //sets value of temp to bitstorage
        shiftLeft = 0; //sets value of shift left to 0
    }
    temp <<= shiftLeft; //shift left temp by the value of shiftLeft
    temp >>= 7; // shift right temp by 7
    shiftLeft++; // increment the value of shiftLeft

    newNode = (huffNode*)malloc(sizeof(huffNode)); //creates new node
    newNode -> ch = '\0'; //nulls new node
    newNode -> leafNode = 0; //sets value of leaf node to 0
    node -> left = newNode; //sets value of left node to newnode
    rebuildTree(node -> left, fp);	//calls rebuild tree function

    if(shiftLeft < 8)  // if shiftLeft is less than 8
        temp = bitStorage; // set temp to bitStorage
    else{  // if the value is more than 8
        readBinary = fgetc(fp); // set the content of file to readBinary
        bitStorage = readBinary; // sets the bitStorage value to readBinary
        temp = bitStorage; // set the temp value to bitStorage
        shiftLeft = 0; // set shiftLeft to 0
    }
    temp <<= shiftLeft; // shift left temp by the value of shiftLeft
    temp >>= 7; // shift right temp by 7
    shiftLeft++; // increment the value of shiftLeft

    newNode = (huffNode*)malloc(sizeof(huffNode)); //creates new node
    newNode -> ch = '\0';	//nulls newnode ->ch
    newNode -> leafNode = 0; //sets value of newnode-> leafnode to 0
    node -> right = newNode; //sets a newnode at node ->right
    rebuildTree(node -> right, fp); //calls rebuild tree function

}

void headerBar(char *title, char *institution, char *section){
system("COLOR 0B");        // change the background color into black and font color into cyan
    // Left and Right Borders
    for(int i = TP; i < 7; i++){
        gotoxy(LP, i); //for output designing
        printf("%c", 186); // printing left par
    }

    for(int i = TP; i < 7; i++){
        gotoxy(RP, i); //for output designing
        printf("%c", 186); // printing right part
    }

    // Top and Bottom Borders
    for(int i = LP; i < 100; i++){
        gotoxy(i, 2); //for output designing
        printf("%c", 205); // printing left borders
    }

    for(int i = LP; i < 100; i++){
        gotoxy(i, 7); //for output designing
        printf("%c", 196);  // printing bottom
    }

    // Corners
    gotoxy(5, 2); //for output designing
    printf("%c", 201); // printing
    gotoxy(100, 2); //for output designing
    printf("%c", 187); // printing
    gotoxy(5, 7); //for output designing
    printf("%c", 211); // printing
    gotoxy(100, 7); //for output designing
    printf("%c", 189); // printing


    // Texts
    gotoxy((((LP + RP) / 2) - (strlen(institution) / 2)), TP + 1); //for output designing
    printf("%s", institution); // printing the given institution
    gotoxy((((LP + RP) / 2) - (strlen(section) / 2)), TP + 2); //for output designing
    printf("%s", section); // printing name of the author
    gotoxy((((LP + RP) / 2) - (strlen(title) / 2)), 7); //for output designing
    for(int i = 0; i < strlen(title); i++){
    SetColor(6);;
        for(int j = 0; j < 10000000; j++); // looping for the effects in title
        SetColor(6);
        printf("%c", title[i]);//219
    }
}

void borderContent(void){

     // Left and Right Borders
    for(int i = TP + 5; i < 30; i++){
        gotoxy(LP, i); //for output designing
        printf("%c", 186); // printing left borders
    }

    for(int i = TP + 5; i < 30; i++){
        gotoxy(RP, i); //for output designing
        printf("%c", 186); // printing right borders
    }

    // Bottom Borders
    for(int i = LP; i < RP; i++){
        gotoxy(i, 26); //for output designing
        printf("%c", 196); // printing first bottom borders
    }

    for(int i = LP; i < RP; i++){
        gotoxy(i, 30); //for output designing
        printf("%c", 205); // for second bottom borders
    }

    // Corners
    gotoxy(LP, TP + 4); //for output designing
    printf("%c", 199); // printing
    gotoxy(RP, TP + 4); //for output designing
    printf("%c", 182); // printing
    gotoxy(LP, TP + 27); //for output designing
    printf("%c", 200); // printing
    gotoxy(RP, TP + 27); //for output designing
    printf("%c", 188); // printing
    gotoxy(LP, 26); //for output designing
    printf("%c", 199); // printing
    gotoxy(RP, 26); //for output designing
    printf("%c", 182); // printing

}

void bye(){
system("cls");
system("COLOR 0B");
    headerBar("  FILE COMPRESSION AND DECOMPRESSION  ", "TECHNOLOGICAL UNIVERSITY OF THE PHILIPPINES", "JAJEMIJOJHARI Final Project"); // for navigation bar
    SetColor(11);
    borderContent(); // for content borders
    char *subtitle = malloc(sizeof(char) * 50); // initialization
    subtitle = " File Compression and Decompression "; // words to be printed
    gotoxy((((LP + RP) / 2) - (strlen(subtitle) / 2)), 26); //for output designing
    printf("%s", subtitle); // printing
    gotoxy(LP + 33, TP + 8); //for output designing
    printf("SUBMITTED AND PROGRAMMED BY:"); // printing
    gotoxy(LP + 22, TP + 10); //for output designing
    SetColor(6);
    printf("JANAH MORANO        JEORGE DANS         MIKA GAJETO"); // printing
    gotoxy(LP + 21, TP + 12); //for output designing
    printf("JOHN DIMAGUILA      JON REXZEL VALLOYAS       RASHEED RIGA"); // printing
    gotoxy(LP + 39, TP + 16); //for output designing
    SetColor(14);
    printf(" BSCS 2AB-M (STEM)"); // printing
    gotoxy(LP + 39, TP + 17); //for output designing
    printf("0 2 . 1 1 . 2 0 2 2"); // printing
    gotoxy(LP + 30, TP + 25); //for output designing
    SetColor(6);
    printf("Thank you for using our program! <3\n\n"); //prompts for the choice of the user

}

void gotoxy(int x,int y){ // function to move the curson

    COORD coord; // coordinates of a character cell in a console screen buffer
    coord.X=x; // for indentation
    coord.Y=y; // for rows
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord); // This is part of the gotoxy function it looks it is setting the position of a cursor
}

void SetColor(int ForgC)
 {
 WORD wColor;

  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;

                       //We use csbi for the wAttributes word.
 if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
 {
                 //Mask out all but the background attribute, and add in the forgournd color
      wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
      SetConsoleTextAttribute(hStdOut, wColor);
 }
 return;
}
