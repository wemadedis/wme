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
        Line l;
        Vertex v;
        v.Position = glm::vec3(0.0, 0.0, -20.0);
        l.Start = v;
        v.Position = glm::vec3(0.0, 100.0, 100.0);
        l.End = v;
        _lines.push_back(l);
        v.Position = glm::vec3(0.0, 0.0, -40.0);
        l.End = v;
        _lines.push_back(l);
        _lines.push_back(l);
        _lines.push_back(l);
        _lines.push_back(l);
        _lines.push_back(l);
        _lines.push_back(l);
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