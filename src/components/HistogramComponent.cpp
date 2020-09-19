#include "pch.h"
#include "HistogramComponent.h"
#include "Core.h"

namespace Awincs
{
    HistogramComponent::HistogramComponent()
        :
        HistogramComponent({}, {})
    {
    }

    HistogramComponent::HistogramComponent(const Component::Point& a, const Component::Dimensions& d)
        :
        Component(a, d)
    {
    }

    void HistogramComponent::setXAxis(double min, double max, const std::wstring& caption)
    {
        xAxisRange = {min, max};
        xAxisCaption = caption;
    }

    void HistogramComponent::setYAxis(size_t min, size_t max, const std::wstring& caption)
    {
        yAxisRange = { min, max };
        yAxisCaption = caption;
    }

    void HistogramComponent::setXAxisMaxCaption(const std::wstring& caption)
    {
        m_XAxisMaxCaption = caption;
    }

    void HistogramComponent::setYAxisMaxCaption(const std::wstring& caption)
    {
        m_YAxisMaxCaption = caption;
    }

    void HistogramComponent::setData(const std::vector<size_t>& columnsData)
    {
        m_ColumnsData = columnsData;
    }

    void HistogramComponent::draw(gp::Graphics& gfx) const
    {
        const int padding = 30;
        const int paddingFromAxisEnd = 20;
        const int paddingFromAxisStart = 50;

        auto [x, y] = p_transformToGlobal(Point{ 0, 0 });
        auto [width, height] = p_getDimensions();

        // Background
        gfx.FillRectangle(&gp::SolidBrush{ gp::Color(235, 235, 235) }, gp::Rect{ x, y, width, height });

        // Data coordinates and Y-axis X coordinate calculation
        int yAxisXCoord = 0;
        int xDataStart = 0;
        int xDataEnd = 0;

        if (isLess(xAxisRange.max, 0.0))
        {
            yAxisXCoord = x + width - padding - paddingFromAxisEnd;
            xDataStart = x + padding;
            xDataEnd = x + width - padding - paddingFromAxisEnd - paddingFromAxisStart;
        }
        else if (isGreater(xAxisRange.min, 0.0))
        {
            yAxisXCoord = x + padding;
            xDataStart = x + padding + paddingFromAxisStart;
            xDataEnd = x + width - padding - paddingFromAxisEnd;
        }
        else if (areEqual(xAxisRange.min, 0.0))
        {
            yAxisXCoord = x + padding;
            xDataStart = x + padding;
            xDataEnd = x + width - padding - paddingFromAxisEnd;
        }
        else if (areEqual(xAxisRange.max, 0.0))
        {
            yAxisXCoord = x + width - padding - paddingFromAxisEnd;
            xDataStart = x + padding;
            xDataEnd = x + width - padding - paddingFromAxisEnd;
        }
        else
        {
            const auto availableWidthForXAxis = width - 2 * padding - 2 * paddingFromAxisEnd;
            const auto totalXRange = xAxisRange.max - xAxisRange.min;
            const auto rangeFromZero = -xAxisRange.min;
            const auto widthFactor = rangeFromZero / totalXRange;

            yAxisXCoord = x + padding + paddingFromAxisEnd + static_cast<int>(widthFactor * availableWidthForXAxis);
            xDataStart = x + padding + paddingFromAxisEnd;
            xDataEnd = x + width - padding - paddingFromAxisEnd;
        }

        // Data drawing
        if (m_ColumnsData.size() != 0)
        {
            const auto yAxisAvailableHeight = height - 2 * padding - paddingFromAxisEnd;
            const auto columnWidth = (xDataEnd - xDataStart) / static_cast<int>(m_ColumnsData.size());

            auto xColumn = xDataStart;
            for (auto value : m_ColumnsData)
            {
                auto columnHeight = static_cast<int>(yAxisAvailableHeight * (static_cast<double>(value) / yAxisRange.max));
                auto yColumn = y + padding + paddingFromAxisEnd + yAxisAvailableHeight - columnHeight;
                gfx.FillRectangle(&gp::SolidBrush{ gp::Color(135, 78, 173) }, gp::Rect{ xColumn, yColumn, columnWidth, columnHeight });
                gfx.DrawRectangle(&gp::Pen(gp::Color::Gray, 0.5), gp::Rect{ xColumn, yColumn, columnWidth, columnHeight });
                xColumn += columnWidth;
            }
        }

        // Y-axis
        gfx.DrawLine(&gp::Pen(gp::Color(51, 51, 51), 1),
            gp::Point{ yAxisXCoord, y + padding },
            gp::Point{ yAxisXCoord, y + height - padding });
        gfx.DrawLine(&gp::Pen(gp::Color(51, 51, 51), 1),
            gp::Point{ yAxisXCoord, y + padding },
            gp::Point{ yAxisXCoord - 3, y + padding + 10 });
        gfx.DrawLine(&gp::Pen(gp::Color(51, 51, 51), 1),
            gp::Point{ yAxisXCoord, y + padding },
            gp::Point{ yAxisXCoord + 3, y + padding + 10 });

        // X-axis
        gfx.DrawLine(&gp::Pen(gp::Color(51, 51, 51), 1),
            gp::Point{ x + padding, y + height - padding },
            gp::Point{ x + width - padding, y + height - padding });
        gfx.DrawLine(&gp::Pen(gp::Color(51, 51, 51), 1),
            gp::Point{ x + width - padding - 10, y + height - padding - 3 },
            gp::Point{ x + width - padding, y + height - padding });
        gfx.DrawLine(&gp::Pen(gp::Color(51, 51, 51), 1),
            gp::Point{ x + width - padding - 10, y + height - padding + 3 },
            gp::Point{ x + width - padding, y + height - padding });

        //Y-axis caption
        gfx.DrawString(yAxisCaption.c_str(), static_cast<INT>(yAxisCaption.size()),
            &gp::Font{ L"Tahoma", 10 },
            gp::PointF{ (gp::REAL)yAxisXCoord + 10, (gp::REAL)y + (gp::REAL)padding },
            &gp::StringFormat(),
            &gp::SolidBrush(gp::Color(51, 51, 51)));
        auto yMaxStringFormat = gp::StringFormat();
        yMaxStringFormat.SetAlignment(gp::StringAlignment::StringAlignmentFar);
        yMaxStringFormat.SetLineAlignment(gp::StringAlignment::StringAlignmentFar);
        gfx.DrawString(m_YAxisMaxCaption.c_str(), static_cast<INT>(m_YAxisMaxCaption.size()),
            &gp::Font{ L"Tahoma", 10 },
            gp::PointF{ (gp::REAL)yAxisXCoord - 2, (gp::REAL)y + (gp::REAL)padding + paddingFromAxisEnd },
            &yMaxStringFormat,
            &gp::SolidBrush(gp::Color(51, 51, 51)));

        //X-axis caption
        auto stringFormat = gp::StringFormat();
        stringFormat.SetAlignment(gp::StringAlignment::StringAlignmentCenter);
        stringFormat.SetLineAlignment(gp::StringAlignment::StringAlignmentFar);

        gfx.DrawString(xAxisCaption.c_str(), static_cast<INT>(xAxisCaption.size()),
            &gp::Font{ L"Tahoma", 10 },
            gp::PointF{ (gp::REAL)x + width - padding, (gp::REAL)y + height - padding - 5 },
            & stringFormat,
            &gp::SolidBrush(gp::Color(51, 51, 51)));

        // X-axis lower bound caption
        if (!areEqual(xAxisRange.min, 0.0))
        {
            auto xCaptionStringFormat = gp::StringFormat();
            xCaptionStringFormat.SetAlignment(gp::StringAlignment::StringAlignmentNear);
            xCaptionStringFormat.SetLineAlignment(gp::StringAlignment::StringAlignmentNear);

            std::wstringstream ss;
            ss << std::setprecision(4) << xAxisRange.min;
            auto xLowerCaption = ss.str();

            gfx.DrawString(xLowerCaption.c_str(), static_cast<INT>(xLowerCaption.size()),
                &gp::Font{ L"Tahoma", 10 },
                gp::PointF{ (gp::REAL)xDataStart, (gp::REAL)y + height - padding + 5 },
                &xCaptionStringFormat,
                &gp::SolidBrush(gp::Color(51, 51, 51)));
        }

        // X-axis higher bound caption
        if (!areEqual(xAxisRange.max, 0.0))
        {
            auto xCaptionStringFormat = gp::StringFormat();
            xCaptionStringFormat.SetAlignment(gp::StringAlignment::StringAlignmentFar);
            xCaptionStringFormat.SetLineAlignment(gp::StringAlignment::StringAlignmentNear);

            std::wstringstream ss;
            ss << std::setprecision(4) << xAxisRange.max;
            auto xHigherBoundCaption = ss.str();

            gfx.DrawString(xHigherBoundCaption.c_str(), static_cast<INT>(xHigherBoundCaption.size()),
                &gp::Font{ L"Tahoma", 10 },
                gp::PointF{ (gp::REAL)xDataEnd, (gp::REAL)y + height - padding + 5 },
                & xCaptionStringFormat,
                & gp::SolidBrush(gp::Color(51, 51, 51)));
        }
        

        //Zero of axices
        auto zstringFormat = gp::StringFormat();
        zstringFormat.SetAlignment(gp::StringAlignment::StringAlignmentCenter);
        zstringFormat.SetLineAlignment(gp::StringAlignment::StringAlignmentNear);

        gfx.DrawString(L"0", static_cast<INT>(1),
            &gp::Font{ L"Tahoma", 10 },
            gp::PointF{ (gp::REAL)yAxisXCoord, (gp::REAL)y + height - padding },
            & zstringFormat,
            &gp::SolidBrush(gp::Color(51, 51, 51)));
    }
}
