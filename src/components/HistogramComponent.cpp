#include "HistogramComponent.h"
#include "ComponentException.h"

namespace Awincs
{
    HistogramComponent::HistogramComponent()
        :
        HistogramComponent({}, {})
    {
    }

    HistogramComponent::HistogramComponent(const Component::Point& anchorPoint, const Component::Dimensions& dimensions)
        :
        Component(anchorPoint, dimensions)
    {
    }

    HistogramComponent::ValueKey HistogramComponent::addValue(gp::ARGB color)
    {
        auto key = m_KeyGenerator();
        m_DataMap[key] = {};
        return key;
    }

    HistogramComponent::ValueKey HistogramComponent::addValue()
    {
        return addValue(m_ColorGenerator());
    }

    void HistogramComponent::removeValue(const ValueKey& key)
    {
        auto valueIterator = m_DataMap.find(key);

        if (valueIterator == m_DataMap.end())
            throw ComponentException(L"removed value does not exist");

        m_DataMap.erase(valueIterator);
    }

    void HistogramComponent::setValueColor(const ValueKey& key, gp::ARGB color)
    {
        m_DataMap.at(key).color = color;
    }

    void HistogramComponent::setValues(const ValueKey& key, const std::vector<double>& values)
    {
        auto& histValue = m_DataMap.at(key);

        if (histValue.values == values)
            return;

        p_MarkValueForUpdate(histValue);

        histValue.values = values;
        std::sort(histValue.values.begin(), histValue.values.end());
    }

    void HistogramComponent::unsetValues(const ValueKey& key)
    {
        auto& histValue = m_DataMap.at(key);

        if (histValue.values.size() == 0)
            return;

        p_MarkValueForUpdate(histValue);

        histValue.values = {};
    }

    void HistogramComponent::setValueNumberColumns(const ValueKey& key, size_t numberColumns)
    {
        auto& histValue = m_DataMap.at(key);

        if (histValue.numberColumns == numberColumns)
            return;

        p_MarkValueForUpdate(histValue);

        histValue.numberColumns = numberColumns;
    }

    void HistogramComponent::setValueXAxisRange(const ValueKey& key, double min, double max)
    {
        expect(min <= max);

        auto& histValue = m_DataMap.at(key);

        if (histValue.xRange.min == min && histValue.xRange.max == max)
            return;

        p_MarkValueForUpdate(histValue);

        if (m_TotalXRange.min >= min && m_TotalXRange.max <= max)
        {
            m_TotalXRange.min = min;
            m_TotalXRange.max = max;
            histValue.xRange = { min, max };
            return;
        }

        bool shouldUpdateMin = histValue.xRange.min == m_TotalXRange.min || m_TotalXRange.min > min;
        bool shouldUpdateMax = histValue.xRange.max == m_TotalXRange.max || m_TotalXRange.max < max;

        histValue.xRange = {min, max};

        if (shouldUpdateMin || shouldUpdateMax)
        {
            m_TotalXRange.min = std::numeric_limits<double>::max();
            m_TotalXRange.max = std::numeric_limits<double>::min();

            for (const auto& [key, value] : m_DataMap)
            {
                if (value.xRange.min < m_TotalXRange.min)
                    m_TotalXRange.min = value.xRange.min;

                if (value.xRange.max > m_TotalXRange.max)
                    m_TotalXRange.max = value.xRange.max;
            }
        }
    }

    void HistogramComponent::setValueYAxisRange(const ValueKey& key, int min, int max)
    {
        expect(min <= max);
        
        auto& histValue = m_DataMap.at(key);

        if (histValue.yRange.min == min && histValue.yRange.max == max)
            return;

        p_MarkValueForUpdate(histValue);

        if (m_TotalYRange.min >= min && m_TotalYRange.max <= max)
        {
            m_TotalYRange.min = min;
            m_TotalYRange.max = max;
            histValue.yRange = { min, max };
            return;
        }

        bool shouldUpdateMin = histValue.yRange.min == m_TotalYRange.min || m_TotalYRange.min > min;
        bool shouldUpdateMax = histValue.yRange.max == m_TotalYRange.max || m_TotalYRange.max < max;

        histValue.yRange = { min, max };

        if (shouldUpdateMin || shouldUpdateMax)
        {
            m_TotalYRange.min = std::numeric_limits<double>::max();
            m_TotalYRange.max = std::numeric_limits<double>::min();

            for (const auto& [key, value] : m_DataMap)
            {
                if (value.yRange.min < m_TotalYRange.min)
                    m_TotalYRange.min = value.yRange.min;

                if (value.yRange.max > m_TotalYRange.max)
                    m_TotalYRange.max = value.yRange.max;
            }
        }
    }

    void HistogramComponent::clear()
    {
        m_DataMap = {};
        m_TotalXRange = { std::numeric_limits<double>::max(), std::numeric_limits<double>::min() };
        m_TotalYRange = { std::numeric_limits<int>::max(), std::numeric_limits<int>::min() };
        m_CacheCalculated = true;
    }

    void HistogramComponent::calculateValues()
    {
        p_CalculateValues();
    }

    void HistogramComponent::draw(gp::Graphics& gfx) const
    {
        expect(m_CacheCalculated);

        const int padding = 10;

        auto [x, y] = p_transformToGlobal(Point{ 0, 0 });
        auto [width, height] = p_getDimensions();

        gfx.FillRectangle(&gp::SolidBrush{ gp::Color(DEFAULT_BACKGROUND_COLOR) }, gp::Rect{ x, y, width, height });

        // Y-axis
        gfx.DrawLine(&gp::Pen(gp::Color(DEFAULT_AXIS_COLOR), DEFAULT_AXIS_WIDTH), 
            gp::Point{ x + padding, y + padding }, 
            gp::Point{ x + padding, y + height - 2 * padding });

        // X-axis
        gfx.DrawLine(&gp::Pen(gp::Color(DEFAULT_AXIS_COLOR), DEFAULT_AXIS_WIDTH),
            gp::Point{ x + padding, y + height - 2 * padding },
            gp::Point{ x + width - 2 * width, y + height - 2 * padding });

        const int paddingFromAxis = 10;

        const auto maxXAxis = width - 2 * padding - paddingFromAxis;
        const auto maxYAxis = height - 2 * padding - paddingFromAxis;

        for (const auto& [key, value] : m_DataMap)
        {
            value.numberColumns
        }
    }

    void HistogramComponent::p_CheckValueCache(HistogramValue& value) const
    {
        if (value.shouldUpdateCache)
        {
            p_UpdateValueCache(value);
            value.shouldUpdateCache = false;
        }
    }

    void HistogramComponent::p_UpdateValueCache(HistogramValue& value) const
    {
        double xRange = value.xRange.max - value.xRange.min;
        double stepSize = xRange / value.numberColumns;

        double x = value.xRange.min;
        for (size_t i = 0; i < value.numberColumns; i++)
        {
            size_t countItems = 0;
            for (const auto& item : value.values)
            {
                if (item < value.xRange.min)
                    continue;

                if (item > value.xRange.max)
                    break;

                countItems++;
            }

            value.cachedColumns[i] = countItems;
        }
    }

    void HistogramComponent::p_CalculateValues()
    {
        for (auto& value : m_DataMap)
            p_CheckValueCache(value.second);

        m_CacheCalculated = true;
    }

    void HistogramComponent::p_MarkValueForUpdate(HistogramValue& value)
    {
        value.shouldUpdateCache = true;
        m_CacheCalculated = false;
    }

    HistogramComponent::KeyGenerator::KeyGenerator(ValueKey seed)
        :
        m_CurrentValue(seed)
    {
    }

    HistogramComponent::ValueKey HistogramComponent::KeyGenerator::operator()()
    {
        return m_CurrentValue++;
    }

    HistogramComponent::ColorGenerator::ColorGenerator()
        :
        m_ColorDist(0, 0xFFFFFFFF),
        m_Gen(m_Rd())
    {
    }

    gp::ARGB HistogramComponent::ColorGenerator::operator()(bool generateAlpha)
    {
        return m_ColorDist(m_Gen) | ((0xff << ALPHA_SHIFT) * !generateAlpha);
    }
}