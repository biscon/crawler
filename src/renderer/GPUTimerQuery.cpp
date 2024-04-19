//
// Created by bison on 17-04-24.
//

#include "GPUTimerQuery.h"

namespace Renderer {

    GPUTimerQuery::GPUTimerQuery() {
        glGenQueries(1, &queryID);
    }

    void GPUTimerQuery::start() {
        // Start timestamp query
        glBeginQuery(GL_TIME_ELAPSED, queryID);
    }

    void GPUTimerQuery::stop() {
        // End timestamp query
        glEndQuery(GL_TIME_ELAPSED);
    }

    float GPUTimerQuery::getElapsedTime() {
        GLuint64 elapsedTime;
        glGetQueryObjectui64v(queryID, GL_QUERY_RESULT, &elapsedTime);
        return (float) ((double) elapsedTime * 1.0e-6); // Convert nanoseconds to milliseconds
    }

    GPUTimerQuery::~GPUTimerQuery() {
        glDeleteQueries(1, &queryID);
    }
}