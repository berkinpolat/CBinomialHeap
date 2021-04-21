// Github: @berkinpolat
// github.com/berkinpolat

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

////////////////////////// HEAP AND NODE STRUCTURES/////////////////////////////
//Structures for node and heap 
typedef struct node {
	int count, degree;
	char *str, *fileName;
	struct node *parent, *child, *sibling;
} node;

typedef struct heap {
	node *head;
} heap;

//int relevant counts how many nodes constructed with count value > 0
//int relevant will be increased by 2 for a value > 0 since there are 2 heaps (context and relevant)
//therefore int relevant will be divided by 2 when printing.
int relevant = 0;
/////////////////////// FUNCTIONS TO CREATE NEW NODES AND HEAPS////////////////////////////////

node *newNode(int count, char *str, char *fileName) {
	node *newNode = (node*)malloc(sizeof(node));
	newNode->count = count;
	newNode->str = malloc(strlen(str) + 1);
	strcpy(newNode->str, str);
	newNode->fileName = malloc(strlen(fileName) + 1);
	strcpy(newNode->fileName, fileName);
	newNode->degree = 0;
	newNode->child = NULL;
	newNode->parent = NULL;
	newNode->sibling = NULL;
	return newNode;
}

heap *newHeap() {
	heap *newHeap = (heap*)malloc(sizeof(heap));
	newHeap->head = NULL;
	return newHeap;
}

///////////////////////// INSERTION TO A HEAP //////////////////////////////

//recmerge function is a recursive function that merges equal degreed nodes in a heap in order to have a clean heap.
void recMerge(heap **heap, node *root, node *rootSibling) {
	//if root node's sibling is null, there is nothing to merge.
	//this is the exit statement for the recursive function.
	if (rootSibling == NULL)
		return;
	//if root node's and it's sibling's degrees are equal, then function will merge them.
	if (root->degree == rootSibling->degree) {
		/*
		-if counted words in the root node is greater than in the sibling,
		function will make sibling as the child of root node. Because this is a maximum heap(greater keys must be the parent root.)
		-root's child will be the sibling of the sibling node.
		*/
		if (root->count >= rootSibling->count) {
			//elements of these 2 nodes will be arranged according to their equalness in counted words.
			if (rootSibling->sibling != NULL)
				root->sibling = rootSibling->sibling;
			else
				root->sibling = NULL;
			
			if (root->child != NULL) {
				node *tempRoot = root->child;
				rootSibling->sibling = tempRoot;
			}
			else
				rootSibling->sibling = NULL;
			
			root->child = rootSibling;
			rootSibling->parent = root;
			root->degree++;
			
			(*heap)->head = root;
			
			//function will work again to check if there are more nodes to merge.
			return recMerge(heap, (*heap)->head, (*heap)->head->sibling);			
		}
		/*
		-if counted words in the root node is smaller than in the sibling,
		function will make sibling as the parent of root node. Because this is a maximum heap(greater keys must be the parent root.)
		-sibling's child will be the sibling of the root node.
		*/
		else {
			//elements of these 2 nodes will be arranged according to their equalness in counted words.
			if (rootSibling->child != NULL) {
				node *tempRoot = rootSibling->child;
				root->sibling = tempRoot;
			}
			else  {
				root->sibling = NULL;
			}
			rootSibling->child = root;
			root->parent = rootSibling;
			rootSibling->degree++;
			(*heap)->head = rootSibling;
			
			//function will work again to check if there are more nodes to merge.
			return recMerge(heap, (*heap)->head, (*heap)->head->sibling);
		}
	}
	//if root and its sibling are not equal degreed, recursion will work again with these parameters:
	//same heap, set sibling of root as the root, set sibling of the sibling as the sibling node.
	//by this way, it will search the equality for the next 2 nodes.
	 recMerge(heap, rootSibling, rootSibling->sibling);
}

//heapInsert function takes the heap and values of the node to insert.
//creates a node with the given values and inserts it to the heap as heap's head node.
//then it sends the heap to recmerge function to merge equal degreed nodes.
//if there is no nodes in heap, then it sets the new node as the heaps head.
void heapInsert(heap **heap, int count, char *str, char *fileName) {
	node *tempNode = newNode(count, str, fileName);
	if ((*heap)->head != NULL) {
		tempNode->sibling = (*heap)->head;
		(*heap)->head = tempNode;
		recMerge(heap, (*heap)->head, (*heap)->head->sibling);
	}
	else {
		(*heap)->head = tempNode;
	}
}

////////////////////// DELETION OF A NODE FROM THE HEAP /////////////////////////////////

/*
-there will be 2 heaps after deletion of a node: original heap and the children of deleted node.
-this function will arrange the root nodes of the 2 heaps in ascending order so that the merge function
can merge binomial trees in the heap.
-this function does this work by implementing 2 heaps, then creating a new line of nodes, then adding nodes from 2 heaps
into the new line of nodes according to their degrees.
*/
node *arrangeRoots(heap *source, heap *merge) {
	/*
	this function works like a linked list merging and sorting function.
	variables are needed to travel around 2 heaps (headSource and headMerge)
	to link new nodes into the newly created line of sorted nodes (builder)
	and to be the first node of the newly created line of sorted nodes. (newHeapHead)
	*/
	node *headSource, *headMerge, *builder, *newHeapHead;
	headSource = source->head;
	headMerge = merge->head;
	
	//if any of these heaps are empty, function will return the other.
	if (headSource == NULL && headMerge != NULL)
		return headMerge;
	if (headMerge == NULL && headSource != NULL)
		return headSource;
	
	/*
	-in this part function will select the minimum degreed node to 
	set it as the first node of the sorted list.
	*/
	if (headSource->degree <= headMerge->degree) {
		newHeapHead = headSource;
		builder = newHeapHead;
		headSource = headSource->sibling;
	}
	else {
		newHeapHead = headMerge;
		builder = newHeapHead;
		headMerge = headMerge->sibling;
	}
	/*
	-in the while loop, function will compare 2 root lists and find
	the minimum node. add the minimum node to the sorted list and travel to the next node in root list.
	-after a while, one of the root lists will end. then function will continue adding from other root list.
	-when both root lists are ended, function will return the new degree-sorted list to the merging function.
	*/
	while (headMerge != NULL || headSource != NULL) {
		/*
		-if any of the root lists has ended, function will start to link nodes 
		to the sorted list, from the non-empty root list.
		*/
		if (headMerge != NULL && headSource == NULL) {
			while (headMerge != NULL) {
				builder->sibling = headMerge;
				builder = builder->sibling;
				headMerge = headMerge->sibling;
			}
			//when the non-empty root list is empty, function will return the sorted list's first node.
			return newHeapHead;
		}
		else if (headMerge == NULL && headSource != NULL) {
			while (headSource != NULL) {
				builder->sibling = headSource;
				builder = builder->sibling;
				headSource = headSource->sibling;
			}
			//when the non-empty root list is empty, function will return the sorted list's first node.
			return newHeapHead;
		}
		/*
		-if none of the root lists are empty, function will compare both of their head nodes and
		add the minimum head into the sorted list.
		-if both of the root lists emptied out at the same time, while loop will end and function will return
		the sorted list.
		-if only one of the root lists emptied out, while loop will work again to empty out the other root list.
		*/
		if (headSource->degree <= headMerge->degree) {
			builder->sibling = headSource;
			builder = builder->sibling;
			headSource = headSource->sibling;
		}
		else {
			builder->sibling = headMerge;
			builder = builder->sibling;
			headMerge = headMerge->sibling;
		}
	}
	//when both of the root lists are empty, function will return the sorted list's first node.
	return newHeapHead;
}

/*
-this function merges 2 heaps into one line of nodes and returns its first node.
-there will be 2 heaps after deletion of a node: original heap and the children of deleted node.
-this function will merge 2 heaps into one degree sorted heap and then it will merge the nodes with the same degrees,
until there are no more equal degreed nodes to merge anymore.
-I got help from the lesson slides' pseudo codes in this function
*/
node *heapMerge(heap *source, heap *merge) {
	
	/*
	function needs some variables to operate properly.
	there is a variable to determine the first node of the merged binomial heap (newHead)
	there are 3 variables to travel in the sorted but not merged linkedlist (current, prev, sibling)
	these variables will be compared and linked to each other in order to merge the heap.
	*/
	node *newHead, *prev, *current, *sibling;
	//newhead will be the first node of the sorted but not merged linkedlist.
	//function will create this linkedlist using the arrangeRoots function.
	newHead = arrangeRoots(source, merge);
	
	//if the linkedlist is empty, there are no nodes to merge. function will return the empty linkedlist.
	if (newHead == NULL)
		return NULL;
	/*
	previous node will be set to null since there are no nodes before the first nodes.
	current and sibling nodes will be implemented accordingly.
	*/
	prev = NULL;
	current = newHead;
	sibling = newHead->sibling;
	/*
	while loop will search the sorted linkedlist to find equal degreed nodes to merge.
	when the sibling node is null, searching will stop since there are no more nodes to compare.
	*/
	while(sibling != NULL) {
		if (sibling->degree != current->degree || (sibling->sibling != NULL &&
		 sibling->sibling->degree == current->degree)) {
			prev = current;
			current = sibling;
		}
		/*
		-if counted words in the current node is greater than in the sibling,
		function will make sibling as the child of current node. Because this is a maximum heap(greater keys must be the parent root.)
		-current's child will be the sibling of the sibling node.
		*/
		else if (current->count >= sibling->count) {
			//elements of these 2 nodes will be arranged according to their equalness in counted words.
			if (sibling->sibling != NULL)
				current->sibling = sibling->sibling;
			else
				current->sibling = NULL;
			
			if (current->child != NULL) {
				node *tempRoot = current->child;
				sibling->sibling = tempRoot;
			}
			else
				sibling->sibling = NULL;
			
			current->child = sibling;
			sibling->parent = current;
			current->degree++;
		}
		/*
		-if counted words in the current node is less than in the sibling,
		function will make sibling as the parent of current node. Because this is a maximum heap(greater keys must be the parent root.)
		-sibling's child will be the sibling of the current node.
		*/
		else {
			//elements of these 2 nodes will be arranged according to their equalness in counted words.
			if (prev == NULL)
				newHead = sibling;
			else
				prev->sibling = sibling;
			
			if (sibling->child != NULL) {
				node *tempRoot = sibling->child;
				current->sibling = tempRoot;
			}
			else  {
				current->sibling = NULL;
			}
			
			sibling->child = current;
			current->parent = sibling;
			sibling->degree++;
		}
		sibling = current->sibling;
	}
	//function returns the new heap's first node.
	return newHead;
}

/*
-this function finds the node with the maximum counted words and removes it from the heap.
-returns the maximum value.
-I got help from the lesson slides' pseudo codes in this function
*/
node *removeNode(heap *originHeap) {
	//variables are implemented as following:
	node *max = originHeap->head;
	node *maxPrev = NULL;
	node *sibling = max->sibling;
	node *siblingPrev = max;

	//while loop will search the heap's roots for the maximum value until there are no more roots to compare.
	while(sibling != NULL)
	{
		if(max->count < sibling->count){
			max = sibling;
			maxPrev = siblingPrev;
		}
		siblingPrev = sibling;
		sibling = sibling->sibling;
	}
	//a new temporary heap is created to include the maximum root's childrem.
	heap *tempHeap = newHeap();
	node *builder = NULL;
	
	//if statement will check if max node is the head or not.
	if (max == originHeap->head)
		originHeap->head = max->sibling;
	else
		maxPrev->sibling = max->sibling;
	
	//max node's children will be inserted into the temporary heap
	node *child = max->child;
	while (child != NULL) {
		node *sibling = child->sibling;
		child->sibling = builder;
		child->parent = NULL;
		builder = child;
		child = sibling;
	}
	//head of child linkedlist set as the head of temporary heap.
	//temporary heap and original heap sent to merging function.
	//function returns the max node.
	tempHeap->head = builder;
	originHeap->head = heapMerge(originHeap, tempHeap);
	return max;
}

//////////////////////// FILE READING - TEXT EDITING ///////////////////////////////

//this function takes an input from user for keyword.
char *inputKeyword() {
	char *keyword = malloc(50);
	printf("Please enter the keyword: ");
	scanf("%s", keyword);
	printf("\n");
	
	return keyword;
}

//readFile function reads the file in the address and stores it into a char array called str.
char *readFile(char *address) {
	int n = 0;
	int c , i;
	int size = 100;
	char *str = malloc(size);
	FILE *fp;
	
   	if ((fp = fopen(address, "r")) == NULL) {
    	printf("Null file. \n");
    	return 0;
	}
	
	//while loop is for checking the given size for the array is enough or not.
	//if it's not enough, size will be doubled.
    while ((c = fgetc(fp)) != EOF) {
	   	if (n >= size/2) {
	   		size *= 2;
	   		str = realloc(str, size);
	   		if (str == NULL) {
	   			printf("Out of memory. \n");
	   			return 0;
			}
		}
		str[n++] = c;
	}
	str[n++] = '\0';
	fclose(fp);
	return str;
}

//this function will edit the stored text (removes some unnecessary markings etc.)
void editText(char *str) {
	int i;

	for (i = 0; i < strlen(str); i++) {
		if (isdigit(str[i]) == 0 && isalpha(str[i]) == 0) {
			if ((i > 0 && i < strlen(str) - 1) && (str[i] == 39 || str[i] == 45) && ((isdigit(str[i - 1]) != 0) || (isalpha(str[i - 1]) != 0)) && ((isdigit(str[i + 1]) != 0) || (isalpha(str[i + 1]) != 0)) )
				continue;

			else if (str[i] == ',' && isdigit(str[i + 1]) != 0)
				continue;
			
			else if (str[i] >= '!' && str[i] <= '~')
				str[i] = ' ';
		}
	}
}

//this function counts the keywords by splitting the stored text by space.
int countWord(char *str, char *keyword) {
	char *token;
	int count = 0;
	token = strtok(str, " ");
	while (token != NULL) {
		if (strcmp(keyword, token) == 0)
			count++;
		token = strtok(NULL, " ");
	}
	return count;
}

//this function will open the directory and search the essential files to read.
void readDir(char *fileName, char *keyword, heap *maxHeap) {
	struct dirent *de;
	DIR *dr = opendir(fileName);
	
	if (dr == NULL) {
		printf("Please put the %s folder to the same path with main code.\n", fileName); 
   	    exit(0);
    }
	
	while ((de = readdir(dr)) != NULL) {
    	if (de->d_name[0] != '.') {
    		
    		int i;
    		
			char address[] = "files/";
    		
    		char *str = readFile(strcat(address, de->d_name));
    		//strCpy is necessary to show user the unedited context of the file.
		    char *strCopy = malloc(strlen(str) + 1);
		    strcpy(strCopy, str);
		    
		    editText(str);
			
			int count = countWord(str, keyword);
			if (count > 0)
				relevant++;
				
			//inserting file informations into the maxHeap.
			heapInsert(&maxHeap, count, strCopy, de->d_name);
			
			free(str);
		}
	}
    closedir(dr);
}

///////////////////////// MAIN FUNCTION //////////////////////////////


int main(int argc, char *argv[]) {
	
	//keyword variable is implemented with using inputKeyword function.
	char *keyword = inputKeyword();
	
	//a maxheap is created to count relevant files with the keyword.
	heap *relevanceHeap = newHeap();
	readDir("files", keyword, relevanceHeap);
	
	//a maxheap is created to print context of the relevant files.
	heap *contextHeap = newHeap();
	readDir("files", keyword, contextHeap);
	
	
	//if there are relevant files, program will print them. (max 5)
	if (relevant > 0) {
		printf("There is/are %d relevant document(s).\n\n", relevant/2);
		printf("The relevance order is: ");
		//int a is for printing ',' after the names.
		int a, i;
		//this for loop will print relevant files's names.
		for (i = 0; i < 5; i++) {
			node *max = removeNode(relevanceHeap);
			if (max->fileName != NULL && max->count != 0) {
				if (i > 0 && a == 1) {
					printf(", ");
					a = 0;
				}
				a = 1;
				printf("%s(%d)", max->fileName, max->count);
			}
		}
		printf(".\n\n");
		//this for loop will print context of these files.
		for (i = 0; i < 5; i++) {
			node *max = removeNode(contextHeap);
			if (max->fileName != NULL && max->count != 0)
				printf("%s(%d): %s \n", max->fileName, max->count, max->str);
		}
	}
	else
		printf("There are no relevant documents.\n");
	
	
	
	return 0;
}
