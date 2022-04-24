struct Scale {
    const char* name;
    unsigned int length;
    unsigned int notes[10];
};

Scale scales[] = {
    {"Major", 7, {0, 2, 4, 5, 7, 9, 11}},
    {"Minor", 7, {0, 2, 3, 5, 7, 8, 10}},
};

const char* notes[12] = {"C", "C#", "D", "D#", "E", "F",
                         "F#", "G", "G#", "A", "A#", "H"};