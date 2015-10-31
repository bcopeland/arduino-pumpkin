/*
 * Arduino sketch to control some pumpkin lights.
 * (c) 2015 Bob Copeland
 * GPLv2 or later.
 */
#define MINVAL 0
#define MAXVAL 255
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define SEQ_INT     100

enum pattern_type {
    PATTERN_FADE,
    PATTERN_RAND,
    PATTERN_SEQ,
    NUM_PATTERNS,
};

struct pumpkin
{
    int pin;
    enum pattern_type pattern;
    int curval;

    int fade_curval;
    int fade_dir;

    int seq_ct;
};

struct pumpkin pumpkins[] = {
    {
        .pin = 9,
        .pattern = PATTERN_FADE,
        .curval = 0,
        .fade_curval = MINVAL + 1,
        .fade_dir = 1,
        .seq_ct = 0,
    },
    {
        .pin = 10,
        .pattern = PATTERN_FADE,
        .curval = 0,
        .fade_curval = MAXVAL / 2,
        .fade_dir = 1,
        .seq_ct = SEQ_INT,
    },
    {
        .pin = 11,
        .pattern = PATTERN_FADE,
        .curval = 0,
        .fade_curval = MAXVAL - 1,
        .fade_dir = -1,
        .seq_ct = 2 * SEQ_INT,
    },
};

void fade_next(struct pumpkin *p)
{
    if (p->fade_curval == MAXVAL || p->fade_curval == MINVAL)
        p->fade_dir = -p->fade_dir;

    p->fade_curval += p->fade_dir;
    p->curval = p->fade_curval;
}

void rand_next(struct pumpkin *p)
{
    p->curval = (rand() & 1) ? MAXVAL : MINVAL;
}

void seq_next(struct pumpkin *p)
{
    p->seq_ct = (p->seq_ct + 1) % (3 * SEQ_INT);
    p->curval = (p->seq_ct < SEQ_INT) ? MAXVAL : MINVAL;
}

void (*callbacks[])(struct pumpkin *p) = {
    fade_next,
    rand_next,
    seq_next,
};

void setup()
{
    unsigned int i;
    for (i=0; i < ARRAY_SIZE(pumpkins); i++) {
        pinMode(pumpkins[i].pin, OUTPUT);
    }
}

static long t;
void loop()
{
    unsigned int i;
    for (i=0; i < ARRAY_SIZE(pumpkins); i++) {
        callbacks[pumpkins[i].pattern](&pumpkins[i]);
        analogWrite(pumpkins[i].pin, pumpkins[i].curval);
    }
    delay(5);
    t += 5;

    if (t > 10000) {
        t = 0;
        for (i=0; i < ARRAY_SIZE(pumpkins); i++) {
            pumpkins[i].pattern = (enum pattern_type)
                ((pumpkins[i].pattern + 1) % (int) NUM_PATTERNS);
        }
    }
}
