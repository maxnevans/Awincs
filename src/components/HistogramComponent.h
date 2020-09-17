#pragma once
#include "pch.h"
#include "../Component.h"
#include "../Core.h"

namespace Awincs
{
    class HistogramComponent
        :
        public Component
    {
    public:
        template<typename T>
        struct AxisRange
        {
            T min;
            T max;
        };

    public:
        HistogramComponent();
        HistogramComponent(const Component::Point& a, const Component::Dimensions& d);
        void setXAxis(double min, double max, const std::wstring& caption);
        void setYAxis(size_t min, size_t max, const std::wstring& caption);
        void setXAxisMaxCaption(const std::wstring& caption);
        void setYAxisMaxCaption(const std::wstring& caption);
        void setData(const std::vector<size_t>& columnsData);

    private:
        virtual void draw(gp::Graphics& gfx) const override;

    private:
        std::vector<size_t> m_ColumnsData = {};
        AxisRange<double> xAxisRange = {};
        AxisRange<size_t> yAxisRange = {};
        std::wstring xAxisCaption;
        std::wstring yAxisCaption;
        std::wstring m_XAxisMaxCaption;
        std::wstring m_YAxisMaxCaption;
    };
}