#ifndef INCLUDE_QUEUE_H
#define INCLUDE_QUEUE_H

struct QueueElement;
typedef QueueElement* QueueAddress;

struct QueueInfoType {};

struct QueueElement {
  QueueInfoType info;
  QueueAddress next;
  QueueAddress prev;
};

struct Queue {
  QueueAddress head;
  QueueAddress tail;
};

/**
 * Menginisialisasi queue
 * I.S : sebuah quueue
 * F.S : element head dan tail pada queue akan menjadi "nullptr"
 */
void initializeQueue(Queue& queue);

/**
 * Mengecek apakah queue kosong, memberikan "true" jika queue
 * kosong, dan memberikan "false" jika tidak kosong.
 */
bool isQueueEmpty(Queue queue);

/**
 * Memberikan element pada posisi terdepan queue, memberikan
 * address element jika ada, dan "nullptr" jika queue kosong.
 * Dan ini tidak mengurangi isi antrian, hanya memberikan
 * address antrian terdepan.
 */
QueueAddress frontQueue(Queue queue);

/**
 * Memberikan berapa banyak element pada queue.
 */
int sizeQueue(Queue queue);

/**
 * Menambahkan antrian element pada "queue"
 * I.S : queue dan element yang ingin ditambahkan
 * F.S : element sudah pada antrian
 */
void enqueue(Queue& queue, QueueAddress new_queue);

/**
 * Memberikan antrian terdepan dan mengurangi antrian
 * terdepan tapi tidak menghancurkannya (dealokasi).
 */
QueueAddress dequeue(Queue& queue);

#endif // INCLUDE_QUEUE_H
