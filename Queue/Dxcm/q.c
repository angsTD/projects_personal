header
/* q_t is a pointer to a structure that we know nothing about */
typedef struct q_private_t* q_t;

source
/* Here we provide details to the compiler about what structure contains */
typedef struct q_private_t
{
  unsigned add; /* Position at which to insert the next q element */
  unsigned rem; /* Position at which element can be dequeued */
  unsigned max; /* Maximum number of elements which can be stored */
  unsigned num; /* Number of elements in the queue right now */
  void*    ptrs[1]; /* array of pointers follows the header information.
                       Array must contain at least 1 element but we will
                       index past this if additional memory is provided */
} q_obj_t;

/* This macro helps clients define memory of the required size for a specifed
   number of managed pointers */
#define Q_REQUIRED_MEM_SIZE(num) (sizeof(q_obj_t) + num*sizeof(void*))

unsigned q_overhead_bytes(void)
{
  return sizeof(q_obj_t);
}

q_t q_init(void* memory, unsigned size)
{
  if (!memory) return NULL;
  if (memory & 3) return NULL;

  if (size < (sizeof(q_obj_t))) return NULL;
  
  q_obj_t *q = (q_obj_t*)memory;
  size -= (sizeof(*q) - sizeof(q->ptrs)); /* Remove most of the header from the
                                             provided memory. q->ptrs __is__ the
                                             rest of the memory */
  q->add = q->rem = q->num = 0;
  q->max = size / sizeof(void*);
}

int q_insert(q_t q, void* ptr)
{
  if (q->num == q->max) return -1;
  q->ptrs[(q->add)++] = ptr;
  if (q->add == q->num) q->add = 0;
  q->num += 1;
}

int q_dequeue(q_t q, void** ptr)
{
  if (!q) return -1;
  if (!ptr) return -2;
  if (!q->num) return -3;
  *ptr = q->ptrs[(q->rem)++];
  if (q->rem == q->num) q->rem = 0;
  q->num -= 1;
  return ptr;
}

/* client */
static uint8_t _mem[Q_REQUIRED_MEM_SIZE(_NUM_REQ)] __attribute__ ((aligned(4)));

void init()
{
  q_t q = q_init(_mem, sizeof(_mem));
}
