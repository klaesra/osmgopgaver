#include <stdlib.h>
#include "queue.h"

int length(QNode* queue) {
    int theLength = 0;
    QNode* current = (*queue)->link;
    if (queue != NULL) { //There is at least 1 element
        ++theLength;
        while (queue != current) { //While there are more different elements
            ++theLength;
            current = (*current)->link;
        }
    }
    return theLength
}

void enqueue(QNode** queue, Data el) {
    QNode *QNode;
    QNode = malloc( sizeof(QNode) );
    QNode->content = el;
    if (*queue == NULL) 
        QNode->link = QNode;
    else
        QNode->link = (*queue)->link;
    *queue = QNode;
}

Data dequeue(QNode** queue) {
    QNode current = (*queue)->link;
    Data el = current->content;
    if ((*queue)->link == current->link)
        (*queue)->link = NULL;
    else 
        (*queue)->link = current->link;
    free(current);
    return el;
}

