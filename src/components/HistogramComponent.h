#pragma once

#include "pch.h"

#include "Component.h"

namespace Awincs
{
    class HistogramComponent
        :
        public Component
    {
    public:
        using ValueKey = size_t;

        class KeyGenerator
        {
        public:
            KeyGenerator(ValueKey seed = 0);
            ValueKey operator()();

        private:
            ValueKey m_CurrentValue = 0;
        };

        class ColorGenerator
        {
        public:
            ColorGenerator();
            gp::ARGB operator()(bool generateAlpha = false);

        private:
            std::random_device m_Rd;
            std::uniform_int_distribution<size_t> m_ColorDist;
            std::mt19937 m_Gen;
        };

        template<typename T>
        struct ValueRange
        {
            T min;
            T max;
        };

        struct HistogramValue
        {
            gp::ARGB color;
            std::vector<double> values;
            size_t numberColumns;
            ValueRange<double> xRange;
            ValueRange<double> yRange;
            std::vector<size_t> cachedColumns;
            bool shouldUpdateCache;
        };

    public:
        HistogramComponent();
        HistogramComponent(const Component::Point& anchorPoint, const Component::Dimensions& dimensions);
        ValueKey addValue(gp::ARGB color);
        ValueKey addValue();
        void removeValue(const ValueKey& key);
        void setValueColor(const ValueKey& key, gp::ARGB color);
        void setValues(const ValueKey& key, const std::vector<double>& values);
        void unsetValues(const ValueKey& key);
        void setValueNumberColumns(const ValueKey& key, size_t numberColumns);
        void setValueXAxisRange(const ValueKey& key, double min, double max);
        void setValueYAxisRange(const ValueKey& key, int min, int max);
        void clear();
        void calculateValues();

    private:
        virtual void draw(gp::Graphics&) const override;
        void p_CheckValueCache(HistogramValue& value) const;
        void p_UpdateValueCache(HistogramValue& value) const;
        void p_CalculateValues();
        void p_MarkValueForUpdate(HistogramValue& value);

    private:
        static constexpr const int COLUMNS_PADDING = 2;
        static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR = makeARGB(0x10, 0x20, 0x30);
        static constexpr gp::ARGB DEFAULT_AXIS_COLOR = makeARGB(0x10, 0x20, 0x30);
        static constexpr gp::REAL DEFAULT_AXIS_WIDTH = 1.0;

        std::map<size_t, HistogramValue> m_DataMap;
        ValueRange<double> m_TotalXRange = { std::numeric_limits<double>::max(), std::numeric_limits<double>::min() };
        ValueRange<int> m_TotalYRange = { std::numeric_limits<int>::max(), std::numeric_limits<int>::min() };
        bool m_CacheCalculated = false;

        KeyGenerator m_KeyGenerator;
        ColorGenerator m_ColorGenerator;
    };
}