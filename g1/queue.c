#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

/* return number of elements in queue */
int length(QNode* queue) {
  int theLength = 0; 
    if (queue != NULL) { //There is at least 1 element
      QNode* current = queue;  
      ++theLength;
        while (queue != current->link) { //While there are more different elements
            ++theLength;
            current = current->link;
        }
    }
    return theLength;
}

/* add element at rear end */
void enqueue(QNode** queue, Data el) {
    QNode *QNode;
    QNode = malloc( sizeof(QNode) );
    if (QNode == NULL){
        printf("Out of memory");
        exit(0);
    }
    QNode->content = el; 
    if (*queue == NULL) //if queue is empty 
        QNode->link = QNode;
    else { 
        QNode->link = (*queue)->link;
	(*queue)->link = QNode;
    } 
    *queue = QNode;
}

/* remove and return front element */
Data dequeue(QNode** queue) {
    QNode* current = (*queue)->link;
    Data el = current->content;
    if ((*queue)->link == current->link) {
      *queue = NULL;
    }
    else { 
      (*queue)->link = current->link;
    }
    free(current);
    return el;
}

int main () {
  QNode* testNode = NULL;
  printf("laengde =%d\n", length(testNode));
  enqueue (&testNode, "esjlf");
  printf("laengde =%d\n", length(testNode));
  enqueue (&testNode, "akfhdfkgjh");
  printf("laengde =%d\n", length(testNode));

  printf("vaerdi =%s\n", (char*) dequeue(&testNode));
  printf("laengde =%d\n", length(testNode));
  printf("vaerdi =%s\n", (char*) dequeue(&testNode));
  printf("laengde =%d\n", length(testNode));

  
  return 0;
}
