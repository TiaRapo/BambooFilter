#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>

class Segment {
    public:
        // TODO: Figure out function parameters that some of these need
        Segment(/*TODO*/);

        ~Segment();

        bool Insert(uint32_t fingerprint, uint32_t index_bucket);
        bool Lookup(/*TODO*/) const;
        bool Delete(/*TODO*/);
        // TODO: Probably needs more public functions

    private:
        // TODO: Internal helper functions ?

        // TODO: Attributes
};

#endif