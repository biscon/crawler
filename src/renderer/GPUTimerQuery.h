//
// Created by bison on 17-04-24.
//

#ifndef CRAWLER_GPUTIMERQUERY_H
#define CRAWLER_GPUTIMERQUERY_H

#include "defs.h"

extern "C" {
#include "glad.h"
}

namespace Renderer {
    class GPUTimerQuery {
    public:
        GPUTimerQuery();
        ~GPUTimerQuery();
        void start();
        void stop();
        float getElapsedTime();
    private:
        GLuint queryID;
    };
}


#endif //CRAWLER_GPUTIMERQUERY_H
