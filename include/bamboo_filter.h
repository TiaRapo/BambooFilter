#ifndef SRC_BAMBOOFILTER_H
#define SRC_BAMBOOFILTER_H

#include "utility.h"
#include "segment.h"

class BambooFilter {
    public:
        // TODO: Figure out function parameters that some of these need
        BambooFilter();

        ~BambooFilter();

        bool Insert();
        bool Lookup() const;
        bool Delete();
        void Extend();
        void Compress();

    private:
        // TODO: Internal helper functions ?

        // TODO: Attributes
};

#endif