# CBinomialHeap
### Using binomial heap structure to implement a priority queue for the task of ranking documents.

## Explanation
In web, when you enter a “keyword” search engines try to rank the documents according to the similarities between your keyword and the documents lying on web. The most famous search engines (Google, Bing, Yahoo etc.) run various machine learning and deep learning algorithms. On the other hand, the common logic behind them is to rank the documents according to relevance.

This project uses binomial heaps to implement a priority queue for the task of ranking documents. In other words, it ranks documents according to given keyword with binomial heaps.

## Usage
There are 42 files in "DataStrcutersProject\files". General content of these text documents are some comments about gadgets and brands.
1. Console interface takes input key from user in order to find most relevant documents in files.
2. According to given input, program starts to rank all documents by calculating frequencies of the input in text documents which are already cleaned and prepared by the program.
3. After ranking process, program builds a priority queue using binomial heaps based on rankings.
4. Program prints counts of keys with their file in descending order.
P.S: 'files' folder needs to be in the same folder with 'main.c'.
