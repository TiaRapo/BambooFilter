#ifndef SRC_SEGMENT_H
#define SRC_SEGMENT_H

#include "utility.h"

class Segment {
    public:
        // TODO: Figure out function parameters that some of these need
        Segment();

        ~Segment();

        bool Insert();
        bool Lookup() const;
        bool Delete();
        // TODO: Probably needs more public functions

    private:
        // TODO: Internal helper functions ?

        // TODO: Attributes
};

#endif