#pragma once
#include "rte/GlmWrapper.hpp"
#include "rte/Colors.hpp"
#include "rte/RenderStructs.h"
#include <vector>

namespace RTE::Rendering
{

struct Line
{
    Vertex Start;
    Vertex End;
};

class LineDebugDrawModule
{
private:
    std::vector<Line> _lines;
public:
    LineDebugDrawModule()
    {

    }

    std::vector<Line> GetLineData()
    {
        RecordLines();
        return _lines;
    }

    void RecordLine(Line &line)
    {
        _lines.push_back(line);
    }

    void ClearData()
    {
        _lines.clear();
    }

    virtual void RecordLines() = 0;
};

}